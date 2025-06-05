#pragma once

#include <avr/io.h>
#include <tools.h>
#include <twi_im.hpp>
#include <getarg.h>

typedef struct __attribute__((packed))
{
	uint8_t status;
	float AccCharge;
	float remainCharge;
	uint8_t proc;
	uint16_t HoursLleft;
	float T;
	float V;
	float I;
} ltc2942data_t;

typedef union
{
	uint16_t qzLH[2];
	uint32_t qz;
} charge_u;

typedef  twi_im_t<LTC2942> i2ci_inst;


#define  TWI_N GETARG_1(LTC2942)

#define  TWI_IX_VARS \
	uint8_t ltc2942c::timeout; uint8_t ltc2942c::send[3]; i2ci_inst ltc2942c::twi; \
	uint8_t ltc2942c::read[2]; float ltc2942c::expfltI = 10; charge_u ltc2942c::Carge; \
	volatile uint8_t ltc2942c::StateSetC; volatile uint8_t ltc2942c::StateGetC; charge_u ltc2942c::SetCarge; \
	ltc2942data_t ltc2942c::pdata; float* ltc2942c::BatVolum;
	
#if TWI_N == I2C0
# define  INSTATCE_LTC2942C(x) TWI_IX_VARS; ISR(TWI0_TWIM_vect) {ltc2942c::twi._isr(); }
#else
# define  INSTATCE_LTC2942C(x) TWI_IX_VARS; ISR(TWI1_TWIM_vect) {ltc2942c::twi._isr(); }
#endif

class ltc2942c
{
	public:
	static uint8_t timeout;
	static uint8_t send[3];
	static uint8_t read[2];
	static i2ci_inst twi;

	static float expfltI;
	static charge_u Carge;
	static charge_u SetCarge;

	static volatile uint8_t StateSetC;
	static volatile uint8_t StateGetC;
	
	static ltc2942data_t pdata;
	static float* BatVolum;

	#define  LTCADR  0b1100100
	#define  CONTDEF 0b00000000
	#define  CONTV   0b10000000
	#define  CONTT   0b01000000
	#define  ACOVF   0b00100000
	enum{
		// NAME ADDRESS           REGISTER DESCRIPTION R/W DEFAULT
		STASTUS=0x00,				// A Status R See Below
		CONTROL=0x01,				// B Control  R/W 3Ch
		ACC_CHARGE_H=0x02,			// C Accumulated Charge MSB R/W 7Fh
		ACC_CHARGE_L=0x03,			// D Accumulated Charge LSB  R/W FFh
		THRESHOLD_CHARGE_HI_H=0x04, // E Charge Threshold High MSB R/W FFh
		THRESHOLD_CHARGE_HI_L=0x05, //F Charge Threshold High LSB R/W FFh
		THRESHOLD_CHARGE_LO_H=0x06, //G Charge Threshold Low MSB R/W 00h
		THRESHOLD_CHARGE_LO_L=0x07, //H Charge Threshold Low LSB  R/W 00h
		V_H=0x08,					// I Voltage MSB R XXh
		V_L=0x09,					// J Voltage LSB  R XXh
		THRESHOLD_V_HI=0x0A,		// K Voltage Threshold High R/W FFh
		THRESHOLD_V_LO=0x0B,		// L Voltage Threshold Low  R/W 00h
		T_H= 0x0C,					// M Temperature MSB R XXh
		T_L= 0x0D,					// N Temperature LSB  R XXh
		THRESHOLD_T_HI=0x0E,		// O Temperature Threshold High R/W FFh
		THRESHOLD_T_LO=0x0F,		// P Temperature Threshold Low  R/W 00h
	};
	
	static void TWIHandler(void)
	{
		if (!twi.IsBusy())
		{
			if (StateGetC)
			{
				switch (StateGetC)
				{
					case 1:
					if (!StateSetC)
					{
						twi.Initialize();
						send[0] = CONTROL; send[1] = CONTV;
						twi.Write(LTCADR, send, 2);
						StateGetC = 2;
					}
					break;
					
					case 3:
					pdata.V = ScaleV((uint16_t) read[0]<<8 | read[1]);
					send[0] = CONTROL; send[1] = CONTT;
					twi.Write(LTCADR, send, 2);
					StateGetC = 4;
					break;
					
					case 5:
					pdata.T = ScaleT((uint16_t) read[0]<<8 | read[1]);
					send[0] = STASTUS;
					twi.WriteRead(LTCADR, send, 1, read, 1);
					StateGetC = 6;
					break;
					
					case 6:
					pdata.status = read[0];
					if (pdata.status & ACOVF)
					{
						send[0] = ACC_CHARGE_H; send[1] = 0; send[2] = 0;
						twi.Write(LTCADR, send, 3);
						StateGetC = 7;
					}
					else
					{
						send[0] = ACC_CHARGE_H;
						twi.WriteRead(LTCADR, send, 1, read, 2);
						StateGetC = 8;
					}
					break;
					
					case 7:
					Carge.qzLH[1]++;
					Carge.qzLH[0] = 0;
					StopGet();
					break;
					
					case 8:
					uint16_t tmpC =  (uint16_t) read[0]<<8 | read[1];
					pdata.I = ScaleI(tmpC-Carge.qzLH[0]);
					expfltI = expfltI * (1 - 0.1) + pdata.I * 0.1;
					Carge.qzLH[0] = tmpC;
					StopGet();
					break;					
				};				
			}
			if (StateSetC)
			{
				switch(StateSetC)
				{
					case 1:
					if (!StateGetC)
					{
						send[0] = ACC_CHARGE_H; send[1] = (uint8_t)(SetCarge.qzLH[0]>>8); send[2] = (uint8_t) SetCarge.qzLH[0];
						twi.Initialize();
						twi.Write(LTCADR,send,3);						
						StateSetC = 2;
					}					
					break;
					
					case 2:
					  Carge.qz = SetCarge.qz;
					  StateSetC = 0;
					  twi.Deinitialize();
					break;
				}
			}
		}
	}
	static void DelayHandler(void)
	{
		//if (!twi.IsBusy())
		{
			switch (StateGetC)
			{
				case 2:
					send[0] = V_H;
					twi.WriteRead(LTCADR, send, 1, read, 2);
					StateGetC = 3;
				break;
				
				case 4:
					send[0] = T_H;
					twi.WriteRead(LTCADR, send, 1, read, 2);
					StateGetC = 5;
				break;				
			};
		}
	}
    static void Get(void* data, float* bat)
	{
		memcpy(data, &pdata, sizeof(ltc2942data_t));
		BatVolum = bat;
		if (!StateGetC) 
		{
			StateGetC = 1;
			TWIHandler();
		}
	}
    static void Set(float* ch)
    {
		SetCarge.qz = ReScaleCharge(*ch);		
		StateSetC = 1;
		TWIHandler();		
	}
	
	static void I2CErr(void)
	{
		i2c_host_error_t e = twi.ErrorGet();
		pdata.status = e;		
		StateSetC = 0;
		StateGetC = 0;
		twi.Deinitialize();
	}
	
	private:
	
	static void StopGet(void)
	{
		pdata.AccCharge = ScaleCharge(Carge.qz);
		pdata.remainCharge = *BatVolum - pdata.AccCharge;
		
		if (pdata.remainCharge < 0) pdata.proc = 0;
		else pdata.proc = pdata.remainCharge/ *BatVolum*100;
		if (pdata.remainCharge < 0) pdata.HoursLleft = 0;
		else if (expfltI > 1) pdata.HoursLleft =  pdata.remainCharge*1000/expfltI;
		
		StateGetC = 0;
		twi.Deinitialize();
	}
INLN static float ScaleI(int32_t dC)
{
	return dC * 0.085/128/ (2.097152/3600);
}
INLN static float ScaleT(uint16_t rowT)
{
	return 600.0/0xFFFF*rowT - 273;
}
INLN static float ScaleV(uint16_t rowV)
{
	return 6.0/0xFFFF*rowV;
}
INLN static float ScaleCharge(uint32_t rowC)
{
	return 0.085/128 * rowC / 1000.0;
}
INLN  static uint32_t ReScaleCharge(float rowC)
{
	return rowC * 1000.0/0.085*128;
}
	
};
	
