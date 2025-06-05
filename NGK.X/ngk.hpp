#pragma once

#if !defined(NGK)
# error "Global define NGK=CMPn,AINPn,TBn,CMPn,AINPn,TBn,CMPn,AINPn,TBn"
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
		  uint8_t timerB2,
		  uint8_t comparator3,
		  uint8_t inpPlus3,
		  uint8_t timerB3>
class ngk_t
{
			#define ngkAC1 (*(AC_t*) (0x0680 + 0x08 * comparator1))
			#define ngkAC2 (*(AC_t*) (0x0680 + 0x08 * comparator2))
			#define ngkAC3 (*(AC_t*) (0x0680 + 0x08 * comparator3))
			#define ngkTB1 (*(TCB_t*) (0x0B00 + 0x10 * timerB1))
			#define ngkTB2 (*(TCB_t*) (0x0B00 + 0x10 * timerB2))
			#define ngkTB3 (*(TCB_t*) (0x0B00 + 0x10 * timerB3))
public:			
	INLN ngk_t(void)
	{
		ngkAC1.CTRLA =
		1 << AC_ENABLE_bp
		| AC_HYSMODE_NONE_gc				/* No hysteresis */
		| AC_POWER_PROFILE0_gc				/* Power profile 0, Shortest response time, highest consumption */
		| 0 << AC_OUTEN_bp					/* Output Buffer Enable: disabled */
		| 1 << AC_RUNSTDBY_bp;				/* Run in Standby Mode: enabled */
		
		ngkAC2.CTRLA =
		1 << AC_ENABLE_bp
		| AC_HYSMODE_NONE_gc				/* No hysteresis */
		| AC_POWER_PROFILE0_gc				/* Power profile 0, Shortest response time, highest consumption */
		| 0 << AC_OUTEN_bp					/* Output Buffer Enable: disabled */
		| 1 << AC_RUNSTDBY_bp;				/* Run in Standby Mode: enabled */

		ngkAC3.CTRLA =
		1 << AC_ENABLE_bp
		| AC_HYSMODE_NONE_gc				/* No hysteresis */
		| AC_POWER_PROFILE0_gc				/* Power profile 0, Shortest response time, highest consumption */
		| 0 << AC_OUTEN_bp					/* Output Buffer Enable: disabled */
		| 1 << AC_RUNSTDBY_bp;				/* Run in Standby Mode: enabled */
		
		ngkAC1.MUXCTRL = (inpPlus1 << 3) | DACREF_bm;
		ngkAC2.MUXCTRL = (inpPlus2 << 3) | DACREF_bm;
		ngkAC3.MUXCTRL = (inpPlus3 << 3) | DACREF_bm;
		
		EVSYS.CHANNEL0 = EVSYS_CHANNEL0_AC0_OUT_gc+comparator1;
		EVSYS.CHANNEL1 = EVSYS_CHANNEL0_AC0_OUT_gc+comparator2;
		EVSYS.CHANNEL2 = EVSYS_CHANNEL0_AC0_OUT_gc+comparator3;
		
		register8_t* user=&EVSYS.USERTCB0COUNT;
		user += 2*timerB1;
		*user = EVSYS_USER_CHANNEL0_gc;
		
		user=&EVSYS.USERTCB0COUNT;
		user += 2*timerB2;
		*user = EVSYS_USER_CHANNEL1_gc;

		user=&EVSYS.USERTCB0COUNT;
		user += 2*timerB3;
		*user = EVSYS_USER_CHANNEL2_gc;
		
		ngkTB1.CNT = 0x0; /* Count: 0x0 */
		ngkTB2.CNT = 0x0; /* Count: 0x0 */
		ngkTB3.CNT = 0x0; /* Count: 0x0 */
		
		ngkTB1.CTRLA = TCB_CLKSEL_EVENT_gc     /* CLK_PER */
		| 1 << TCB_ENABLE_bp   /* Enable: enabled */
		| 1 << TCB_RUNSTDBY_bp /* Run Standby: enabled */
		| 0 << TCB_SYNCUPD_bp  /* Synchronize Update: disabled */
		| 0 << TCB_CASCADE_bp; /* Cascade Two Timer/Counters: disabled */
		
		ngkTB2.CTRLA = TCB_CLKSEL_EVENT_gc     /* CLK_PER */
		| 1 << TCB_ENABLE_bp   /* Enable: enabled */
		| 1 << TCB_RUNSTDBY_bp /* Run Standby: enabled */
		| 0 << TCB_SYNCUPD_bp  /* Synchronize Update: disabled */
		| 0 << TCB_CASCADE_bp; /* Cascade Two Timer/Counters: disabled */
		
		ngkTB3.CTRLA = TCB_CLKSEL_EVENT_gc     /* CLK_PER */
		| 1 << TCB_ENABLE_bp   /* Enable: enabled */
		| 1 << TCB_RUNSTDBY_bp /* Run Standby: enabled */
		| 0 << TCB_SYNCUPD_bp  /* Synchronize Update: disabled */
		| 0 << TCB_CASCADE_bp; /* Cascade Two Timer/Counters: disabled */
	}
		
		
	INLN void updateDACREF(uint16_t* Vmv)
	{
		ngkAC1.DACREF = *Vmv/4;
		Vmv++;
		ngkAC2.DACREF = *Vmv/4;
		Vmv++;
		ngkAC3.DACREF = *Vmv/4;
	}
	INLN void ClearCounts(void)
	{
		ngkTB1.CNT = 0;
		ngkTB2.CNT = 0;
		ngkTB3.CNT = 0;
	}		
	INLN void get(uint16_t* data)
	{
		*data++ = ngkTB1.CNT;
		*data++ = ngkTB2.CNT;
		*data = ngkTB3.CNT;
		ClearCounts();
	}
					
};
