
#pragma once


#if !defined(DIOD)
# error "Global define DIOD=PORT,PIN,INV"
// DIOD=PF,2,INV
#endif

#include <protokol.h>
#include <tools.h>
#include <avr/io.h>

#define  IND_BEGIN 0xFFFFFFFFFFFFFFFF
#define  IND_ERROR  0xF0F0F0F0F0F0F0F0
#define  IND_ERROR2 0xFF00FF00FF00FF00
#define  IND_ERROR3 0xFFF000FFF000FF00
#define  IND_1 0xF0
#define  IND_2 0xF0F0
#define  IND_3 0xF0F0F0
#define  IND_4 0xF0F0F0F0

template <uint8_t port, uint8_t pin, uint8_t inv>
class indicator_t
{
	#define PINDIC (*(PORT_t*) (0x0400 + 0x20 * port))
	
	private:
	 unio64_t data;
	 uint8_t cntCurrent;
	 uint8_t indexCurrent;
	 uint8_t current;
//	 uint8_t appSatate;
	 //uint8_t appTurbo;
	public:
	 uint8_t appSatate;
	bool User;
	bool Error3;
	bool Error2;
	bool Error1;
	
	INLN indicator_t(void)
	{
		data.u64 = IND_BEGIN; 
		appSatate = 0xFF;
		PINDIC.DIRSET = 1 << pin;
		Off();
	}
	void SetCriticalError1(void)
	{
		Error1 = true;
		data.u64 = IND_ERROR;
		cntCurrent = 7;
	}
	void SetCriticalError2(void)
	{
		Error2 = true;
		data.u64 = IND_ERROR2;
		cntCurrent = 7;
	}
	void SetCriticalError3(void)
	{
		Error3 = true;
		data.u64 = IND_ERROR3;		
		cntCurrent = 7;
	}	
	INLN void Not(void)
	{
		PINDIC.OUTTGL = 1 << pin;		
	}
	INLN void On(void)
	{
		if (inv) PINDIC.OUTCLR = 1 << pin;
		else PINDIC.OUTSET = 1 << pin;
		
	}
	INLN void Off(void)
	{
		if (inv) PINDIC.OUTSET = 1 << pin;
		else PINDIC.OUTCLR = 1 << pin;		
	}
	void Handler64(uint8_t status)
	{
		if (User) return;
		uint8_t st = status & 0x3F;
		if (Error3 ||Error2 || Error1)
		{
			
		}
		//else if (turbo != appTurbo)
		//{
			//appTurbo = turbo;
			//if (turbo) data.u64 = IND_ERROR;
			//else appSatate = 0; 
		//}
		else if (appSatate != st)
		{
			//data.u64 = IND_ERROR;
			switch(st)
			{
			case  APP_CLEAR_RAM:
			 data.u64 = 0;
			break;
			case  APP_DELAY:
              data.u64 = IND_1;
			break;
			case  APP_WORK:
			 data.u64 = IND_2;			
			break;
			case  APP_IDLE:
			 data.u64 = IND_BEGIN;
			break;				
			}
			cntCurrent = 7;
			indexCurrent = 7;
			appSatate = st;
		}
		(current &  1)? On(): Off(); 
		current >>= 1;
		if (++cntCurrent == 8)
		{
			cntCurrent = 0;
			current = data.B[indexCurrent];
			if (++indexCurrent == 8) indexCurrent = 0;
		} 
	}
};

extern indicator_t<DIOD> Indicator;