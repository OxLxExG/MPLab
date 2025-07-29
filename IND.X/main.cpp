/* 
    FUSES:
    BOOTSiZE 0x10
    CODSIZE  0x60

 */ 
#include <stddef.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <ccp.h>
#include <tools.h>
#include <Indicator.hpp>
#include <flash_n.hpp>
#include <getarg.h>
#include <usarts.hpp>
#include <mt29f4g.hpp>
#include <ais2ih.hpp>
#include <flash_n.hpp>
#include "Ind.hpp"
#include <protokol.h>
#include <crc.h>
#include <clock.hpp>
#include <Data.h> // данные для метаданных
#include <MetaData.h> // для создания секции линкера "meta_data" после векторов прерываний

#include <eep_ix.hpp>

using namespace induc;

typedef  struct
{
	uint8_t InclBad;
	uint16_t InclCnt;
	uint8_t MCUSR_Save;
	uint16_t Resetcnt;
} ErrIncl_t __attribute__((aligned));


#define eeprom (*(eep_t*)0x1400)
#define dataprog (*(flash_nand_t*)0x8000)
#define dataprog512 (*(flash_bad_bloks_t*)0x8200)



// *********** RTC correction and 2.097152 interval
// err: 0.00  PER=68717  PRESC=1  Corr=36   10DaysErr : -36.76ms
//   PER = 11 = 335us 6247 times 
//   PER = 6247 190ms 11 times

//
//frac: 0.0029  PER=68717  PRESC=1  Corr=36    10DaysErr : -36.76ms		11 * 6247
//frac: 0.0103  PER=68715  PRESC=1  Corr=65    10DaysErr : -129.01ms   	3 * 3 * 3 * 5 * 509
//frac: 0.0029  PER=34363  PRESC=2  Corr=-95    10DaysErr : -71.74ms 	7 * 4909
//frac: 0.0063  PER=34362  PRESC=2  Corr=-66    10DaysErr : -157.41ms   2 * 3 * 3 * 23 * 83
//frac: 0.0031  PER=17181  PRESC=4  Corr=-66    10DaysErr : -157.41ms ..3 * 3 * 23 * 83

INLN void resetPORTs(void)
{
	PORT_t* p = &PORTA;
	for (uint8_t i = 0; i<6; i++)
	{
		p->PINCONFIG = PORT_ISC_INPUT_DISABLE_gc;
		p->PINCTRLUPD = 0xFF;
		p++;
	}
}

void init3(void) __attribute__ ((naked)) __attribute__ ((section (".init3")));
void init3(void)
{
	resetPORTs();	
	//Indicator.On();
}

#define Com serial1
#define DEF_SPEED 125
#define EEP_OFFSET 8
#define NO_POWER_LEN (sizeof(uint32_t)+sizeof(uint8_t))
#define EEP_UPDATE_KADRS 900 //30min
#define DEFAULT_KADR -100000000
#define WDTO_N WDTO_250MS

EEPROM_T(eep);
INST_CLOCK;

//F_CPU=8000000UL;
//ADDRESS_PROCESSOR=6;
//CLOCK=CLKHT,TA0,CLKNOOUT;
//DIOD=PB,5,INV;
//USEUSART1=DEF0,2048,TB0;
//USEUSART2=DEF0,2048,TB1;
//MT29F4G=SP1,ALT1,UART1,PD,4,PD,5;
//AIS2IH=SP0,DEF0,PA,3;
//INDUC=UART2,PD,3;DEBUG
        
indicator_t<DIOD> Indicator;
static ais2ih_t<AIS2IH> Ais2ih;
static ram_t<MT29F4G> Ram;
static Ind_t<INDUC> Ind;
static uint8_t TurboTimer, TestModeTimer;
static uint8_t saveLen;
static uint8_t curTurbo;
static WorkData_t workData={APP_IDLE,DEFAULT_KADR};
static volatile uint8_t ResetFunction NO_INIT;
static int32_t LastKadrEEPChargeUpdate;

static errors_t err;

// виртуальные секции EEPROM
#define EEP_OFFSET_VOLUME 8 // from 8 len 4
#define EEP_OFFSET_ERR 16 // from 16 len 8
#define EEP_OFFSET_ERR_QZ 16+8 // from 16 len 4
#define EEP_OFFSET_KADR_CHARGE 32 // from 512

//static eep_reis_t CurrentZaezd;
//static uint16_t saveGK;
//const eep_t* eep = (eep_t*) EEPROM_START;

//static void StdkEvent(uint8_t len)
//{
	//if (len > 0)
	//{
		//memcpy(Com.buf, Ind.buf(), len);
		//Com.startTxD(len);
	//}
//}

// СОБЫТИЕ: постановка на задержку пользователем
static void ResetErrorsInEEP(void)
{
	eep_errors_t e = {0,0,0,DEFAULT_KADR,DEFAULT_KADR};
	//eep.Save(EEP_OFFSET_ERR, e, sizeof(eep_errors_t));
    FLASH_write_eeprom_block((eeprom_adr_t) EEP_OFFSET_ERR, (uint8_t*) &e, sizeof(eep_errors_t));							
	while(FLASH_is_eeprom_ready()); 
}
// СОБЫТИЕ: ошибка кварца
static void SaveQzErrInEEP(void)
{
	//eep.Save(EEP_OFFSET_ERR_QZ, &workData.time, 4);
	FLASH_write_eeprom_block((eeprom_adr_t) EEP_OFFSET_ERR_QZ, (uint8_t*) &workData.time, 4);
	while(FLASH_is_eeprom_ready());
}

// СОБЫТИЕ: аномальный ресет
static void SaveResetInEEP(int32_t* restored_kadr)
{
	reset_t r;
	r.ResetCount = eep_errors.reset.ResetCount+1;
	r.ResetFunction = ResetFunction;
	r.kadr_Reset = *restored_kadr;
	r.ResetRegister = RSTCTRL.RSTFR;
	FLASH_write_eeprom_block((eeprom_adr_t) EEP_OFFSET_ERR , (uint8_t*) &r, sizeof(r));
	while(FLASH_is_eeprom_ready()); // пусть будет 
}

static void SaveChargeAndStateToEEP_Async(void)
{
	LastKadrEEPChargeUpdate = workData.time;	
	eep.Save(EEP_OFFSET_KADR_CHARGE, &workData.AppState, sizeof(eep_save_state_and_charge_t));
	//FLASH_write_eeprom_block((eeprom_adr_t) EEP_OFFSET_KADR_CHARGE, (uint8_t*) &workData.AppState, sizeof(eep_save_state_and_charge_t));
}


static void TestWorkEvent(uint8_t len)
{
	WorkData_t* d = (WorkData_t*)(&Com.buf[DATA_POS]);
	//d->gk.gk = saveGK;
	*(WorkDataStd_t*) d = *(WorkDataStd_t*) &workData;
	if (len > 0)
	{
		Induc_t* s = (Induc_t*)(&Ind.buf()[3]);
		memcpy(&d->dat, s, sizeof(Induc_t));
	}
	else
	{
		//Err.InclCnt++;
		//Err.InclBad = 1;
		//Error_Register(2, &Error_handler);
		memset(&d->dat, 0, sizeof(Induc_t));
		d->AppState |= 0x40;
	}
	d->accel.T = Ais2ih.ReadT();
	Ais2ih.ReadXYZ(&d->accel.X);
	//d->dat.gk_m = saveGK * eeprom.GR1.kGK; 
	d->AppState |= 0x80;
	if (Indicator.Error3 || Indicator.Error2) d->AppState |= 0x40;
	Com.CRCSend(saveLen+HEADER_LEN);
}
//static void UpdateDAC()
//{
	//GGKP.initDAC(dataprog.DAC1,dataprog.DAC2);	
//}
static void getMetrEvent(uint8_t len)
{
	if (len >0)
	{
		memcpy(&Com.buf[DATA_POS], &Ind.buf()[3], sizeof(uart_ind_t));
		Com.CRCSend(sizeof(uart_ind_t)+HEADER_LEN);
	}
}
static void setMetrEvent(uint8_t len)
{
	if (len >0)
	{
		Com.CRCSend(HEADER_LEN);
	}
}

static bool CheckWcursor(void)
{
	bool r = Ram.LastReis.reis == eeprom.Currentwreis.reis && Ram.LastReis.startBlock == eeprom.Currentwreis.startBlock;
	if (!r) 
	{
		Indicator.SetCriticalError3();
		err.reis++;
	}
	return r;
}

static void WorkEvent(uint8_t len)
{
	RamData_t d;
	if (len > 0) 
	{
		memcpy(&d.dat, &Ind.buf()[3], sizeof(Induc_t));
		err.induct = 0;
	}
	else 
	{
		memset(&d.dat, 0, sizeof(Induc_t));
		err.induct++;
	}
	d.ramtime = workData.time;
	d.accel.T = Ais2ih.ReadT();
	Ais2ih.ReadXYZ(&d.accel.X);
	if (!Ram.write((uint8_t*) &d, sizeof(RamData_t))) err.ram++;	
}

//static void CheckNANDBadBlocks(uint16_t* bb)
//{
	//RowAddrs a ={0, 0};
	//uint8_t badIdx =0;
	//
	////Ram.chip = die+1;
	//
	//for (uint16_t i =0; i < 2048; i++)
	//{
		//a.block = i;
		//uint8_t bad;
		//Ram.NAND_ReadPage(&a, BAD_BLOCK_BYTE, (uint8_t*)&bad,1);
		//if (bad != 0xFF)
		//{
			//bb[badIdx++] = i;
			//if (badIdx == 40) return;
		//}
	//}
//}
static void BadUpdate(void)
{
	uint16_t bb[40];
	memset(bb,0xFF,sizeof(bb));
	Ram.NAND_CheckBadBlocks( &bb[0]);
	for(uint8_t i= 0; i<40; i++)
	{
		if (bb[i] != dataprog512.bad_blocks[i])
		{
			FLASH_write_flash_n(512, (uint8_t*)&bb, sizeof(bb));
			
			return;
		}
	}
}

static bool NANDCheckFormat(void)
{
	if (Ram.NAND_CheckFormat())
	{
		err.ramForm = 0;
		return true;
	}
	else
	{
		Indicator.SetCriticalError3();
		err.ramForm = 1;
		return false;
	}
}

//typedef ram_t<MT29F4G> ttt;
//void tram(ttt& r)
//{
	//r.chip =0xAA;
//}
static void GoToSleep(void)
{
				BadUpdate();
				Indicator.Error3 = false;
				Indicator.Error2 = false;
				err.cursor = 0;
				Indicator.appSatate = 100;
				if (NANDCheckFormat()) CheckWcursor();
				Ram.rlogadr = 0xFFFFFFFF;
				workData.AppState = APP_IDLE;
				//SetModeALL();
				Ind.off();
}
static void RunCmd(void)
{
	switch (Com.buf[CMD_POS])
	{
		case CMD_TIME_SYNC:
		if (Com.Count == HEADER_LEN + 4 + CRC_LEN)
		{
			int32_t* wait_time = (int32_t*) &Com.buf[DATA_POS];
			//setup sleep timer, stop all peripheral work, goto idle mode
			if (*wait_time == 0)
			{
				GoToSleep();
			}
			else
			{
				Clock.resetTik();
				workData.time = *wait_time;
				if (workData.AppState > APP_DELAY)
				{
					workData.AppState = APP_SET_TIME;
					Ind.off();
				}
			}
		}
		break;
		case CMD_BEACON:
		if (Com.Count == HEADER_LEN + 4 + CRC_LEN)
		{
			Clock.resetTik();
			workData.time = *(uint32_t*)(&Com.buf[DATA_POS]);
			// ведущий модуль уже заработал если данный модуль еще спит то нужно также переходить в работу
			if (workData.AppState != APP_WORK)
			{
				workData.AppState = APP_WORK;
				Ind.on();
				Ram.ResetPageBase();
			}
		}
		break;
		case CMD_TURBO:
			switch (Com.buf[DATA_POS])
			{
				case 1:
				    curTurbo = 1;
					Com.setBaud(500);
					TurboTimer = 4;
				break;
				case 2:
				curTurbo = 2;
				Com.setBaud(1000);
				TurboTimer = 4;
				break;
				case 3:
				curTurbo = 3;
				Clock.MAXInternalClock();
				Clock.HiSpeedReady = true;
				Com.setBaudMaxClock(1500);
				TurboTimer = 4;
				break;
				case 4:
				curTurbo = 4;
				Clock.MAXInternalClock();
				Clock.HiSpeedReady = true;
				Com.setBaudMaxClock(2000);
				TurboTimer = 4;
				break;
				case 6:
				curTurbo = 6;
				Clock.MAXInternalClock();
				Clock.HiSpeedReady = true;
				Com.setBaudMaxClock(3000);
				TurboTimer = 4;
				break;
				default:
					if(curTurbo >= 3) 
					{
						Clock.RestoreExternalClock();
						Clock.HiSpeedReady = false;
					}				
				    curTurbo = 0;
					Indicator.User = false;
					Com.setBaud(DEF_SPEED);
					TurboTimer = 0;
				break;
			}
			if (TurboTimer)
			{
				Indicator.User = true;
				Indicator.Off();
			}
		break;
		case CMD_BOOT:
			if (*(uint32_t*)(&Com.buf[DATA_POS]) == 0x12345678)
			{
				//stop all int , timers
				 cli();
				 Ind.off();
				 FLASH_write_eeprom_byte(0, 0);
				 while(FLASH_is_eeprom_ready());
				 ccp_write_io((void *)&RSTCTRL.SWRR, 1);				 
			}
		break;
		case CMD_ERAM:
		    if(workData.AppState == APP_IDLE) Ram.readAndSendUart(&Com.buf[DATA_POS]);
			err.cursor = Indicator.Error2;
		break;
		//case CMD_ERAM_WRITE:
			//Com.buf[DATA_POS] = Ram.write(&Com.buf[DATA_POS+1], Com.buf[DATA_POS]);
			//Com.CRCSend(HEADER_LEN+1);
		//break;
		//case CMD_ERAM_SET_BASE:
			//Ram.SetWritePage(&Com.buf[DATA_POS]);
			//Com.CRCSend(HEADER_LEN);
		//break;

		case CMD_READ_EE:
		{
			uint8_t n = Com.buf[DATA_POS+2];
			uint16_t from = *(uint16_t*)(&Com.buf[DATA_POS]);
			uint8_t* dptr = &Com.buf[DATA_POS];
			bool sendAsk = true;
			
			if (from == 4096)
			{
				FLASH_read_eeprom_block((eeprom_adr_t) 0, dptr, n);
			}
			else if (from == 1024)
			{
				Ind.getMetr(&getMetrEvent);
				sendAsk = false;
			}
			else if ((from == 2048) )
			{
				if (n <= sizeof(err))
				{
					memcpy(dptr, (const void*) &err, n);
				}
				else 
				{
					memcpy(dptr, (const void*) &err, sizeof(err));
					memcpy(dptr + sizeof(err), (const void*) &Ram.LastReis, n-sizeof(err));
				}
			}
			else if (from == 0)
			{
				memcpy(dptr, (const void*) 0x8000, n);
			}
			else if (from == 512)
			{
				memcpy(dptr, (const void*) 0x8200, n);
			}
			if (sendAsk) Com.CRCSend(n+HEADER_LEN);
		}
		break;

		case CMD_WRITE_EE:
		{
			uint16_t from = *(uint16_t*)(&Com.buf[DATA_POS]);
			uint8_t n = Com.Count-(HEADER_LEN+2+CRC_LEN);
			uint8_t* dptr = &Com.buf[DATA_POS+2];
			bool sendAsk = true;
			if (from == 2048)
			{
				sendAsk = false;
				//FLASH_write_eeprom_block((eeprom_adr_t) from-2048, dptr, n);
			}
			if (from == 4096)
			{
				sendAsk = false;
				//FLASH_write_eeprom_block((eeprom_adr_t) from-4096, dptr, n);
			}
			else if (from == 1024)
			{
				Ind.setMetr(dptr, n, &setMetrEvent);
				sendAsk = false;
			}
			else if (from == 0)
			{
//				FLASH_write_flash_n(0, dptr, n);
				sendAsk = false;
			}
			else if (from == 512)
			{
				FLASH_write_flash_n(512, dptr, n);
			}
			if (sendAsk) Com.CRCSend(HEADER_LEN);
		}
		break;
				
		case CMD_WORK:
			saveLen = Com.buf[DATA_POS];
			if (saveLen > (sizeof(uint32_t)+sizeof(uint8_t)))
			{
				if (!Ind.isOn) 
				{
					Ind.on();
					_delay_ms(1500);
				}
				TestModeTimer = 5;
				if (Ind.ready())
				{
					Ind.getData(&TestWorkEvent);
				}
				else
				{
					WorkData_t* d = (WorkData_t*)(&Com.buf[DATA_POS]);
					*(WorkDataStd_t*) d = *(WorkDataStd_t*) &workData;					
					memset(&d->dat, 0, sizeof(Induc_t));
					d->AppState |= 0x40;
					d->AppState |= 0x80;
					Com.CRCSend(saveLen+HEADER_LEN);
				}
			}
			else 
			{
				if (Ind.isOn && (workData.AppState != APP_WORK)) Ind.off();
				TestModeTimer = 0;
				if (Indicator.Error3 || Indicator.Error2) workData.AppState |= 0x40;				
				Com.CRCSend(&workData, saveLen);
				workData.AppState &= ~0x40;
			}
		break;
		case CMD_ERAM_CLEAR:
			if (*(uint32_t*)(&Com.buf[DATA_POS]) == 0x12345678)
			{
				Ram.NAND_Format();				
				Com.CRCSend(HEADER_LEN+4);
			}
		break;
		case CMD_INFO:
			Com.CRCSend(ReadMetaData(&Com.buf[DATA_POS], Com.buf[DATA_POS], (Com.Count == HEADER_LEN+1+2+2)? *(uint16_t*)(&Com.buf[DATA_POS+1]): 0)+HEADER_LEN);
		break;
		//case CMD_ERR:
		//uint8_t NeedClearErr = Com.buf[DATA_POS];
		//Com.buf[DATA_POS] = Error_Code;
		//if (Error_Code) Com.CRCSend(HEADER_LEN+1 + Error_Handler(&Com.buf[DATA_POS+1]));
		//else Com.CRCSend(HEADER_LEN+1);
		//if (NeedClearErr == 0xA5) Error_Code = 0;
		//break;
		
	}
}

static void SaveNewWcursor(void)
{
	eep_t e = {0xFF};
	Indicator.Error3 = false;
	err.reis = 0;
	memcpy(&e.reises[1],&eeprom.reises[0], sizeof(eep_reis_t)*2);
	
	e.reises[0] = *(eep_reis_t*) &Ram.LastReis;
	
	e.Currentwreis.reis = Ram.wreis.reis;
	e.Currentwreis.startBlock = Ram.waddr.block;
	
	//eeprom_adr_t p = offsetof(eep_t, Currentwreis);
	FLASH_write_eeprom_block(0, (uint8_t*) &e, sizeof(eep_t));
}

static bool CheckNAND()
{
	//Ram.chip = 1;
	
	if (Ram.NAND_Reset())
	{
		//uint16_t id = Ram.NAND_ReadID();
		//if (id == 0x2C34)
		//{
		//Com.buf[3] = 1;
		//}

		if (dataprog.blocks_per_lun == 0xFFFF)
		{
			if (Ram.NAND_IsNotFormatted()) 
			{
				Ram.NAND_Format();
			}
			param_page_t pp;
			if (Ram.NAND_Read_Param_Page(&pp))
			{
				flash_nand_t n;
				memset(&n,0xff,sizeof(n));
				memcpy(&n.manufacturer, &pp.manufacturer, sizeof(n.manufacturer));
				memcpy(&n.model, &pp.model, sizeof(n.model));
				n.data_bytes_per_page = pp.data_bytes_per_page;
				n.spare_bytes_per_page = pp.spare_bytes_per_page;
				n.pages_per_block = pp.pages_per_block;
				n.blocks_per_lun = pp.blocks_per_lun;
				n.max_bad_blocks_per_lun = pp.max_bad_blocks_per_lun;
				n.guarenteed_valid_blocks = pp.guarenteed_valid_blocks;
				
				FLASH_write_flash_n(0, (uint8_t*)&n, sizeof(n)+8);
				//return true	;
			}
		}
		if (NANDCheckFormat()) CheckWcursor();
		return true;
	}
	return false;	
}

#ifdef TEST_RAM_EMULATE_BAD
static void assignRamdata(RamData_t& d, uint32_t& i)
{	
	d.ramtime = i;
	d.accel.X = Ram.waddr.block;
	d.accel.Y = Ram.waddr.page;
	d.accel.Z = Ram.wreis.reis;
	d.accel.T = Ram.wreis.block;
	d.dat.signature = i;
	d.dat.condition = i;
	d.dat.frame = i;
	d.dat.temperature = i;
}
static void EmulateBadTest(void)
{
	Ram.initWCursor();
	SaveNewWcursor();
	Ram.Emulate_bad_program_page = Ram.waddr;
	Ram.Emulate_bad_program_page.block += 2;
	Ram.Emulate_bad_program_page.page = 62;
	Ram.Emulate_bad_erase_block = Ram.waddr.block + 3;
	Ram.Emulate_bad_move = Ram.waddr;
	Ram.Emulate_bad_move.block += 4;
	Ram.Emulate_bad_move.page = 60;
	//Ram.Emulate_bad_move[1] = Ram.waddr;
	//Ram.Emulate_bad_move[1].block += 5;
	//Ram.Emulate_bad_move[1].page = 61;
	RamData_t d;
	for (uint32_t i = 1; i <= 4009; i++)
	{
		assignRamdata(d,i);
		Ram.write((uint8_t*) &d, sizeof(d));
	}
	GoToSleep();
}
#endif

 //uint8_t pbuf1[PAGE_SIZE];
 //uint8_t pbuf0[PAGE_SIZE];
 //uint8_t buf32[113];
 //uint32_t idx=0;
  
int main(void)
{
	//tram(Ram);
	
	//PORTD.DIRSET = 1<< 1;
	
	Com.setRS485mode();
	Com.setBaud(DEF_SPEED);
	Com.intMode();
	Com.enableRxD();
	
	
    
	if ((workData.AppState > APP_MAX)||\
	(workData.AppState == APP_SET_TIME)||\
	(workData.AppState == APP_CLEAR_RAM))
	{
		workData.AppState = APP_IDLE;
		//memset(&Err, 0, sizeof(ErrIncl_t));
	}
 	else if (workData.AppState == APP_WORK)
 	{
 		Ind.on();
  		_delay_ms(1500);
 	}		
	_delay_ms(2);
	sei();
	err.ram = !CheckNAND();	
	err.accel = !Ais2ih.InitAccel();	
	
  #ifdef TEST_RAM_EMULATE_BAD
  EmulateBadTest();
  #endif
	
	//RowAddrs a;
	//a.page=0;
	//a.block=0;
	//reis_t r[16];
	//memset(r,0,sizeof(r));
	//Ram.init();
	//for(uint8_t i=0;i<16;i++)
	//{
		//a.block = i;
		//Ram.SkipBlock(&a, &r[i]);		
	//}
	//Ram.chip = 1;
	//Ram.initWcursor();
	
//	Ram.init();
//	Ram.unlockAllBlock();
	//Ram.deinit();
	
	//uint32_t*pb = (uint32_t*)&Ram.page;
	//for(uint16_t i=0; i<PAGE_DATA_SIZE/4; i++) pb[i] = i;
	//
	//RowAddrs a =Ram.waddr;
	//a.block=0x12;
	//a.page=5;
	//for(uint16_t i=0;i<20;i++)
	//{
		//Ram.NAND_ReadPage(&a,PAGE_DATA_SIZE+0x40-8,&pbuf0[0], 0x20);
		//a.block++;
		////*((uint16_t*) &pbuf0[PAGE_SIZE-2]) = a.block;
	//}
	//
	//pbuf0[110] = 1;
	//RowAddrs b;
	//b.block = 0xe7;
	//for( uint8_t i=0;i<200;i++)
	//{
		//
		//if (a.block & 1)  Indicator.On();
		//else Indicator.Off();
		//
		//Ram.BlockMove(a.block,b.block,64);
//
		//bool res = Ram.PageRead(&a,0,pbuf0,PAGE_SIZE);
		//res = Ram.PageRead(&b,0,pbuf1,PAGE_SIZE);
		//if (res)
		//{
			//for(uint16_t i =0; i<PAGE_DATA_SIZE; i++)
			//{
				//if(pbuf0[i] != pbuf1[i])
				//{
					//pbuf0[0] = Ram.lastStatus;
				//}
				//
			//}
		//}
		//
		//a.block++;
		//b.block++;
	//}
//	Ram.BlockMove(a.block,b.block,64);
	//Ram.BlockErase(a.block);
	//
	//bool res = Ram.PageWrite(&a);
	//if (res)
	//{
		//if ((Ram.lastStatus & SPI_NAND_PF) ==0)
		//{
			//reis_t z;
	//Ram.init();
	//for (uint8_t p =0; p<64;p++)
	//{
		//if (Ram.SkipBlock(&a,&z))
		//{
			//pbuf0[0] = Ram.lastStatus;			
		//}				
		//a.block++;
		//b.page = p;
		//bool res = Ram.PageRead(&a,0,pbuf0,PAGE_SIZE);
		//res = Ram.PageRead(&b,0,pbuf1,PAGE_SIZE);
		//if (res)
		//{
			//for(uint16_t i =0; i<PAGE_DATA_SIZE; i++)
			//{
				//if(pbuf0[i] != pbuf1[i])
				//{
					//pbuf0[0] = Ram.lastStatus;
				//}
				//
			//}
		//}
	//}
	//pbuf0[0] = Ram.lastStatus;
	
	//for(uint8_t j=0; j<113; j++) buf32[j] = j;
	//while (++idx < FLASH_SIZE_BYTES)
	//{
		//if (Ram.waddr.block & 1)  Indicator.On();
		//else Indicator.Off();
		//Ram.write((uint8_t*)buf32,sizeof(buf32));		
	//}
	
	
	//Ram.ResetPageBase();
	//for (uint32_t i = 0; i < 131072; i++) Ram.writePage();
    while (1) 
    {	/// GPR.GPR0: uarts data ready
		///           70ms ticks
		///           2s ticks		
		/// если нет данных GPR.GPR0 == 0 то спать
		if (GPR.GPR0 == 0)
		{
			/// GPR.GPR1 - uarts lock если какой-то порт занят, то спать в режиме IDLE
			if (GPR.GPR1) SLPCTRL.CTRLA = SLPCTRL_SMODE_IDLE_gc | 1;
			else SLPCTRL.CTRLA = SLPCTRL_SMODE_STDBY_gc | 1; 
			
		    //PORTD.OUTCLR = 1 << 1;
			// спать
			sleep_cpu();			
			//PORTD.OUTSET = 1 << 1;
		}	
		if ( Ind.isReadyRxD())
		{
			 Ind.handler70ms();				
		}
		if ( Clock.checkReadyTik()) 
		{
   			eep.Handler();			
			Indicator.Handler64(workData.AppState); 				
			Ind.handler70ms();
		};
		if (Clock.checkReady2Sec())
		{
			if (TestModeTimer > 0) 			
			{				
				if(--TestModeTimer == 0) 
				{
					if (workData.AppState != APP_WORK) 
					{
						Ind.off();
					}
				}
			}
			else Com.errRS485DirReset();
			
			if (TurboTimer > 0) {
				if(--TurboTimer == 0) 
				{
					if(curTurbo >= 3) 
					{
						Clock.RestoreExternalClock();
						Clock.HiSpeedReady = false;
					}
					Com.setBaud(DEF_SPEED);
					Indicator.User = true;
				    curTurbo = 0;
				}
				}
			workData.time++;				
			switch (workData.AppState)// & 0b00111111)
			{
				case APP_IDLE:	
						
				break;					
				case APP_SET_TIME:
					if (!Ram.cleared()) 
					{
						Ram.startClear();
					}
					workData.AppState = APP_CLEAR_RAM;
				break;
					
				case APP_CLEAR_RAM:					
					if (Ram.checkReadReady()) 
					{
						workData.AppState = APP_DELAY;
						SaveNewWcursor();
					}
				break;
				case APP_DELAY:					
					if (workData.time == 0)
					{
						workData.AppState = APP_WORK;						
						Ram.ResetPageBase();
						Ind.on();
						//UpdateDAC();
					} 					
				break;					
				case APP_WORK:					
					if (TestModeTimer == 0) {Ind.getData(&WorkEvent);} //TODO: remove test regim
					else WorkEvent(1);
					//WorkEvent(0);
				break;
			}
		} // ~if (SysTick.Is2SecTick())
		if (Com.checkReadyRxD())
		{
			#ifdef BT2
			 if ((Com.Count > 3) && ((Com.buf[0] == ADDRESS_PROCESSOR) || (Com.buf[0] == 0xFF)) && (crc16(Com.buf, Com.Count) == 0))
			#else
			 uint8_t tmp = Com.buf[0] & 0xF0;
			 if ((Com.Count > 2) && ((tmp == ADDRESS) || (tmp == 0xF0)) && (crc16(Com.buf, Com.Count) == 0))
			#endif
			{
				//Com.buf[Com.Count] = 0xAA;
				//Com.startTxD(Com.Count);
				if (TurboTimer > 0) TurboTimer = 4;
				RunCmd();
				//if (RunCmd()) Com.set_rx();	
			}
			//else 
			//{
				//Com.buf[Com.Count] = 0x1E;
				//Com.startTxD(Com.Count+1);
			//};
			//else Com.set_rx();//*/
		} // ~Com.Pool())
    }
}

