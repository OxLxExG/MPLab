/*
 *
 * Created: 2025 
 * Author : User
 */ 

#include <avr/io.h>
#include <crc.h>
#include <ccp.h>
#include <getarg.h>
#include <protokol.h>
#include <tools.h>
#include <MetaData.h>
#include <tiny1616uart.hpp>

static usart_pol_t<ALT1> Com; 
static uint16_t cnt0;
static uint16_t cnt1;

 ISR(AC0_AC_vect)
 {
     AC0.STATUS = 1;
     cnt0++;
 }
 ISR(AC1_AC_vect)
 {     
     AC1.STATUS = 1;
     cnt1++;
 }

int main(void)
{	
      Com.buf[0] = cmetaAll[0];
    ccp_write_io((void*)&(CLKCTRL.MCLKCTRLB),CLKCTRL_PDIV_2X_gc   // PDIV 2X
            | (1 << CLKCTRL_PEN_bp)   // PEN enabled
            );

    #define mV_TO_DAC 10
    VREF.CTRLC = VREF_DAC1REFSEL_2V5_gc;
    VREF.CTRLA = VREF_DAC0REFSEL_2V5_gc;
    DAC0.CTRLA = 1;
    DAC1.CTRLA = 1;
    
    DAC0.DATA = *(uint8_t *) (EEPROM_START+EEPROM_PAGE_SIZE);
    DAC1.DATA = *(uint8_t *) (EEPROM_START+EEPROM_PAGE_SIZE+1);
    
    
    AC0.CTRLA =
        1 << AC_ENABLE_bp
        | AC_INTMODE_POSEDGE_gc				
        ;
    AC0.MUXCTRLA =
        AC_MUXNEG_DAC_gc
        | AC_MUXPOS_PIN0_gc
        ;
    AC0.INTCTRL = 1;
    
    AC1.CTRLA =
        1 << AC_ENABLE_bp
        | AC_INTMODE_POSEDGE_gc	
        ;
    AC1.MUXCTRLA =
        AC_MUXNEG_DAC_gc
        | AC_MUXPOS_PIN1_gc
        ;
    AC1.INTCTRL = 1;

    
	while(1)
	{
		Com.readPool();
		
		if (Com.Count>0)
        {
            switch(Com.buf[0])
            {			
                case CMD_BOOT:
                if (Com.Count == (HEADER_LEN+CRC_LEN+4) 
                && crc16(Com.buf, Com.Count) == 0 
                && (*(uint32_t*)(&Com.buf[DATA_POS]) == 0x12345611))
                {
                    cli();
                    /* Clear page buffer */
                    ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEBUFCLR_gc);
                    *(uint8_t *) EEPROM_START = 0x0;
                    ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);
                    while (NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm);


                    ccp_write_io((void *)&RSTCTRL.SWRR, 1);

                    while(1);
                }
                break;
                
                case 1:
                    if (Com.Count == 5)
                    {
                        /* Clear page buffer */
                        ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEBUFCLR_gc);
                        *(uint8_t *) (EEPROM_START+EEPROM_PAGE_SIZE) = (*(uint16_t *)&Com.buf[1])/mV_TO_DAC;
                        *(uint8_t *) (EEPROM_START+EEPROM_PAGE_SIZE+1) = (*(uint16_t *)&Com.buf[3])/mV_TO_DAC;
                        ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);
                        while (NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm);
                        DAC0.DATA = *(uint8_t *) (EEPROM_START+EEPROM_PAGE_SIZE);
                        DAC1.DATA = *(uint8_t *) (EEPROM_START+EEPROM_PAGE_SIZE+1);
                    }                    
                break;    
                case 2:
                    if (Com.Count == 1)
                    {
                        *(uint16_t *) &Com.buf[0] = (uint16_t) (*(uint8_t *) (EEPROM_START+EEPROM_PAGE_SIZE) * mV_TO_DAC);
                        *(uint16_t *) &Com.buf[2] = (uint16_t) (*(uint8_t *) (EEPROM_START+EEPROM_PAGE_SIZE+1) * mV_TO_DAC);
                        Com.SendPool(4);
                    }                    
                break;    
                case 3:
                    if (Com.Count == 1)
                    {
                      cli();
                      *(uint16_t *) &Com.buf[0] = cnt0;
                      *(uint16_t *) &Com.buf[2] = cnt1;
                      cnt0 = cnt1 = 0;
                      sei();
                      Com.SendPool(4);
                    }                    
                break;  
                case 4:
                    ReadMetaData(&Com.buf[0], sizeof(cmetaAll), 0);
                    Com.SendPool(sizeof(cmetaAll));
                break;
                
            }
        }
	}
}
