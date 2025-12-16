#pragma once

#include <avr/io.h>
#include <stddef.h>
#include <string.h>
#include <getarg.h>
#include <tools.h>
#include <ccp.h>

#ifdef TSTPIN
 #include <pinout.hpp>
 extern pinout_t<TSTPIN> tstpin;
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
		
        #ifdef TSTPIN
        tstpin.Off();
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
	INLN void _beginSave(uint16_t adr, void* data, uint8_t cnt)
    {
        #ifdef TSTPIN
        tstpin.On();
        #endif

		memcpy(buf, data, cnt);		

		wptr =  (uint8_t*) (EEPROM_START + adr);
		writeN = cnt;
		
		uint8_t cmd;
		if (cnt <= 2) cmd = NVMCTRL_CMD_EEMBER2_gc;
		else if (cnt <= 4) cmd = NVMCTRL_CMD_EEMBER4_gc;
		else if (cnt <= 8) cmd = NVMCTRL_CMD_EEMBER8_gc;
		else if (cnt <= 16) cmd = NVMCTRL_CMD_EEMBER16_gc;
		else cmd = NVMCTRL_CMD_EEMBER32_gc;
				
		while (NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm | NVMCTRL_FBUSY_bm));		
		ccp_write_spm((void *)&NVMCTRL.CTRLA, cmd);        
    }	
    INLN bool SaveBytes(uint16_t adr, void* vdata, uint8_t cnt)
    {
		if (GPR.GPR1 & BUSY_GPR1_BIT) return false;
		GPR.GPR1 |= BUSY_GPR1_BIT;
        
        uint8_t* write = (uint8_t *) (EEPROM_START + adr);
        uint8_t* data = (uint8_t*) vdata;

        /* Wait for completion of previous operation */
        while (NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm | NVMCTRL_FBUSY_bm));

        /* Program the EEPROM with desired value(s) */
        ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_EEERWR_gc);

        do {
            /* Write byte to EEPROM */
            *write++ = *data++;
            cnt--;
        } while (cnt != 0);

        /* Clear the current command */
        ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);

        while (NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm | NVMCTRL_FBUSY_bm));

        GPR.GPR1 &= ~BUSY_GPR1_BIT;

        return true;               
    }
    
    INLN bool Save(uint16_t adr, void* data, uint8_t cnt)
    {
		if (GPR.GPR1 & BUSY_GPR1_BIT) return false;
		GPR.GPR1 |= BUSY_GPR1_BIT;
        
		_beginSave(adr,data,cnt);		

		// dummi write EEPROM
		// begin erase
		*wptr = 0;		 
        
		while (NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm | NVMCTRL_FBUSY_bm));		
		
        _isr();
        
        while (NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm | NVMCTRL_FBUSY_bm));

        return true;		
    }
    
	INLN bool Save_Async(uint16_t adr, void* data, uint8_t cnt)
	{
		
		if (GPR.GPR1 & BUSY_GPR1_BIT) return false;
		GPR.GPR1 |= BUSY_GPR1_BIT;

		_beginSave(adr,data,cnt);		
		
		timout = 2;		
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
    INLN void Read(uint16_t eeprom_adr, uint8_t *data, uint8_t size)
    {
        // Read operation will be stalled by hardware if any write is in progress
        memcpy(data, (uint8_t *)(EEPROM_START + eeprom_adr), size);
    }
		
};