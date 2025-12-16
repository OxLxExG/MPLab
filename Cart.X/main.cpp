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
#ifdef DIOD
#include <Indicator.hpp>
#endif
#include <nand.hpp>
#include <flash_n.hpp>
#include <getarg.h>
#include <usarts.hpp>
#include <mt29f4g.hpp>
#include <ais2ih.hpp>
#include <flash_n.hpp>
#include "Cart.hpp"
#include "turbo.hpp"
#include <protokol.h>
#include <crc.h>
#include <clock.hpp>
#include <Data.h> // данные для метаданных
#include <MetaData.h> // для создания секции линкера "meta_data" после векторов прерываний
#include <pinout.hpp>

#include <eep_ix.hpp>

using namespace induc;

enum
{
    ERR_PERIPH_OK = 0,
    ERR_PERIPH_AIS2IH = 1,
    ERR_PERIPH_NAND = 2,
};

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
#define EEP_UPDATE_KADRS 900 // 900 // 900 // 90 //900 30min
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
        
#ifdef DIOD
indicator_t<DIOD> Indicator;
#endif
#ifdef TSTPIN
pinout_t<TSTPIN> tstpin;
#endif
ram_t<MT29F4G> Ram;
static pinout_t<PWR> power;
static ais2ih_t<AIS2IH> Ais2ih;
static Cart_c Cart;
//static uint8_t TurboTimer, 
static uint8_t TestModeTimer;
static uint16_t saveLen;
//static uint8_t curTurbo;
static WorkDataStd_t workData={APP_IDLE,DEFAULT_KADR};
static volatile uint8_t ResetFunction NO_INIT;
static int32_t LastKadrEEPChargeUpdate;
static turbo_t<DEF_SPEED> Turbo;
static uint8_t FlagSave30;

//static errors_t err;

// виртуальные секции EEPROM
#define EEP_OFFSET_ERR          16 // from 16
#define EEP_OFFSET_ERR_RESET    16 // from 16 len 8
#define EEP_OFFSET_ERR_QZ       16+8 // from 16 len 4
#define EEP_OFFSET_ERR_DEV      16+8+4 // from 16+8 len 2
#define EEP_OFFSET_ERR_NAND     16+8+4+2 // from 16+8 len 2
#define EEP_OFFSET_STATE30      32 // from 32
#define EEP_OFFSET_REIS_HIST    64 // from 64

// реальные  EEPROM (для чтения)
#define eep_errors (*(eep_errors_t*) (EEPROM_START + EEP_OFFSET_ERR))
#define eep_err_reset (*(reset_t*) (EEPROM_START + EEP_OFFSET_ERR))
#define eep_err_qz (*(int32_t*) (EEPROM_START + EEP_OFFSET_ERR_QZ))
#define eep_err_perif (*(perif_errors_t*) (EEPROM_START + EEP_OFFSET_ERR_DEV))
#define eep_save30 (*(eep_save_state30_t*) (EEPROM_START + EEP_OFFSET_STATE30))
#define eep_reis_hist (*(eep_reis_history_t*) (EEPROM_START + EEP_OFFSET_REIS_HIST))



static bool Powered(void)
{
	return power.IsOn();
}

static void StandBy(void)
{
    power.Off();
    Cart.off();
}

static void WakeUp()
{
    power.On();
    Cart.on();
}

// СОБЫТИЕ: постановка на задержку пользователем
static void ResetErrorsInEEP(void)
{
	eep_errors_t e = {0,0,0,DEFAULT_KADR,DEFAULT_KADR,0,0};
    eep.Save(EEP_OFFSET_ERR, &e, sizeof(eep_errors_t));
}
// СОБЫТИЕ: ошибка кварца
static void SaveQzErrInEEP(void)
{
    eep.Save(EEP_OFFSET_ERR_QZ, &workData.time, 4);
}
// СОБЫТИЕ: ошибка периферии
static void SaveDevErrInEEP(uint16_t Err)
{
    if (!(Err & eep_err_perif.ErrPerifMsk))
    {  
        Err |= eep_err_perif.ErrPerifMsk;
        eep.Save(EEP_OFFSET_ERR_DEV, &Err, 2);
    }
}
// СОБЫТИЕ: ошибка NAND
static void SaveNANDErrInEEP(uint16_t Err)
{ 
    if (!(Err & eep_err_perif.ErrNandMsk))
    {  
        Err |= eep_err_perif.ErrNandMsk;
        eep.Save(EEP_OFFSET_ERR_NAND, &Err, 2);
    }
}
// СОБЫТИЕ: аномальный ресет
static void SaveResetInEEP(int32_t* restored_kadr)
{
	reset_t r;
	r.ResetCount = eep_errors.reset.ResetCount+1;
	r.ResetFunction = ResetFunction;
	r.kadr_Reset = *restored_kadr;
	r.ResetRegister = RSTCTRL.RSTFR;
    eep.Save(EEP_OFFSET_ERR , &r, sizeof(r));
}

static void SaveReisHistoryToEEP(ereis_t* reisNow)
{
    ereis_t r[4];
    memcpy(&r[1], &eep_reis_hist.reises[0], sizeof(ereis_t)*3);
    r[0] = *reisNow;
    eep.Save(EEP_OFFSET_REIS_HIST , &r, sizeof(r));
}
static void SaveState30ToEEP_Async(void)
{
    eep_save_state30_t s;
    s.State = *(work_state_t*) &workData;
  //  if (s.State.AppState == APP_WORK || s.State.AppState == APP_SET_TIME)
  //  {
        s.reis.reis = Ram.wreis.reis;
        s.reis.blocks = Ram.wreis.block;
        s.reis.startBlock = Ram.CurrReisStartBlock;
        s.reis.endBlock = Ram.waddr.block;
   // }
  //  else s.reis = *((ereis_t*) &Ram.LastReis);
    
	LastKadrEEPChargeUpdate = workData.time;	
	eep.Save_Async(EEP_OFFSET_STATE30, &s, sizeof(eep_save_state30_t));
}


static void cbNANDErr(nand_errors_t e)
{
   SaveNANDErrInEEP(e); 
}

static void UpdateTstRAM(tst_ram* tstram)
{
                tstram->CurrReisStartBlock = Ram.CurrReisStartBlock;
                tstram->LastReis = *(ereis_t*) &Ram.LastReis;
                tstram->pageCnt = Ram.pageCnt;
                tstram->waddr.block = Ram.waddr.block;
                tstram->waddr.page = Ram.waddr.page;
                tstram->wreis.block = Ram.wreis.block;
                tstram->wreis.reis = Ram.wreis.reis;
                tstram->WCursorStatus = Ram.WCursorStatus;
}

static void TestWorkEvent(buf_len_t len)
{
	WorkData_t* d = (WorkData_t*)(&Com.buf[DATA_POS]);
	*(WorkDataStd_t*) d = workData;
	if (len > 0)
	{
        Cart.get_data(&d->dat, sizeof(Cart_t));
	}
	else
	{
		memset(&d->dat, 0, sizeof(Cart_t));
		d->AppState |= 0x40;
	}
	d->accel.T = Ais2ih.ReadT();
	Ais2ih.ReadXYZ(&d->accel.X);
    UpdateTstRAM(&d->tstram);
	d->AppState |= 0x80;

#ifdef DIOD
	if (Indicator.Error3 || Indicator.Error2) d->AppState |= 0x40;
#endif
	
    Com.CRCSend(saveLen+HEADER_LEN);
}

static void WorkEvent(buf_len_t len)
{
	RamData_t d;
	if (len > 0) 
	{
        Cart.get_data(&d.dat, sizeof(Cart_t));
	}
	else 
	{
		memset(&d.dat, 0, sizeof(Cart_t));
		//err.induct++;
	}
    d.dat.data.depth = FlagSave30;
	d.ramtime = workData.time;
	d.accel.T = Ais2ih.ReadT();
	Ais2ih.ReadXYZ(&d.accel.X);
    UpdateTstRAM(&d.tstram);
	if (!Ram.write((uint8_t*) &d, sizeof(RamData_t))) SaveNANDErrInEEP(1);// err.ram++;	
}
static void EchoEvent(buf_len_t size)
{
    if (size > 0)
    {
        memcpy(Com.buf,Cart.buf(), size); 
        Com.startTxD(size);
    }
}
static void RunCmd(void)
{
    wdt_disable();            
    
	switch (Com.buf[CMD_POS])
	{
		case CMD_TIME_SYNC:
		if (Com.Count == HEADER_LEN + 4 + CRC_LEN)
		{
			int32_t* wait_time = (int32_t*) &Com.buf[DATA_POS];
			
			//setup sleep timer, stop all peripheral work, goto idle mode
			if (*wait_time == 0)
			{
				StandBy();
                if (workData.AppState != APP_IDLE)
                {
                    workData.AppState = APP_IDLE;  
                    SaveState30ToEEP_Async();		
                }
                Ram.NAND_CheckFormat();//  NAND_Check(); // wcursor = lastReis
                //Ram.InitNewReis();
			}
			else
			{
				Clock.resetTik();
				workData.time = *wait_time;
				if (workData.AppState > APP_DELAY)
				{
					workData.AppState = APP_SET_TIME;
					StandBy();
                    SaveReisHistoryToEEP((ereis_t*) &Ram.LastReis);
                    Ram.InitNewReis();
				}
				ResetErrorsInEEP();
                SaveState30ToEEP_Async();		
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
				WakeUp();
				Ram.ResetPageBase();
			}
		}
		break;
		case CMD_TURBO:
            Turbo.Set(Com.buf[DATA_POS]);
		break;
		case CMD_BOOT:
			if (*(uint32_t*)(&Com.buf[DATA_POS]) == 0x12345678)
			{
				//stop all int , timers
				 cli();
				 StandBy();
                 eep.Save(0, (uint8_t*)"\0",1);
				 ccp_write_io((void *)&RSTCTRL.SWRR, 1);				 
			}
//            else 
//            {
//              if (Ind.ready()) Ind.dataReqStd(WAIT_IND,Com.Count,&Com.buf[0], &EchoEvent); 
//            }
		break;
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:     
        case 0x11:
        case 0x16:     
        case 0x17:     
        case 0x18:     
        case 0x19:     
            if (!Powered()) 
			{
                WakeUp();
                _delay_ms(1500);
			}
			TestModeTimer = 255;
            if (Cart.ready()) Cart.dataReqStd(WAIT_CART*2,Com.Count,&Com.buf[0], &EchoEvent); 
        break;
		case CMD_ERAM:
		    if(workData.AppState == APP_IDLE) Ram.readAndSendUart(&Com.buf[DATA_POS]);
			//err.cursor = Indicator.Error2;
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
            buf_len_t n = (Com.Count == HEADER_LEN+CRC_LEN+2+1)? Com.buf[DATA_POS+2] : *(uint16_t*)(&Com.buf[DATA_POS+2]);
			//uint8_t n = Com.buf[DATA_POS+2];
			uint16_t from = *(uint16_t*)(&Com.buf[DATA_POS]);
			uint8_t* dptr = &Com.buf[DATA_POS];
			
			if (from == 0) 
			{
                NAND_ReadModel(dptr);
				Com.CRCSend(n+HEADER_LEN);
			}
			else if (from == 128)
			{
                NAND_ReadBadBlocks(dptr, n/2);
				Com.CRCSend(n+HEADER_LEN);
			}
			else if (from == 16)// eep_errors_t
			{
				eep.Read(EEP_OFFSET_ERR, dptr, n);
				Com.CRCSend(n+HEADER_LEN);
			}
			else if (from == 32) 
			{
				eep.Read(EEP_OFFSET_STATE30, dptr, n);
				Com.CRCSend(n+HEADER_LEN);
			}
			else if (from == 64) 
			{
				eep.Read(EEP_OFFSET_REIS_HIST, dptr, n);
				Com.CRCSend(n+HEADER_LEN);
			}
//            else if (from ==1024)
//            {
//   				if (Ind.ready()) Ind.getMetr(&getMetrEvent);
//            }
            if (workData.AppState == APP_DELAY || workData.AppState == APP_WORK) wdt_enable(WDTO_N);		
		}
		break;

		case CMD_WRITE_EE:
		{
			uint16_t from = *(uint16_t*)(&Com.buf[DATA_POS]);
//			buf_len_t n = Com.Count-(HEADER_LEN+2+CRC_LEN);
			uint8_t* dptr = &Com.buf[DATA_POS+2];
			if (from == 32)
			{
                uint16_t reis = *(uint16_t*) dptr;
                
               if (reis == 0)
               {
                  Ram.NAND_CheckFormat(); 
               }
               else 
               {
                 Ram.SetLastReis(reis);
               //  Ram.Restore((lastReisInfo_t*) &eep_save30.reis, sizeof(RamData_t), &workData.time);
               }                   
               eep.Save(EEP_OFFSET_STATE30, &Ram.LastReis, sizeof(lastReisInfo_t));
			}
    		Com.CRCSend(HEADER_LEN);
		}
		break;
				
		case CMD_WORK:
             
			saveLen =  (Com.Count == HEADER_LEN+2+2)? *(uint16_t*)(&Com.buf[DATA_POS]): Com.buf[DATA_POS];
			if (saveLen > (sizeof(uint32_t)+sizeof(uint8_t)))
			{
				if (!Powered()) 
				{
                    WakeUp();
					_delay_ms(1500);
				}
				TestModeTimer = 5;
				if (Cart.ready())
				{
					Cart.DataReq(&TestWorkEvent);
				}
				else
				{
					WorkData_t* d = (WorkData_t*)(&Com.buf[DATA_POS]);
					*(WorkDataStd_t*) d = *(WorkDataStd_t*) &workData;					
					memset(&d->dat, 0, sizeof(Cart_t));
					d->AppState |= 0x40;
					d->AppState |= 0x80;
					Com.CRCSend(saveLen+HEADER_LEN);
				}
			}
			else 
			{
				if (Powered() && (workData.AppState != APP_WORK)) StandBy();
				TestModeTimer = 0;
        #ifdef DIOD
				if (Indicator.Error3 || Indicator.Error2) workData.AppState |= 0x40;				
        #endif
				Com.CRCSend(&workData, saveLen);
				workData.AppState &= ~0x40;
			}
		break;
		case CMD_ERAM_CLEAR:
			if (*(uint32_t*)(&Com.buf[DATA_POS]) == 0x12345678)
			{
				Ram.NAND_Format();
                _delay_ms(150);				
				Com.CRCSend(HEADER_LEN+4);
                Ram.NAND_CheckFormat();   
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
   	// включаем сторожевой таймер
    if (workData.AppState == APP_WORK || workData.AppState == APP_DELAY) wdt_enable(WDTO_N); 
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
  
int main(void)
{
    
    do {
        tstpin.IsOn() ?  tstpin.Off() : tstpin.On();
        _delay_ms(150);	
     } while (!Ram.NAND_Reset()); 
        
    if (Ram.NAND_IsNotFormatted()) 
    {
        Ram.NAND_Format();
        _delay_ms(150);	
    }
    
    Ram.NAND_CheckFormat();    
    
    Ram.CallbackRegister(&cbNANDErr);
//	StandBy(); должно быть поумолчанию
    
	if (eep_save30.State.AppState != APP_IDLE) // жопа	
	{		
		workData.time = eep_save30.State.time;
		
		if (eep_save30.State.AppState == APP_DELAY)
		{
			workData.AppState = APP_DELAY;
            Ram.InitNewReis();            
		}
		else if(eep_save30.State.AppState == APP_WORK)
		{
			workData.AppState = APP_WORK;
			// восстановим последний кадр и состояние курсора записи из NAND
			if (
//                    (eep_err_perif.ErrNandMsk & NAND_BAD_CRITICAL) 
//              ||    
                    (!Ram.Restore((lastReisInfo_t*) &eep_save30.reis, sizeof(RamData_t), &workData.time))
               ) 
                // если неудачно то начнем новый рейс с сохраненного кадра 
                Ram.InitNewReis();
		}
		else
		{
			 // жопа полная				
		}
		
        Ram.CheckReis((lastReisInfo_t*) &eep_save30.reis);
		SaveResetInEEP(&workData.time);		
		ResetFunction = 77;
		
	}
	
	LastKadrEEPChargeUpdate = workData.time - EEP_UPDATE_KADRS;
	
	Com.setRS485mode();
	Com.setBaud(DEF_SPEED);
	Com.intMode();
	Com.enableRxD();
	    	
  #ifdef TEST_RAM_EMULATE_BAD
  EmulateBadTest();
  #endif

    if (!Ais2ih.InitAccel()) SaveDevErrInEEP(ERR_PERIPH_AIS2IH); 
	// включаем сторожевой таймер
    if (workData.AppState == APP_WORK || workData.AppState == APP_DELAY) wdt_enable(WDTO_N); 
	// включаем питание
	if (workData.AppState == APP_WORK) WakeUp();
  
    sei();
    while (1) 
    {
		if (GPR.GPR0 == 0)
		{
			/// GPR.GPR1 - uarts lock если какой-то порт занят, то спать в режиме IDLE
			if (GPR.GPR1) SLPCTRL.CTRLA = SLPCTRL_SMODE_IDLE_gc | 1;
			else SLPCTRL.CTRLA = SLPCTRL_SMODE_STDBY_gc | 1; 
			
			sleep_cpu();			
		}	
		wdt_reset();
				
		if (Clock.IsQzErr())
		{
			SaveQzErrInEEP();
		}
        
		if ( Cart.isReadyRxD())
		{
			 Cart.handler70ms();				
		}
		if ( Clock.checkReadyTik()) 
		{
   			eep.Handler();			
        #ifdef DIOD
			Indicator.Handler64(workData.AppState); 				
        #endif
			Cart.handler70ms();
		};
		if (Clock.checkReady2Sec())
		{
			if (TestModeTimer > 0) 			
			{				
				if(--TestModeTimer == 0) 
				{
					if (workData.AppState != APP_WORK) 
					{
						StandBy();
					}
				}
			}
			else Com.errRS485DirReset();
			
            Turbo.Handler2sec();
			workData.time++;				

			ResetFunction = 5;
			if ((workData.time - LastKadrEEPChargeUpdate >= EEP_UPDATE_KADRS) && (workData.AppState != APP_IDLE)) 
            {
                FlagSave30 = 1;                
                SaveState30ToEEP_Async();			
            }
            else FlagSave30 = 0;
			ResetFunction = 6;

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
						SaveState30ToEEP_Async();					
						wdt_enable(WDTO_N);
					}
				break;
				case APP_DELAY:					
					if (workData.time >= 0)
					{						
						workData.AppState = APP_WORK;	
												
                        ResetFunction = 7;
						Ram.ResetPageBase();
                        ResetFunction = 8;
						SaveState30ToEEP_Async();					
                        ResetFunction = 9;												
						WakeUp();
                        ResetFunction = 10;												
					} 					
				break;					
				case APP_WORK:	
                    if (!Powered()) 
                    {
                        WakeUp();
                    }
					if (TestModeTimer == 0 && Cart.ready()) {Cart.DataReq(&WorkEvent);} //TODO: remove test regim
					else WorkEvent(1);
					//WorkEvent(0);
				break;
			}
		} // ~if (SysTick.Is2SecTick())
		if (Com.checkReadyRxD())
		{
			 uint8_t tmp = Com.buf[0] & 0xF0;

			 if ((Com.Count > 2) && ((tmp == ADDRESS) || (tmp == 0xF0) || (tmp == 0x10)) && (CRC(Com.buf, Com.Count) == 0))                  
			{
                Turbo.RestartTimer();
				RunCmd();
			}
		} // ~Com.Pool())
    }
}
