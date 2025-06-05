#pragma once

#if !defined(NNK)
# error "Global define NNK=CMPn,AINPn,TBn,CMPn,AINPn,TBn"
// GK=CMP0,AINP0,TA1
#endif

#include <avr/io.h>
#include <tools.h>
#include <getarg.h>


#define  DACREF_bm 0x3

template <uint8_t comparator1, 
          uint8_t inpPlus1, 
		  uint8_t timerB1,
		  uint8_t comparator2,
		  uint8_t inpPlus2,
		  uint8_t timerB2>
class nnk_t
{
			#define nnkAC1 (*(AC_t*) (0x0680 + 0x08 * comparator1))
			#define nnkAC2 (*(AC_t*) (0x0680 + 0x08 * comparator2))
			#define nnkTB1 (*(TCB_t*) (0x0B00 + 0x10 * timerB1))
			#define nnkTB2 (*(TCB_t*) (0x0B00 + 0x10 * timerB2))
public:			
	INLN nnk_t(void)
	{
		nnkAC1.CTRLA =
		1 << AC_ENABLE_bp
		| AC_HYSMODE_NONE_gc				/* No hysteresis */
		| AC_POWER_PROFILE0_gc				/* Power profile 0, Shortest response time, highest consumption */
		| 0 << AC_OUTEN_bp					/* Output Buffer Enable: disabled */
		| 1 << AC_RUNSTDBY_bp;				/* Run in Standby Mode: enabled */
		
		nnkAC2.CTRLA =
		1 << AC_ENABLE_bp
		| AC_HYSMODE_NONE_gc				/* No hysteresis */
		| AC_POWER_PROFILE0_gc				/* Power profile 0, Shortest response time, highest consumption */
		| 0 << AC_OUTEN_bp					/* Output Buffer Enable: disabled */
		| 1 << AC_RUNSTDBY_bp;				/* Run in Standby Mode: enabled */
		
		nnkAC1.MUXCTRL = (inpPlus1 << 3) | DACREF_bm;
		nnkAC2.MUXCTRL = (inpPlus2 << 3) | DACREF_bm;
		
		EVSYS.CHANNEL0 = EVSYS_CHANNEL0_AC0_OUT_gc+comparator1;
		EVSYS.CHANNEL1 = EVSYS_CHANNEL0_AC0_OUT_gc+comparator2;
		
		register8_t* user=&EVSYS.USERTCB0COUNT;
		user += 2*timerB1;
		*user = EVSYS_USER_CHANNEL0_gc;
		
		user=&EVSYS.USERTCB0COUNT;
		user += 2*timerB2;
		*user = EVSYS_USER_CHANNEL1_gc;
		
		nnkTB1.CNT = 0x0; /* Count: 0x0 */
		nnkTB2.CNT = 0x0; /* Count: 0x0 */
		
		nnkTB1.CTRLA = TCB_CLKSEL_EVENT_gc     /* CLK_PER */
		| 1 << TCB_ENABLE_bp   /* Enable: enabled */
		| 1 << TCB_RUNSTDBY_bp /* Run Standby: enabled */
		| 0 << TCB_SYNCUPD_bp  /* Synchronize Update: disabled */
		| 0 << TCB_CASCADE_bp; /* Cascade Two Timer/Counters: disabled */
		
		nnkTB2.CTRLA = TCB_CLKSEL_EVENT_gc     /* CLK_PER */
		| 1 << TCB_ENABLE_bp   /* Enable: enabled */
		| 1 << TCB_RUNSTDBY_bp /* Run Standby: enabled */
		| 0 << TCB_SYNCUPD_bp  /* Synchronize Update: disabled */
		| 0 << TCB_CASCADE_bp; /* Cascade Two Timer/Counters: disabled */
	}
		
		
	INLN void updateDACREF(uint16_t* Vmv)
	{
		nnkAC1.DACREF = *Vmv/4;
		Vmv++;
		nnkAC2.DACREF = *Vmv/4;
	}
	INLN void ClearCounts(void)
	{
		nnkTB1.CNT = 0;
		nnkTB2.CNT = 0;
	}		
	INLN void get(uint16_t* data)
	{
		*data++ = nnkTB1.CNT;
		*data = nnkTB2.CNT;
		ClearCounts();
	}
					
};
