#pragma once

#include <avr/io.h>
#include <tools.h>
#include <getarg.h>
#include <ccp.h>

// GPIO.7- 2.097152 interval 2c
// GPIO.6 - 64 times per 2c HTC
// GPIO.6 - 69 times per 2c RTC

 
template <uint8_t clkSel, uint8_t timerANo, uint8_t clockOut>
class clock_t 
{
	#define TCA  (*(TCA_t*)(0x0A00 + 0x40*timerANo))
	
private:
	uint8_t s2cnt;	
	bool QzError;
public:	
	bool HiSpeedReady;

	INLN clock_t (void)
  {
	s2cnt = (clkSel == CLKRTC)? 69 : 64;
	  
	if (clkSel == CLKHT)
	{
		ccp_write_io((void *)&(CLKCTRL.XOSCHFCTRLA),
		1 << CLKCTRL_RUNSTDBY_bp     /* Run standby: disabled */
		| CLKCTRL_CSUTHF_4K_gc  /* 4k XOSCHF cycles */
		| CLKCTRL_FRQRANGE_8M_gc /* Max 8 MHz XTAL Frequency */
		| 0 << CLKCTRL_SEL_bp    /* Source Select: disabled */
		| 1 << CLKCTRL_ENABLE_bp /* External high-frequency Oscillator: enabled */);

		ccp_write_io((void *)&(CLKCTRL.OSCHFCTRLA),
		CLKCTRL_FRQSEL_8M_gc       /* 8 */
		| 0 << CLKCTRL_AUTOTUNE_bp /* Auto-Tune enable: disabled */
		| 0 << CLKCTRL_RUNSTDBY_bp /* Run standby: disabled */);
		
		/* Clear Main Clock Prescaler */
		ccp_write_io((uint8_t *) &CLKCTRL.MCLKCTRLB, 0x00);		

		ccp_write_io((void *)&(CLKCTRL.MCLKCTRLA),
		CLKCTRL_CLKSEL_EXTCLK_gc /* External clock */
		| clockOut << CLKCTRL_CLKOUT_bp /* System clock out */);

		/* wait for system oscillator changing to finish */
		while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm);
	
		/* Enable Clock Failure Detection on main clock */
		ccp_write_io((uint8_t *) &CLKCTRL.MCLKCTRLC, CLKCTRL_CFDSRC_CLKMAIN_gc
		| CLKCTRL_CFDEN_bm);

		/* Enable interrupt for CFD */
		ccp_write_io((uint8_t *) &CLKCTRL.MCLKINTCTRL, CLKCTRL_INTTYPE_bm
		| CLKCTRL_CFD_bm);	
		
		if (timerANo <= 1)
		{
			TCA.SINGLE.INTCTRL = 0 << TCA_SINGLE_CMP0_bp   /* Compare 0 Interrupt: disabled */
			| 0 << TCA_SINGLE_CMP1_bp /* Compare 1 Interrupt: disabled */
			| 0 << TCA_SINGLE_CMP2_bp /* Compare 2 Interrupt: disabled */
			| 1 << TCA_SINGLE_OVF_bp; /* Overflow Interrupt: enabled */

			TCA.SINGLE.PER = T64_TIK-1; /* Period: 0x100 */

			TCA.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc   /* System Clock / 1024 */
			| 1 << TCA_SINGLE_ENABLE_bp    /* Module Enable: enabled */
			| 1 << TCA_SINGLE_RUNSTDBY_bp; /* RUN STANDBY: enabled */		
		}
	}
	else if (clkSel == CLKRC)
	{
		/// TODO: setup internal OSC external XOSC32K
		
		//ccp_write_io((void *)&(CLKCTRL.XOSCHFCTRLA),
		//1 << CLKCTRL_RUNSTDBY_bp     /* Run standby: disabled */
		//| CLKCTRL_CSUTHF_256_gc  /* 256 XOSCHF cycles */
		//| CLKCTRL_FRQRANGE_8M_gc /* Max 8 MHz XTAL Frequency */
		//| 0 << CLKCTRL_SEL_bp    /* Source Select: disabled */
		//| 1 << CLKCTRL_ENABLE_bp /* External high-frequency Oscillator: enabled */);

		ccp_write_io((void *)&(CLKCTRL.OSCHFCTRLA),
		CLKCTRL_FRQSEL_8M_gc          /* 8 */
		| 0 << CLKCTRL_AUTOTUNE_bp /* Auto-Tune enable: disabled */
		| 0 << CLKCTRL_RUNSTDBY_bp /* Run standby: disabled */);

		//ccp_write_io((void *)&(CLKCTRL.MCLKCTRLA),
		//CLKCTRL_CLKSEL_EXTCLK_gc /* External clock */
		//| 0 << CLKCTRL_CLKOUT_bp /* System clock out: disabled */);

		/* wait for system oscillator changing to finish */
		while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm);
		
		if (timerANo <= 1)
		{
			TCA.SINGLE.INTCTRL = 0 << TCA_SINGLE_CMP0_bp   /* Compare 0 Interrupt: disabled */
			| 0 << TCA_SINGLE_CMP1_bp /* Compare 1 Interrupt: disabled */
			| 0 << TCA_SINGLE_CMP2_bp /* Compare 2 Interrupt: disabled */
			| 1 << TCA_SINGLE_OVF_bp; /* Overflow Interrupt: enabled */

			TCA.SINGLE.PER = T64_TIK-1; /* Period: 0x100 */

			TCA.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc   /* System Clock / 1024 */
			| 1 << TCA_SINGLE_ENABLE_bp    /* Module Enable: enabled */
			| 1 << TCA_SINGLE_RUNSTDBY_bp; /* RUN STANDBY: enabled */
		}
	}	
  }
  
   INLN bool IsQzErr(void)
   {
	  if (QzError) 
	  {
		  QzError = false;
		  return true;
	  }
	  else return false;
   }
   
  
	INLN void resetTik(void)
	{
		TCA.SINGLE.CTRLA = 0;
		TCA.SINGLE.CNT = 0;
		s2cnt = (clkSel == CLKRTC)? 69 : 64;
		TCA.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1024_gc   /* System Clock / 1024 */
		| 1 << TCA_SINGLE_ENABLE_bp    /* Module Enable: enabled */
		| 1 << TCA_SINGLE_RUNSTDBY_bp; /* RUN STANDBY: enabled */
	}
	
	INLN void MAXInternalClock(void)
	{
	if (clkSel == CLKHT)
	{
		ccp_write_io((void *)&(CLKCTRL.OSCHFCTRLA),
		CLKCTRL_FRQSEL_24M_gc          /* 8 */
		| 0 << CLKCTRL_AUTOTUNE_bp /* Auto-Tune enable:  */
		| 1 << CLKCTRL_RUNSTDBY_bp /* Run standby:  */);
		
		ccp_write_io((void *)&(CLKCTRL.MCLKCTRLA),
		CLKCTRL_CLKSEL_OSCHF_gc /* Internal clock */
		| clockOut << CLKCTRL_CLKOUT_bp /* System clock out:  */);

		if (timerANo <= 1)
		{
			TCA.SINGLE.PER = T64_TIK_MAX-1; /* Period: 0x100 */
		}
		/* wait for system oscillator changing to finish */
		while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm);		
	}
	// else RTC TODO		
	}
	
	INLN void RestoreExternalClock(void)
	{
		//ccp_write_io((void *)&(CLKCTRL.OSCHFCTRLA),
		//CLKCTRL_FRQSEL_24M_gc          /* 8 */
		//| 0 << CLKCTRL_AUTOTUNE_bp /* Auto-Tune enable:  */
		//| 0 << CLKCTRL_RUNSTDBY_bp /* Run standby:  */);

		ccp_write_io((void *)&(CLKCTRL.OSCHFCTRLA),
		CLKCTRL_FRQSEL_8M_gc          /* 8 */
		| 0 << CLKCTRL_AUTOTUNE_bp /* Auto-Tune enable: disabled */
		| 0 << CLKCTRL_RUNSTDBY_bp /* Run standby: disabled */);
		

		ccp_write_io((void *)&(CLKCTRL.MCLKCTRLA),
		CLKCTRL_CLKSEL_EXTCLK_gc /* ext clock */
		| clockOut << CLKCTRL_CLKOUT_bp /* System clock out:  */);

		if (timerANo <= 1)
		{
			TCA.SINGLE.PER = T64_TIK-1; /* Period: 0x100 */
		}
		/* wait for system oscillator changing to finish */
		while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm);
		
	}

	INLN bool checkReady2Sec(void)
	{
		if (GPR.GPR0 & 0x80)
		{
			GPR.GPR0 &= ~0x80;
			return true;			
		}
		else return false;
	}
	//countdown 32.768ms
	INLN uint8_t Tik(void)
	{
		return s2cnt;
	}
	//countdown 128us
	INLN uint16_t Tik2(void)
	{
		return s2cnt*T64_TIK - TCA.SINGLE.CNT;
	}
	
	// 16mc<N<48mc
	INLN uint8_t NextTik(void)
	{
		return HalfTik() ? s2cnt+1 : s2cnt+2;
	}
	
	INLN bool HalfTik(void)
	{
		return T64_TIK/2 > TCA.SINGLE.CNT;
	}
	INLN uint8_t MaxTik(void)
	{
		return (clkSel == CLKRTC)? 69 : 64;
	}
	INLN bool checkReadyTik(void)
	{
		if (GPR.GPR0 & 0x40)
		{
			GPR.GPR0 &= ~0x40;
			return true;
		}
		else return false;
	}
   
  INLN void _ovf(void)
  {
	TCA.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
	GPR.GPR0 |= 0x40;
	if (--s2cnt == 0)
	{
		s2cnt = (clkSel == CLKRTC)? 69 : 64;
		GPR.GPR0 |= 0x80;
	}
  }
	INLN void _nmi(void)
	{
		ccp_write_io((void *)&(CLKCTRL.OSCHFCTRLA),
		CLKCTRL_FRQSEL_8M_gc       /* 8 */
		| 0 << CLKCTRL_AUTOTUNE_bp /* Auto-Tune enable: disabled */
		| 0 << CLKCTRL_RUNSTDBY_bp /* Run standby: disabled */);

		ccp_write_io((void *)&(CLKCTRL.MCLKCTRLA),
		CLKCTRL_CLKSEL_OSCHF_gc /* Internal clock */
		| clockOut << CLKCTRL_CLKOUT_bp /* System clock out:  */);
		
		/* wait for system oscillator changing to finish */
		while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm);
		
		/* Clear the CFD interrupt flag */
		CLKCTRL.MCLKINTFLAGS = CLKCTRL_CFD_bm;		
					
		QzError = true;
	}
};

#if !defined(CLOCK)
# error "Global define CLOCK=sel,timerA CLKNOOUT"
 // CLOCK=CLKHT,TA0, CLKNOOUT
#endif

typedef clock_t<CLOCK> prj_clock_t;

#define INST_NMI prj_clock_t Clock; ISR(NMI_vect) { if(CLKCTRL.MCLKINTFLAGS & CLKCTRL_CFD_bm) {Clock._nmi();}}

#define CLKTMR GETARG_2(CLOCK)
#if   CLKTMR == TA0
#	define INST_CLOCK  INST_NMI ISR(TCA0_OVF_vect) {Clock._ovf();}
#elif CLKTMR == TA1
#	define INST_CLOCK INST_NMI ISR(TCA1_OVF_vect) {Clock._ovf();}
#elif CLKTMR == TRTC
#	define INST_CLOCK INST_NMI ISR(RTC_CNT_vect) {Clock._ovf();}
#endif

extern prj_clock_t Clock;
