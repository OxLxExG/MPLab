/*
     FUSES:
     BOOTSiZE 0x10
     CODSIZE  0x60

	 .text=0x1000 flash segment
	 -T ..\avrxmega4.x - misc опция линкера ДОБАВИТЬ!!! -AVR128Dx
 */ 


#include <string.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <ccp.h>

// мои библиотеки для всех проектов
#include <tools.h>
#include <getarg.h>
#include <protokol.h>
#include <crc.h>
#include <clock.hpp>
#include <eep_ix.hpp>
#include <flash_n.hpp>
#include <usarts.hpp>
#include <pinout.hpp>
#include <mt29f4g.hpp>
#include <ais2ih.hpp>

// мои библиотеки для данного проекта
#include <Data.h> // данные для метаданных
#include <MetaData.h> // сгенерировано для создания секции линкера "meta_data" после векторов прерываний 
#include <nand.hpp>
#include <bk.hpp>

#include <turbo.hpp>


using namespace bk128;

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
	for (uint16_t i = 0x4000; i<0x8000; i++) *(uint8_t*) i = 0xAA; 
}


#define Com serial1
#define	DEF_SPEED 125
#define EEP_OFFSET 8
#define NO_POWER_LEN (sizeof(uint32_t)+sizeof(uint8_t) + sizeof(ais21h_t))
#define EEP_UPDATE_KADRS 900 //30min
#define DEFAULT_KADR -100000000
#define WDTO_N WDTO_250MS

EEPROM_T(eep);
INST_CLOCK;

enum
{
    ERR_PERIPH_OK = 0,
    ERR_PERIPH_AIS2IH = 1,
    ERR_PERIPH_BK = 2,
};

ram_t<MT29F4G> Ram;
static pinout_t<POWER> power;
static ais2ih_t<AIS2IH> Ais2ih;
static bk_t bk;
static uint8_t TestModeTimer;
static WorkData_t workData={APP_IDLE,DEFAULT_KADR};
static volatile uint8_t ResetFunction NO_INIT;
static int32_t LastKadrEEPChargeUpdate;
static turbo_t<DEF_SPEED> Turbo;


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

// реальные  FLASH (для чтения)
// #define FLASH_BAD_SECTOR MAPPED_PROGMEM_START
// #define FLASH_NAND_DESC (MAPPED_PROGMEM_START+MAPPED_PROGMEM_PAGE_SIZE)

// #define flash_bad_sectors (*(flash_nand_bad_bloks_t*) FLASH_BAD_SECTOR) // from 0
// #define flash_nand_desc (*(flash_nand_t*) FLASH_NAND_DESC)


static bool Powered(void)
{
	return power.IsOn();
}
static void StandBy(void)
{
    bk.off();
    power.Off();
}
static void WakeUp()
{
	power.On();
    bk.on();
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

static void bk_callback(void* buf, uint8_t size)
{
	if (size > 0) 
	{        
		memcpy(&workData.bks, buf, sizeof(bks_t));
	}
	else 
	{
        SaveDevErrInEEP(ERR_PERIPH_BK); 	
		memset(&workData.bks, 0, sizeof(bks_t));
	}
    if (workData.AppState == APP_WORK)
    {
       Ram.write(&workData.time, sizeof(RamData_t)); 
    }
}
static void UpdateThenPowered(void)
{
    bk.Cmd(&bk_callback);
}
static void RunCmd(void)
{
	switch (Com.buf[CMD_POS])
	{
		case CMD_TIME_SYNC:
		if (Com.Count == HEADER_LEN + 4 + CRC_LEN)
		{
			int32_t* wait_time = (int32_t*) &Com.buf[DATA_POS];
			wdt_disable();
			//setup sleep timer, stop all peripheral work, goto idle mode
			if (*wait_time == 0)
			{
				StandBy();
				workData.AppState = APP_IDLE;  
                SaveState30ToEEP_Async();		
                NAND_Check(); // wcursor = lastReis
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
			workData.time =  *(int32_t*)(&Com.buf[DATA_POS]);
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
			cli();	
            eep.Save(0, (uint8_t*)"\0",1);
			ccp_write_io((void *)&RSTCTRL.SWRR, 1);
		}
		break;
		case CMD_ERAM:
			Ram.readAndSendUart(&Com.buf[DATA_POS]);
		break;

		case CMD_READ_EE:
		{
            wdt_disable();
            
			uint8_t n = Com.buf[DATA_POS+2];
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
            if (workData.AppState == APP_DELAY || workData.AppState == APP_WORK) wdt_enable(WDTO_N);		
		}
		break;
		
		case CMD_WORK:
		{
			uint8_t saveLen = Com.buf[DATA_POS];
			TestModeTimer = 5;
			if (saveLen > NO_POWER_LEN)
			{
				if (!Powered())
				{
					WakeUp();
				}
                
                workData.tstram.CurrReisStartBlock = Ram.CurrReisStartBlock;
                workData.tstram.LastReis = *(ereis_t*) &Ram.LastReis;
                workData.tstram.pageCnt = Ram.pageCnt;
                workData.tstram.waddr.block = Ram.waddr.block;
                workData.tstram.waddr.page = Ram.waddr.page;
                workData.tstram.wreis.block = Ram.wreis.block;
                workData.tstram.wreis.reis = Ram.wreis.reis;
                workData.tstram.WCursorStatus = Ram.WCursorStatus;
                
				workData.AppState |= 0x80;
				Com.CRCSend(&workData, saveLen);
				workData.AppState &= ~0x80;
			}
			else
			{
				if (Powered() && (workData.AppState != APP_WORK))
				{
					StandBy();
				}
				Com.CRCSend(&workData, saveLen);
			}			
		}
		break;
		case CMD_INFO:
		Com.CRCSend(ReadMetaData(&Com.buf[DATA_POS], Com.buf[DATA_POS], (Com.Count == HEADER_LEN+1+2+2)? *(uint16_t*)(&Com.buf[DATA_POS+1]): 0)+HEADER_LEN);
		break;
   		case CMD_ERAM_CLEAR:
			if (*(uint32_t*)(&Com.buf[DATA_POS]) == 0x12345678)
			{
                wdt_disable();
				Ram.NAND_Format();				
				Com.CRCSend(HEADER_LEN+4);
                NAND_Check();                
			}
		break;
	}
}

int main(void)
{
    Ram.CallbackRegister(&cbNANDErr);
    NAND_Check();
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
			if (!Ram.Restore((lastReisInfo_t*) &eep_save30.reis, sizeof(RamData_t), &workData.time)) 
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
	
	LastKadrEEPChargeUpdate = workData.time;
			
	Com.setRS485mode();
    Com.setFullDuplexmode();
	Com.setBaud(DEF_SPEED);
	Com.intMode();
	Com.enableRxD();	
    
    if (!Ais2ih.InitAccel()) SaveDevErrInEEP(ERR_PERIPH_AIS2IH); 	

	// включаем сторожевой таймер
    if (workData.AppState == APP_WORK || workData.AppState == APP_DELAY) wdt_enable(WDTO_N); 
	// включаем питание
	if (workData.AppState == APP_WORK) WakeUp();
		
	sei();
    
    while (1) 
    {

	    ///           32.768 ms ticks
	    ///           2.097152s ticks
    	/// GPR.GPR0: uarts data ready
	    /// если нет данных GPR.GPR0 == 0 то спать
	    if (GPR.GPR0 == 0)
	    {
		    /// GPR.GPR1 - uarts lock если какой-то порт занят, то спать в режиме IDLE
		    if (GPR.GPR1) SLPCTRL.CTRLA = SLPCTRL_SMODE_IDLE_gc | 1;
		    else SLPCTRL.CTRLA = SLPCTRL_SMODE_STDBY_gc | 1;		    
		    // спать
			ResetFunction = 255;
		    sleep_cpu();
			ResetFunction = 0;
	    }
		
		wdt_reset();
				
		if (Clock.IsQzErr())
		{
			SaveQzErrInEEP();
		}
		///           32 ms ticks
		if ( Clock.checkReadyTik()) 
		{
			eep.Handler();	
            bk.handler32ms();
		};
		///           2s ticks
		if (Clock.checkReady2Sec())
		{           
			ResetFunction = 1;
            
            workData.accel.T = Ais2ih.ReadT();
            Ais2ih.ReadXYZ(&workData.accel.X);
           
			ResetFunction = 2;
			
			if (TestModeTimer > 0) 			
			{				
				if(--TestModeTimer == 0) 
				{
					if (workData.AppState != APP_WORK) 
					// off power sensors
					{
						StandBy();
					}
				}
			}
			else Com.errRS485DirReset();

			ResetFunction = 3;

			Turbo.Handler2sec();

			workData.time++;
						
			ResetFunction = 4;
            
			if (Powered()) 
			{
				UpdateThenPowered();
			}
			ResetFunction = 5;
			if ((workData.time - LastKadrEEPChargeUpdate >= EEP_UPDATE_KADRS) && (workData.AppState != APP_IDLE)) SaveState30ToEEP_Async();			
			ResetFunction = 6;
									
			switch (workData.AppState & 0b00111111)
			{
				case APP_IDLE:

				break;					
				
				case APP_SET_TIME:
				
				    StandBy();	
					
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
			}
		} // ~if (SysTick.Is2SecTick())
        
        
        
		if (Com.checkReadyRxD())
		{
			uint8_t tmp = Com.buf[0] & 0xF0;
			if ((Com.Count > 2) && ((tmp == ADDRESS) || (tmp == 0xF0)) && (crc16(Com.buf, Com.Count) == 0))
			{
                Turbo.RestartTimer();
				ResetFunction = 11;												
				RunCmd();
				ResetFunction = 12;
			}
		} // ~Com.checkReadyRxD()
		
    }
}

