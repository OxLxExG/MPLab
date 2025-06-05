#pragma once

#include <avr/io.h>
#include <stddef.h>
#include <string.h>
#include <getarg.h>
#include <tools.h>
#include <ccp.h>

//#define DBG_IND

#ifdef DBG_IND
 #include <Indicator.hpp>
 extern indicator_t<DIOD> Indicator;
#endif

#define BUSY_GPR1_BIT 0x80  // 0x20 Used TWI0 0x40 TWI1
//#define READY_GPR0_BIT 0x40 // 0x20 Used ADS131

#define EEPROM_T(eep) eeprom_t eep; ISR(NVMCTRL_EE_vect) {eep._isr();} 


class eeprom_t
{
private:

    uint8_t buf[32];
	uint8_t writeN;
	uint8_t timout;
	uint8_t* wptr;

public:
	INLN uint8_t Busy(void) { return GPR.GPR1 & BUSY_GPR1_BIT; }
		
	// erase end irq	
	INLN void _isr(void)
	{		
		NVMCTRL.INTCTRL = 0;		
		
		ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_EEWR_gc);
		
		for (uint8_t i = 0; i < writeN; i++) *wptr++ = buf[i];					
		
		ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);		
		
		#ifdef DBG_IND
		Indicator.Off();
		#endif
		
		GPR.GPR1 &= ~BUSY_GPR1_BIT;
	}
	
	INLN void Handler(void)
	{
		if (Busy())
		{
			if (timout-- == 0)
			{
				GPR.GPR1 &= ~BUSY_GPR1_BIT;
			}
		}
	}
		
	bool Save(uint16_t adr, void* data, uint8_t cnt)
	{
		
		if (GPR.GPR1 & BUSY_GPR1_BIT) return false;
		GPR.GPR1 |= BUSY_GPR1_BIT;

		#ifdef DBG_IND
	    Indicator.On();
		#endif
		
		timout = 2;		
				
		memcpy(buf, data, cnt);		
		wptr =  (uint8_t*) (EEPROM_START + adr);
		writeN = cnt;
		
		uint8_t cmd;
		if (cnt <= 4) cmd = NVMCTRL_CMD_EEMBER4_gc;
		else if (cnt <= 8) cmd = NVMCTRL_CMD_EEMBER8_gc;
		else if (cnt <= 16) cmd = NVMCTRL_CMD_EEMBER16_gc;
		else cmd = NVMCTRL_CMD_EEMBER32_gc;
				
		while (NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm | NVMCTRL_FBUSY_bm));		
		ccp_write_spm((void *)&NVMCTRL.CTRLA, cmd);
		
		// критическая секция кода
		cli();
		// dummi write EEPROM
		// begin erase
		*wptr = 0;		 
		// clear eeprom ready flag
		NVMCTRL.INTFLAGS = 1;
		// erase irq Oh 
		NVMCTRL.INTCTRL = 1;
		// check if eeprom not busy run erase end irq Now
		if (NVMCTRL.INTFLAGS) _isr();
		sei();
		
		return true;
	}
	
	
};