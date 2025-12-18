/*
 * NNK_avr128.cpp
 *
 * Created: 23.01.2024 14:58:56
 * Author : User
 
     FUSES:
     BOOTSiZE 0x10
     CODSIZE  0x60

	 .text=0x1000 flash segment
	 -T c:\AVR\avrxmega4.x - misc опция линкера ДОБАВИТЬ!!! -AVR128Dx
 */ 


#include <stddef.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <ccp.h>
#include <tools.h>
#include <getarg.h>
#include <Indicator.hpp>
#include <usarts.hpp>
#include <protokol.h>
#include <crc.h>
#include <clock.hpp>
#include <Data.h> // данные для метаданных
#include <MetaData.h> // для создания секции линкера "meta_data" после векторов прерываний

#include <flash_n.hpp>
#include <mx25l256.hpp>
#include <AIS328soft.hpp>
#include <pinout.hpp>

#include "GGTiny1616.hpp"
#include <eep_ix.hpp>
#include <twi_im.hpp>
#include <LTC2942_im.hpp>

#include <turbo.hpp>


using namespace ggkp128;

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
//	for (uint16_t i = 0x4000; i<0x8000; i++) *(uint8_t*) i = 0xAA; 
}


#define Com serial2
#define	DEF_SPEED 125
#define EEP_OFFSET 8
#define NO_POWER_LEN (sizeof(uint32_t)+sizeof(uint8_t)+sizeof(charge_t))
#define EEP_UPDATE_KADRS 900 //30min
#define DEFAULT_KADR -100000000
#define WDTO_N WDTO_250MS

EEPROM_T(eep);
INSTATCE_LTC2942C(LTC2942);
INST_CLOCK;

indicator_t<DIOD> Indicator;
static ram_t<MX25L256> Ram;
static pinout_t<PWR> power;
static pinout_t<BATON> batOn;
static GGTiny1616_t ggkp;
static ais328_t<AIS328SOFT> ais328;
static uint8_t TestModeTimer;
static WorkData_t workData={APP_IDLE,DEFAULT_KADR};
static volatile uint8_t ResetFunction NO_INIT;
static int32_t LastKadrEEPChargeUpdate;
static turbo_t<DEF_SPEED> Turbo;

// реальные они-же виртуальные секции ROM
#define newbat (*(eep_new_bat_t*)0x8000) // from 0
//#define daclevel (*(eep_nnk_dac_t*)0x8200) // from 1024

// реальная полная структура EEPROM
// 8-nop 8 16 32 4
//typedef struct __attribute__((packed))
//{
	//eep_batt_volum_t bat_vol; //4 adr64 
	//eep_save_state_t state; //8 -adr8
	//charge_t charge; //32 - adr 32
	//eep_errors_t err; //16 -adr16
//} eep_real_struct;

// виртуальные секции EEPROM
#define EEP_OFFSET_VOLUME 8 // from 8 len 4
#define EEP_OFFSET_ERR 16 // from 16 len 8
#define EEP_OFFSET_ERR_QZ 16+8 // from 16 len 4
#define EEP_OFFSET_KADR_CHARGE 32 // from 512

// реальные  EEPROM
#define eep_errors (*(eep_errors_t*) (EEPROM_START + EEP_OFFSET_ERR))
#define eep_kadr_charge (*(eep_save_state_and_charge_t*) (EEPROM_START + EEP_OFFSET_KADR_CHARGE))
#define eep_batt_volum (*(eep_batt_volum_t*) (EEPROM_START + EEP_OFFSET_VOLUME))


static bool Powered(void)
{
	return power.IsOn();
}
static void StandBy(bool fullStop)
{
  ggkp.off();  
  power.Off();
  if(fullStop) batOn.Off();
  else batOn.On();	
}
static void WakeUp()
{
	batOn.On();	
	power.On();
    ggkp.on();  
}

// СОБЫТИЕ: постановка на задержку пользователем
static void ResetErrorsInEEP(void)
{
	eep_errors_t e = {0,0,0,DEFAULT_KADR,DEFAULT_KADR};
	eep.Save(EEP_OFFSET_ERR, &e, sizeof(eep_errors_t));
}
// СОБЫТИЕ: ошибка кварца
static void SaveQzErrInEEP(void)
{
	eep.Save(EEP_OFFSET_ERR_QZ, &workData.time, 4);
}

// СОБЫТИЕ: аномальный ресет
static void SaveResetInEEP(int32_t* restored_kadr)
{
	reset_t r;
	r.ResetCount = eep_errors.reset.ResetCount+1;
	r.ResetFunction = ResetFunction;
	r.kadr_Reset = *restored_kadr;
	r.ResetRegister = RSTCTRL.RSTFR;
	eep.Save(EEP_OFFSET_ERR, &r, 4);
}

static void SaveChargeAndStateToEEP_Async(void)
{
	LastKadrEEPChargeUpdate = workData.time;	
	eep.Save_Async(EEP_OFFSET_KADR_CHARGE, &workData.AppState, sizeof(eep_save_state_and_charge_t));
}



static void UpdateFromEEP(bool initDac= false, void* DacData = 0)
{
    eep_nnk_dac_t* p = (eep_nnk_dac_t*) DacData;
    if (initDac) ggkp.initDAC(p->DAC1,p->DAC2);	

	ltc2942c::Set(&eep_kadr_charge.charge.AccCharge);
}
static uint8_t ggkp_error;
static void cbWorkData(uint8_t len)
{
    if (len > 0)
    {
       ggkp.get_data(&workData.ggkp, sizeof(ggkp_data_t)); 
       ggkp_error = 0;
    }
    else 
    {
        memset(&workData.ggkp,0,sizeof(ggkp_data_t));
       ggkp_error = 0x40;
    }        
}
static void UpdateWorkData(void)
{
	ais328.get_data(&workData.accel);    
	ggkp.dataReq(&cbWorkData);
}

static void EchoEvent(buf_len_t size)
{
    if (size > 0)
    {
        memcpy(Com.buf, ggkp.buf(), size); 
        Com.startTxD(size);
    }
}
static void EepEvent(buf_len_t size)
{
    if (size > 0)
    {
        memcpy(&Com.buf[DATA_POS], ggkp.buf(), size); 
        Com.CRCSend(size+HEADER_LEN);
    }
}
static void SendSlaveBoot(void)
{
    if (!Powered()) WakeUp();
    TestModeTimer = 255;
    ggkp.dataReqStd(WAIT_GGTINY1616,Com.Count,&Com.buf[0],&EchoEvent);
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
				workData.AppState = APP_IDLE;  
				StandBy(true);
			}
			else
			{
				Clock.resetTik();
				workData.time = *wait_time;
				if (workData.AppState > APP_DELAY)
				{
					workData.AppState = APP_SET_TIME;
					StandBy(false);
				}
				ResetErrorsInEEP();
			}
			//update eeprom charge
			SaveChargeAndStateToEEP_Async();		
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
        else
        {
            SendSlaveBoot();
        }
		break;
        
        case CMD_BOOT_READ:
        case CMD_BOOT_EXIT:
        case CMD_BOOT_WRITE:
            SendSlaveBoot();
        break;            
        
		case CMD_ERAM:
			Ram.readAndSendUart(&Com.buf[DATA_POS]);
		break;
		case CMD_ERAM_WRITE:
			Com.buf[DATA_POS] = Ram.write(&Com.buf[DATA_POS+1], Com.buf[DATA_POS]);
			Com.CRCSend(HEADER_LEN+1);
		break;
		case CMD_ERAM_SET_BASE:
			Ram.SetWritePage(&Com.buf[DATA_POS]);
			Com.CRCSend(HEADER_LEN);
		break;

		case CMD_READ_EE:
		{
			uint8_t n = Com.buf[DATA_POS+2];
			uint16_t from = *(uint16_t*)(&Com.buf[DATA_POS]);
			uint8_t* dptr = &Com.buf[DATA_POS];
			
			if (from == 0) // newbat flash 0x8000
			{
				memcpy(dptr, (const void*) 0x8000, n);
				Com.CRCSend(n+HEADER_LEN);
			}
			else if (from == 8)// vat_vol 
			{
				eep.Read(EEP_OFFSET_VOLUME, dptr, n);
				Com.CRCSend(n+HEADER_LEN);
			}
			else if (from == 16)// eep_errors_t
			{
				eep.Read(EEP_OFFSET_ERR, dptr, n);
				Com.CRCSend(n+HEADER_LEN);
			}
			else if (from == 512) // eep_save_state_t + charge_t
			{
				eep.Read(EEP_OFFSET_KADR_CHARGE, dptr, n);
				Com.CRCSend(n+HEADER_LEN);
			}
			else if (from == 1024) // daclevel (*(eep_nnk_dac_t*)0x8200) 
			{
				//memcpy(dptr, (const void*) 0x8200, n);
                uint8_t Data = 2;
                ggkp.dataReqStd(WAIT_GGTINY1616,1,&Data,&EepEvent);
				//Com.CRCSend(n+HEADER_LEN);
			}
		}
		break;

		case CMD_WRITE_EE:
		{
			uint16_t from = *(uint16_t*)(&Com.buf[DATA_POS]);
			uint8_t n = Com.Count-(HEADER_LEN+2+CRC_LEN);
			uint8_t* dptr = &Com.buf[DATA_POS+2];
			
			wdt_disable();
			
			if (from == 0) // newbat flash 0x8000
			{
				eep_new_bat_t* b = (eep_new_bat_t*) dptr;
				if (b->ResetFlag == 1)
				{
				//	ltc2942.ResetCharge();
					float data = 0;					
					eep.SaveBytes(EEP_OFFSET_KADR_CHARGE + sizeof(save_state_t)
					 + offsetof(charge_t,AccCharge) ,&data, sizeof(float));					
					b->ResetFlag = 0;
					FLASH_write_flash_n(0, dptr, n+2);	
                    UpdateFromEEP(false, dptr);
                    
				}
			}
			else if (from == 8) // vat_vol 
			{
				eep.SaveBytes(EEP_OFFSET_VOLUME, dptr, n);
			}
			else if (from == 512) // charge_t
			{
				eep.SaveBytes(EEP_OFFSET_KADR_CHARGE, dptr, n);
			}
			else if (from == 1024) // daclevel (*(eep_nnk_dac_t*)0x8200) 
			{
				//FLASH_write_flash_n(0x200, dptr, n);
                UpdateFromEEP(true, dptr);
			}
			Com.CRCSend(HEADER_LEN);
			
			
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
				//if (workData.AppState != APP_WORK)
				//{
				//UpdateWorkData();
				//}
				workData.AppState |= 0x80 | ggkp_error;
				Com.CRCSend(&workData, saveLen);
				workData.AppState &= ~(0x80 | ggkp_error);
			}
			else
			{
				if (Powered() && (workData.AppState != APP_WORK))
				{
					StandBy(false);
				}
				Com.CRCSend(&workData, saveLen);
			}			
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

int main(void)
{
// новый подход
//	StandBy(true); ИТАК ПОУМОЛЧАНИЮ

	ais328.norm_mode_2g_50hz();
	
	if (eep_kadr_charge.kadr.AppState != APP_IDLE) // жопа	
	{		
		int32_t lastKadr = eep_kadr_charge.kadr.kadr;
		
		if (eep_kadr_charge.kadr.AppState == APP_DELAY)
		{
			workData.AppState = APP_DELAY;
			StandBy(false);
		}
		else if(eep_kadr_charge.kadr.AppState == APP_WORK)
		{
			workData.AppState = APP_WORK;
			
			// восстановим последний кадр из памяти
			uint32_t* pk = (uint32_t*) &lastKadr;			
			Ram.Restore(sizeof(RamData_t), pk);
			
			StandBy(false);
		}
		else
		{
			 // жопа полная				
		}
		workData.time = lastKadr;
		
		SaveResetInEEP(&lastKadr);		
		ResetFunction = 77;
		
		wdt_enable(WDTO_N);		
	}
	else StandBy(true);	
	
	LastKadrEEPChargeUpdate = workData.time;
			
	Com.setRS485mode();
	Com.setBaud(DEF_SPEED);
	Com.intMode();
	Com.enableRxD();	
	
	ltc2942c::twi.CallbackRegister(ltc2942c::I2CErr);
	
	sei();
	
	UpdateFromEEP(false);

	// включаем питание
	if (workData.AppState == APP_WORK) WakeUp();
		
    while (1) 
    {

	    ///           70ms ticks
	    ///           2s ticks
    	/// GPR.GPR0: uarts data ready
	    /// если нет данных GPR.GPR0 == 0 то спать
	    if (GPR.GPR0 == 0)
	    {
		    /// GPR.GPR1 - uarts lock если какой-то порт занят, то спать в режиме IDLE
		    if (GPR.GPR1) SLPCTRL.CTRLA = SLPCTRL_SMODE_IDLE_gc | 1;
		    else SLPCTRL.CTRLA = SLPCTRL_SMODE_STDBY_gc | 1;		    
		    // спать
			ResetFunction = 1;
		    sleep_cpu();
			ResetFunction = 0;
	    }
		
		wdt_reset();
		
		ResetFunction = 55;
		ltc2942c::TWIHandler();
		ResetFunction = 0;
		
		if (!eep.Busy() && Clock.IsQzErr())
		{
			SaveQzErrInEEP();
		}
		if ( ggkp.isReadyRxD())
		{
			ggkp.handler70ms();				
		}        
		///           70ms ticks
		if ( Clock.checkReadyTik()) 
		{
			ggkp.handler70ms();				

			eep.Handler();			
			
			Indicator.Handler64(workData.AppState); 				
			
			ltc2942c::DelayHandler();
			
			if (Powered())
			{
				ResetFunction = 2;
				ais328.sample();
				ResetFunction = 0;
			}			
		};
		///           2s ticks
		if (Clock.checkReady2Sec())
		{
			ResetFunction = 10;
			
			if (TestModeTimer > 0) 			
			{				
				if(--TestModeTimer == 0) 
				{
					if (workData.AppState != APP_WORK) 
					// off power sensors
					{
						StandBy(workData.AppState == APP_IDLE);
					}
				}
			}
			else Com.errRS485DirReset();
			
            Turbo.Handler2sec();

			workData.time++;
			
			ResetFunction = 3;
			ltc2942c::Get(&workData.charge, &eep_batt_volum.volum);
			ResetFunction = 30;
			
			if (Powered()) 
			{
				UpdateWorkData();
			}
			ResetFunction = 31;
			//Indicator.On();
			if ((workData.time - LastKadrEEPChargeUpdate >= EEP_UPDATE_KADRS) && (workData.AppState != APP_IDLE)) SaveChargeAndStateToEEP_Async();			
			//Indicator.Off	();
			ResetFunction = 32;
									
			switch (workData.AppState & 0b00111111)
			{
				case APP_IDLE:

				break;					
				
				case APP_SET_TIME:
				
				    StandBy(false);	
					
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
						 SaveChargeAndStateToEEP_Async();					
						wdt_enable(WDTO_N);
					}
				break;
				case APP_DELAY:					
					if (workData.time >= 0)
					{						
						workData.AppState = APP_WORK;	
												
			ResetFunction = 33;
						SaveChargeAndStateToEEP_Async();					
			ResetFunction = 34;
												
						WakeUp();
			ResetFunction = 35;
						Ram.ResetPageBase();
//			ResetFunction = 36;
//						nnk.ClearCounts();
//			ResetFunction = 37;
					} 					
				break;					
				case APP_WORK:
				    ResetFunction = 4;												
				    Ram.write(reinterpret_cast<uint8_t*>(&workData.time), sizeof(RamData_t));				
					ResetFunction = 40;
				break;
			}
		} // ~if (SysTick.Is2SecTick())
		if (Com.checkReadyRxD())
		{
			uint8_t tmp = Com.buf[0] & 0xF0;
			if ((Com.Count > 2) && ((tmp == ADDRESS) || (tmp == 0xF0)) && (crc16(Com.buf, Com.Count) == 0))
			{
                Turbo.RestartTimer();
				
				ResetFunction = 5;												
				RunCmd();
				ResetFunction = 50;
			}
		} // ~Com.checkReadyRxD()
		
    }
}

