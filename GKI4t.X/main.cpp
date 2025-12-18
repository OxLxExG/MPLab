/*
 *
 * Created: 6.6.2025 
 * Author : User
 
   FUSES:
   BOOTSiZE 0x10
   CODSIZE  0x60	 
 * .text=0x1000 - ld
// -T c:\AVR\avrxmega4.x - misc опция линкера ДОБАВИТЬ!!! -AVR128Dx
 */ 

#define FULL_DUPLEX

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <ccp.h>
#include <getarg.h>
#include <usarts.hpp>
#include <mx25l256.hpp>
#include <Indicator.hpp>
#include "ADS131.hpp"
#include "ADXL356.hpp"
#include "HMC.hpp"
#include <protokol.h>
#include <crc.h>
#include <flash_n.hpp>
#include <clock.hpp>
#include <gk.hpp>

#include <Data.h> // данные для метаданных update build events !!
#include <MetaData.h> // для создания секции линкера "meta_data" после векторов прерываний
#include <pinout.hpp>

#include <eep_ix.hpp>
#include <twi_im.hpp>
#include <LTC2942_im.hpp>

#include <turbo.hpp>

using namespace adxl356gk_v2;

typedef  struct
{
	uint8_t InclBad;
	uint16_t InclCnt;
	uint8_t MCUSR_Save;
	uint16_t Resetcnt;
} ErrIncl_t __attribute__((aligned));

#define	DEF_SPEED 125
#define EEP_OFFSET 8
#define EEP_UPDATE_KADRS 900 // 900 //30min
#define DEFAULT_KADR -100000000
#define WDTO_N WDTO_250MS


//#define eeprom (*(eep_charge_t*)(0x1400 + EEP_OFFSET)) // from

#define dataprog (*(eep_t*)0x8000)
#define dataprog512 (*(TrrTemp_t*)0x8200)

// реальные они-же виртуальные секции ROM
#define newbat (*(eep_new_bat_t*)0x8400) // from 0
#define daclevel (*(TrrGk_t*)0x8600) // from 1024

// виртуальные секции EEPROM
#define EEP_OFFSET_VOLUME 8 // from 8 len 4
#define EEP_OFFSET_ERR 16 // from 16 len 8
#define EEP_OFFSET_ERR_QZ 16+8 // from 16 len 4
#define EEP_OFFSET_KADR_CHARGE 32 // from 512

// реальные  EEPROM
#define eep_errors (*(eep_errors_t*) (EEPROM_START + EEP_OFFSET_ERR))
#define eep_kadr_charge (*(eep_save_state_and_charge_t*) (EEPROM_START + EEP_OFFSET_KADR_CHARGE))
#define eep_batt_volum (*(eep_batt_volum_t*) (EEPROM_START + EEP_OFFSET_VOLUME))


//static void UpdateDAC(void)
//{
	//Gk.updateDACREF(dataprog.GR1.DAC);
//}

// F_CPU=8000000UL;ADDRESS_PROCESSOR=3;CLOCK=CLKHT,TA0,CLKOUT;DIOD=PF,2,INV;GK=CMP0,AINP1,TA1;USEUSART2=DEF0,255,TB0;USEUSART3=DEF0,200,SPI_MODE;MX25L256=SP1,ALT1,PF,4,UART2;ADS131=UART3,PB,4,PB,3,PB,5;ADXL356=3,-6,4,0;HMC=PA,4,1,5,-2;PWR=PD,4;BATON=PD,7;DEBUG;LTC2942=I2C0,ALT2;
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

#define Com serial2 // TODO: create global define

EEPROM_T(eep);
INSTATCE_LTC2942C(LTC2942);
ADS131_INIT;
INST_CLOCK;


indicator_t<DIOD> Indicator;
static pinout_t<PWR> power;
static pinout_t<PWRGK> power_gk;
static pinout_t<BATON> batOn;
static adxl365_t<ADXL356> adx356;
hmc_t<HMC> hmc;
static ram_t<MX25L256> Ram;
static gk_t<GK> Gk;
static int32_t LastKadrEEPChargeUpdate;
static volatile uint8_t ResetFunction NO_INIT;
static WorkData_t workData={APP_IDLE,DEFAULT_KADR};
static turbo_t<DEF_SPEED> Turbo;
static uint8_t TestModeTimer, WdrADC;
static uint8_t saveLen;
static uint16_t saveGK;




INLN void resetPORTs(void){
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
//	Indicator.On();
	for (uint16_t i = 0x4000; i<0x8000; i++) *(uint8_t*) i = 0xAA;	
}


static void trrt(sensorTrrT_t& t, TrrT_t& rt, Dat_m_t& nt, Dat_m_t& tr)
{
	float dt = workData.dat.T - rt.t0;
    tr.X = (nt.X + t.X.d0 + dt*t.X.d) * t.X.kOld * (1.0 + t.X.k*dt);
    tr.Y = (nt.Y + t.Y.d0 + dt*t.Y.d) * t.Y.kOld * (1.0 + t.Y.k*dt);
    tr.Z = (nt.Z + t.Z.d0 + dt*t.Z.d) * t.Z.kOld * (1.0 + t.Z.k*dt);
}

static void trr(m3x4_t& t, Dat_m_t& nt, Dat_m_t& tr)
{
	tr.X = t.m11*nt.X + t.m12*nt.Y + t.m13*nt.Z + t.m14;
	tr.Y = t.m21*nt.X + t.m22*nt.Y + t.m23*nt.Z + t.m24;
	tr.Z = t.m31*nt.X + t.m32*nt.Y + t.m33*nt.Z + t.m34;
}

float RadToDeg(float rad)
{
	float res = rad * (180/M_PI);	
	if (res < 0) return 360+res;
	else return res;
}

static float hypot3D(Dat_m_t& d)
{
	//return sqrt(square(d.X)+square(d.Y)+square(d.Z)); 
	if ((d.X == 0)&&(d.Y==0)&&(d.Z == 0)) return 0;
	float x = fabs(d.X);
	float y = fabs(d.Y);
	float z = fabs(d.Z);
		if (x>y)
		{
			if(x > z)
			// X max
			{
				return x*sqrt(1 + square(z/x)+square(y/x));
			}
		}
		else if (y > z)
		// max y
		{
				return y*sqrt(1 + square(z/y)+square(x/y));//(z+x)/y)
		}
		// max z
		return z*sqrt(1 + square(x/z)+square(y/z));//(x+y)/z)
}

#define  AX workData.dat.accel_m.X
#define  AY workData.dat.accel_m.Y
#define  AZ workData.dat.accel_m.Z
#define  MX workData.dat.magnit_m.X
#define  MY workData.dat.magnit_m.Y
#define  MZ workData.dat.magnit_m.Z
#define  OTK workData.dat.Gtf
#define  ZU workData.dat.Zenit
#define  AZI workData.dat.Azimut
#define MOTK workData.dat.Mtf
#define GTOT workData.dat.Gtot
#define MTOT workData.dat.Mtot
#define MNAK workData.dat.Mn
#define MAGTX dataprog.Inclin.magnit.m3x4
#define ACCTX dataprog.Inclin.accel.m3x4
#define KNT dataprog.toNanoTesla.kmul
#define TRRT  dataprog512

static void UpdateWorkData(void)
{
	adx356.ResetSumm((adx356_adc_t *) &workData.dat.T);
	hmc.ResetSumm(&workData.dat.magnit.X);
	
	float Tcur = workData.dat.T;
	
	uint8_t idxCurT = 0;
	
	for (uint8_t idx = 0; idx < TEMPR_POINTS; idx++)
	{
		float t1 = TRRT.T[idx].t1;
		if (Tcur < t1) //isfinite(t1) && (
		{
			idxCurT = idx;
			break;
		}
	}
		
	Dat_m_t nt;//, tt;
	nt.X = workData.dat.accel.X;
	nt.Y = workData.dat.accel.Y;
	nt.Z = workData.dat.accel.Z;
	trrt(TRRT.T[idxCurT].accel, TRRT.T[idxCurT], nt, workData.dat.accel);
	trr(ACCTX, workData.dat.accel, workData.dat.accel_m);
	
	nt.X = workData.dat.magnit.X;
	nt.Y = workData.dat.magnit.Y;
	nt.Z = workData.dat.magnit.Z;
	trrt(TRRT.T[idxCurT].magnit, TRRT.T[idxCurT], nt, workData.dat.magnit);
	trr(MAGTX, workData.dat.magnit, workData.dat.magnit_m);	
	
	GTOT =hypot3D(workData.dat.accel_m)*10;
	MTOT =hypot3D(workData.dat.magnit_m)*10;
	
	
    float o = atan2(AY, -AX);
    float z = atan2(hypot(AX, AY), AZ);

    float mo = atan2(MY, -MX);
	 
    float os = sin(o);
    float oc = cos(o);
    float zs = sin(z);
    float zc = cos(z);

	float Hx = (MX*oc - MY*os)*zc + MZ*zs;
	float Hy =  MX*os + MY*oc;
	float Hz =-(MX*oc - MY*os)*zs + MZ*zc;

    float a = -atan2(Hy, Hx);
    float b = atan2(hypot(Hx, Hy), Hz);

	workData.dat.magnit_m.X *= KNT;
	workData.dat.magnit_m.Y *= KNT;
	workData.dat.magnit_m.Z *= KNT;
	
	OTK = RadToDeg(o);
	ZU  = RadToDeg(z);
	AZI = RadToDeg(a);
	MOTK = RadToDeg(mo);
	MNAK = RadToDeg(b);
	//float temp = workData.dat.T - 21422;
	//workData.dat.T_m = temp/7864;
	workData.gk.gk = saveGK;
	workData.dat.gk_m = saveGK * daclevel.kGK;
}

// СОБЫТИЕ: постановка на задержку пользователем
static void ResetErrorsInEEP(void)
{
	eep_errors_t e = {0,0,0,DEFAULT_KADR,DEFAULT_KADR};
	eep.Save(EEP_OFFSET_ERR, &e, sizeof(eep_errors_t));
	//FLASH_write_eeprom_block((eeprom_adr_t) EEP_OFFSET_ERR, (uint8_t*) &e, sizeof(eep_errors_t));
	//while(FLASH_is_eeprom_ready());
}
// СОБЫТИЕ: ошибка кварца
static void SaveQzErrInEEP(void)
{
	eep.Save(EEP_OFFSET_ERR_QZ, &workData.time, 4);
	//FLASH_write_eeprom_block((eeprom_adr_t) EEP_OFFSET_ERR_QZ, (uint8_t*) &workData.time, 4);
	//while(FLASH_is_eeprom_ready());
}
// СОБЫТИЕ: аномальный ресет
static void SaveResetInEEP(int32_t* restored_kadr)
{
	reset_t r;
	r.ResetCount = eep_errors.reset.ResetCount+1;
	r.ResetFunction = ResetFunction;
	r.kadr_Reset = *restored_kadr;
	r.ResetRegister = RSTCTRL.RSTFR;
    eep.Save(EEP_OFFSET_ERR, &r, sizeof(r));
	//FLASH_write_eeprom_block((eeprom_adr_t) EEP_OFFSET_ERR , (uint8_t*) &r, sizeof(r));
	//while(FLASH_is_eeprom_ready()); // пусть будет
}
static void SaveChargeAndStateToEEP_Async(void)
{
	eep_save_state_and_charge_t e;
	e.kadr = *(save_state_t*) &workData;
	e.charge = workData.charge;
	LastKadrEEPChargeUpdate = workData.time;
	eep.Save_Async(EEP_OFFSET_KADR_CHARGE, &e, sizeof(eep_save_state_and_charge_t));
}
static void UpdateFromEEP(void)
{
	Gk.updateDACREF(daclevel.DAC);
	ltc2942c::Set(&eep_kadr_charge.charge.AccCharge);
}

// вызывается после ads131.WakeUp();
static void AddSyncFrameSetupADC(void)
{	
	union rwreg_u rw;
	rw.wr.preambula = PRE_WREG;
	rw.wr.adr = 3;
	rw.wr.cnt = 4;
	Uart.buf[0] =rw.bt[1]; //h
	Uart.buf[1] =rw.bt[0]; //l

	union clockreg_u clk;
	clk.clk.ch0_en = 1;
	clk.clk.ch1_en = 1;
	clk.clk.ch2_en = 1;
	clk.clk.ch3_en = 1;
	clk.clk.ch4_en = 1;
	clk.clk.ch5_en = 1;
	clk.clk.ch6_en = 1;
	clk.clk.xtal_dis = 1;
	clk.clk.pwr = 2; // hi power hi resolution
	clk.clk.osr = 7; // max filter setup
	Uart.buf[3] =clk.bt[1]; //h
	Uart.buf[4] =clk.bt[0]; //l

	union gain1reg_u g1;
//	g1.gain.pgagain1 = 1;
//	g1.gain.pgagain2 = 1;
//	g1.gain.pgagain3 = 1;
	Uart.buf[6] =g1.bt[1]; //h no gain
	Uart.buf[7] =g1.bt[0]; //l no gain
		
	union gain2reg_u g2;
//	g2.gain.pgagain4 = 1;
//	g2.gain.pgagain6 = 1;
//	g2.gain.pgagain5 = 1;
	Uart.buf[9] =g2.bt[1]; //h no gain
	Uart.buf[10] =g2.bt[0]; //l no gain

	union cfgreg_u cfg;
	cfg.cfg.gc_en = 1; // global chop
	cfg.cfg.gc_delay = 0b11; //default delay
	Uart.buf[12] =cfg.bt[1]; //h
	Uart.buf[13] =cfg.bt[0]; //l
	
	ads131.AddSyncFrameUserCmd(1,  0);
}

#define READ_REGS_START 0
#define READ_REGS_CNT 7
static void cbReadADC(uint8_t cmd)
{
	ads131.Transaction(READ_REGS_CNT+1+1);
	
	uint8_t* inptr = &Com.buf[DATA_POS]; 
	uint8_t* dptr = Uart.SpiData();
	for (uint8_t i =0; i< READ_REGS_CNT; i++)
	{
		dptr +=3;
		*inptr++ = dptr[1];
		*inptr++ = dptr[0];
	}
	Com.CRCSend(HEADER_LEN+READ_REGS_CNT*2);
}
static void AddSyncFrameReadRegsADC(void)
{
	union rwreg_u rw;
	rw.wr.preambula = PRE_RREG;
	rw.wr.adr = READ_REGS_START;
	rw.wr.cnt = READ_REGS_CNT;
	Uart.buf[0] =rw.bt[1]; //h
	Uart.buf[1] =rw.bt[0]; //l

	ads131.AddSyncFrameUserCmd(2,  cbReadADC);
}

static void ResetWdrADC(void)
{
	if (!WdrADC)
	{
		ads131.Init();
		AddSyncFrameSetupADC(); //?
	}
	WdrADC = 0;
}


static bool Powered(void)
{
	return power.IsOn();
}

static bool AdcWakeUp;

static void WakeUp(void)
{
	batOn.On();
	power.On();
    power_gk.On();
	if (!ads131.IsOn) 
    {
        ads131.WakeUp();
        AdcWakeUp = true;
    }
}
static void StandBy(bool fullStop)
{
	// выполняется вконце фрейма (по даташиту) 
	// (после разрешения прерываний sei() в обработчике ads131.DataReadyHandler();)
	if (ads131.IsOn) ads131.AddSyncFrameStandBy();	
	power.Off();
    power_gk.Off();
	if(fullStop) batOn.Off();
	else batOn.On();
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
			workData.time = *(uint32_t*)(&Com.buf[DATA_POS]);
			// ведущий модуль уже заработал если данный модуль еще спит то нужно также переходить в работу
			if (workData.AppState != APP_WORK)
			{
				workData.AppState = APP_WORK;
				//Incl.on();
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
            
			//uint8_t n = Com.buf[DATA_POS+2];
            buf_len_t n = (Com.Count == HEADER_LEN+CRC_LEN+2+1)? Com.buf[DATA_POS+2] : *(uint16_t*)(&Com.buf[DATA_POS+2]);
			uint16_t from = *(uint16_t*)(&Com.buf[DATA_POS]);
			uint8_t* dptr = &Com.buf[DATA_POS];
						
			if (from == 0) // newbat flash 0x8000
			{
				memcpy(dptr, (const void*) 0x8400, n);
				Com.CRCSend(n+HEADER_LEN);
			}
			else if (from == 8)// vat_vol
			{
                eep.Read(EEP_OFFSET_VOLUME, dptr, n);
				//FLASH_read_eeprom_block((eeprom_adr_t) EEP_OFFSET_VOLUME, dptr, n);
				Com.CRCSend(n+HEADER_LEN);
			}
			else if (from == 16)// eep_errors_t
			{
                eep.Read(EEP_OFFSET_ERR, dptr, n);
				//FLASH_read_eeprom_block((eeprom_adr_t) EEP_OFFSET_ERR, dptr, n);
				Com.CRCSend(n+HEADER_LEN);
			}
			else if (from == 512) // eep_save_state_t + charge_t
			{
                eep.Read(EEP_OFFSET_KADR_CHARGE, dptr, n);
//				FLASH_read_eeprom_block((eeprom_adr_t) EEP_OFFSET_KADR_CHARGE, dptr, n);
				Com.CRCSend(n+HEADER_LEN);
			}
			else if (from == 1024) // daclevel (*(eep_nnk_dac_t*)0x8200)
			{
				memcpy(dptr, (const void*) 0x8600, n);
				Com.CRCSend(n+HEADER_LEN);
			}
			else if (from == 2000)
			{
				memcpy(dptr, (const void*) 0x8000, n);
				Com.CRCSend(n+HEADER_LEN);
			}
			else if (from == 3000)
			{
				memcpy(dptr, (const void*) 0x8200, n);
				Com.CRCSend(n+HEADER_LEN);
			}
			else if (from == 4000)
			{
				AddSyncFrameReadRegsADC();
			}						
		}
		break;

		case CMD_WRITE_EE:
		{
			uint16_t from = *(uint16_t*)(&Com.buf[DATA_POS]);
			buf_len_t n = Com.Count-(HEADER_LEN+2+CRC_LEN);
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
					+ offsetof(charge_t,AccCharge) , (uint8_t*) &data, sizeof(float));
					b->ResetFlag = 0;
					FLASH_write_flash_n(0x400, dptr, n+2);
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
				FLASH_write_flash_n(0x600, dptr, n);
			}
			else if (from == 2000) // daclevel (*(eep_nnk_dac_t*)0x8200)
			{
				FLASH_write_flash_n(0, dptr, n);
			}
			else if (from == 3000) // daclevel (*(eep_nnk_dac_t*)0x8200)
			{
				FLASH_write_flash_n(0x200, dptr, n);
			}
			Com.CRCSend(HEADER_LEN);
			
			UpdateFromEEP();						
		}
		break;
		
		case CMD_WORK:
			saveLen = Com.buf[DATA_POS];
			TestModeTimer = 5;
			if (saveLen > (sizeof(uint32_t)+sizeof(uint8_t)))
			{
				if (!Powered()) 
				{
					WakeUp();
				}
				workData.AppState |= 0x80;				
				Com.CRCSend(&workData, saveLen);
				workData.AppState &= ~0x80;
			}
			else 
			{
				if (Powered() && (workData.AppState != APP_WORK)) StandBy(false);
				Com.CRCSend(&workData, saveLen);
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
			StandBy(false);
		
			// восстановим последний кадр из памяти
			uint32_t* pk = (uint32_t*) &lastKadr;
			Ram.Restore(sizeof(RamData_t), pk);
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

	LastKadrEEPChargeUpdate = workData.time;
	
    
	Com.setRS485mode();
	#ifdef FULL_DUPLEX
    	Com.setFullDuplexmode();
	#endif	
	Com.setBaud(DEF_SPEED);
	Com.intMode();
	Com.enableRxD();
	
	// Reset, setup Uart, Enable DataReady Interrupt, wait Ready
   	ads131.Init();	   
	// будет команда по DataReady (после разрешения прерываний sei() в обработчике ads131.DataReadyHandler();)
	AddSyncFrameSetupADC();
	
	ltc2942c::twi.CallbackRegister(ltc2942c::I2CErr);
	
	sei();	
	
	UpdateFromEEP();
	
	// включаем питание
	if (workData.AppState == APP_WORK) 
	{
		power.On();
	}
	else 
	{
		ads131.AddSyncFrameStandBy();
	} 
	
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
		wdt_reset();

		if (ads131.checkDataReady())
		{
			ads131.DataReadyHandler();
            if (AdcWakeUp)
            {
                AdcWakeUp = false;
                AddSyncFrameSetupADC();
            }
			adx356.SummData(&ads131.data[0]);
			hmc.SummData(&ads131.data[0]);
			WdrADC++;
		}

		ResetFunction = 55;
		ltc2942c::TWIHandler();
		ResetFunction = 0;
		
		if (!eep.Busy() && Clock.IsQzErr())
		{
			SaveQzErrInEEP();
		}
		
		if ( Clock.checkReadyTik()) 
		{				
			Indicator.Handler64(workData.AppState); 				
			ltc2942c::DelayHandler();
			//Incl.handler70ms();
		};
		if (Clock.checkReady2Sec())
		{
			ResetFunction = 10;

			saveGK = Gk.get();
			if ((workData.AppState == APP_WORK) || (TestModeTimer > 0)) UpdateWorkData();
			
			if (TestModeTimer > 0) 
			{
				if(--TestModeTimer == 0) 
				{
					if (workData.AppState != APP_WORK) 					
					{
						StandBy(workData.AppState == APP_IDLE);
					}
				}
				else if (Powered() && (workData.AppState != APP_WORK)) ResetWdrADC();
			}
			else Com.errRS485DirReset();
			
            Turbo.Handler2sec();
            
			workData.time++;				

			ResetFunction = 3;
			ltc2942c::Get(&workData.charge, &eep_batt_volum.volum);
			ResetFunction = 30;
			
			ResetFunction = 31;
			if ((workData.time - LastKadrEEPChargeUpdate >= EEP_UPDATE_KADRS) && (workData.AppState != APP_IDLE)) SaveChargeAndStateToEEP_Async();
			ResetFunction = 32;
			
			switch (workData.AppState)
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
				    //	17.3.25 OLD: if (workData.time == 0)
					if (workData.time >= 0)
					{
						workData.AppState = APP_WORK;						
						Ram.ResetPageBase();
						ResetFunction = 33;
						SaveChargeAndStateToEEP_Async();
						ResetFunction = 34;
						
                        // вариант
                        // ads131.Reset();
                        // AddSyncFrameSetupADC(); //?
                        
                        WakeUp();
  						// После WakeUp() !!!
       					//AddSyncFrameSetupADC(); //?
						//UpdateDAC(); //?
					} 					
				break;					
				case APP_WORK:
				    
					ResetWdrADC();
					
				    ResetFunction = 4;
					Ram.write((uint8_t*) &workData.time, sizeof(RamData_t));	
				    ResetFunction = 40;
				break;
			}
		} // ~if (SysTick.Is2SecTick())
		if (Com.checkReadyRxD())
		{
            uint8_t tmp = Com.buf[0] & 0xF0;
			if ((Com.Count > 2) && ((tmp == ADDRESS) || (tmp == 0xF0)) && (CRC(Com.buf, Com.Count) == 0))
			{
                Turbo.RestartTimer();
				RunCmd();
			}
		} // ~Com.Pool())
    }
}

