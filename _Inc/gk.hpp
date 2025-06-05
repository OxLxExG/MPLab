#pragma once

#if !defined(GK)
# error "Global define GK=CMPn,AINPn,TAn"
  // GK=CMP0,AINP0,TA1
#endif

#include <avr/io.h>
#include <tools.h>
#include <getarg.h>
#include <usarts.hpp>


#define CMP0 0
#define CMP1 1
#define CMP2 2

#define TA0  0
#define TA1  1

//#define CMP GETARG_1(GK)

#define AINP0 0
#define AINP1 1
#define AINP2 2
#define AINP3 3

#define  DACREF_bm 0x3

//#if CMP == CMP0
	//#define AC AC0
//#elif CMP == CMP1
	//#define AC AC1
//#else
	//#define AC AC2
//#endif 
template <uint8_t comparatorNo, uint8_t inpPlusNo, uint8_t timerANo>
class gk_t
{
		#define AC (*(AC_t*) (0x0680 + 0x08 * comparatorNo))
		#define TCA  (*(TCA_t*)(0x0A00 + 0x40*timerANo))
private:
/*  	INLINE_FORCE register8_t* getInputPinPort(void)
  	{
	  	switch (inpPlusNo)
	  	{
		  	case 0: return &PORTD.PIN2CTRL;
		  	case 1:
			  switch(comparatorNo)
			  {
				case 0: return &PORTE.PIN0CTRL;
				case 1: return &PORTD.PIN3CTRL;
				case 2: return &PORTD.PIN4CTRL;
			  }		  	
		  	case 2:
			  switch(comparatorNo)
			  {
				  case 0: return &PORTE.PIN2CTRL;
				  case 1: return &PORTD.PIN4CTRL;
				  case 2: return &PORTE.PIN1CTRL;
			  }
		  	case 3: return &PORTD.PIN6CTRL;
	  	}
  	}*/	
public:
	INLN gk_t(void)
	{
		//VREF.ACREF = VREF_ALWAYSON_bm;

	//AC0.INTCTRL = AC_INTMODE_NORMAL_POSEDGE_gc /* Positive input goes above negative input */
	//| 1 << AC_CMP_bp;            /* Analog Comparator Interrupt Enable: enabled */
//
	//AC0.MUXCTRL = 0 << AC_INVERT_bp     /* Invert AC Output: disabled */
	//| AC_MUXNEG_DACREF_gc /* DAC Reference */
	//| AC_MUXPOS_AINP0_gc; /* Positive Pin 0 */
//
	//// AC0.DACREF = 0xff; /* DAC Voltage Reference: 0xff */
//
	//AC0.CTRLA = 1 << AC_ENABLE_bp      /* Enable: enabled */
	//| AC_HYSMODE_NONE_gc   /* No hysteresis */
	//| AC_POWER_PROFILE0_gc /* Power profile 0, Shortest response time, highest consumption */
	//| 0 << AC_OUTEN_bp     /* Output Buffer Enable: disabled */
	//| 1 << AC_RUNSTDBY_bp; /* Run in Standby Mode: enabled */
	
		//AC.MUXCTRL = (inpPlusNo << 3) | DACREF_bm;		
		//AC.DACREF = 0xff;
		//AC.CTRLA = 1 << AC_ENABLE_bp;		/* Enable: enabled */
		
		AC.CTRLA = 
		  1 << AC_ENABLE_bp
		| AC_HYSMODE_NONE_gc				/* No hysteresis */
		| AC_POWER_PROFILE0_gc				/* Power profile 0, Shortest response time, highest consumption */
		| 0 << AC_OUTEN_bp					/* Output Buffer Enable: disabled */
		| 1 << AC_RUNSTDBY_bp;				/* Run in Standby Mode: enabled */
		
		AC.MUXCTRL = (inpPlusNo << 3) | DACREF_bm;
		
		
		EVSYS.CHANNEL0 = EVSYS_CHANNEL0_AC0_OUT_gc+comparatorNo;
		register8_t* user=&EVSYS.USERTCA0CNTA;
		user += 2*timerANo;
		*user = EVSYS_USER_CHANNEL0_gc;
		
		TCA.SINGLE.EVCTRL = TCA_SINGLE_CNTAEI_bm;
		TCA.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc   /* System Clock / 1024 */
				| 1 << TCA_SINGLE_ENABLE_bp    /* Module Enable: enabled */
				| 1 << TCA_SINGLE_RUNSTDBY_bp; /* RUN STANDBY: enabled */
	
	}
	void updateDACREF(uint16_t Vmv)
	{
		AC.DACREF = Vmv/4;		
	}
	uint16_t get(void)
	{
		uint16_t g = TCA.SINGLE.CNT;
		TCA.SINGLE.CNT = 0;
		return g;
	}
};
