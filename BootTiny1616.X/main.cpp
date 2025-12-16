/*
 *
 * Created: 2025 
 * Author : User
 */ 

#include <avr/io.h>
#include <crc.h>
#include <ccp.h>
#include <getarg.h>
#include <tools.h>
#include <protokol.h>
#include <tiny1616uart.hpp>


/** Datatype for flash address */
typedef uint16_t flash_adr_t;

/** Datatype for EEPROM address */
typedef uint16_t eeprom_adr_t;

/** Datatype for return status of NVMCTRL operations */
typedef enum {
	NVM_OK    = 0, ///< NVMCTRL free, no write ongoing.
	NVM_ERROR = 1, ///< NVMCTRL operation retsulted in error
	NVM_BUSY  = 2, ///< NVMCTRL busy, write ongoing.
} nvmctrl_status_t;

static usart_pol_t<ALT1> Com; 

void __jumpMain     (void) __attribute__ ((naked)) __attribute__ ((section (".vectors")));
void __jumpMain(void)
{
	asm volatile ( "clr __zero_reg__" );        // r1 set to 0
	//SP = RAMEND; после ресета итак показывает
    ccp_write_io((void*)&(CLKCTRL.MCLKCTRLB),CLKCTRL_PDIV_2X_gc   // PDIV 2X
            | (1 << CLKCTRL_PEN_bp)   // PEN enabled
            );
		
	if ((*(uint32_t *)(MAPPED_PROGMEM_START + 0x3FE) == 0x940CFFFF) && *(uint8_t*)EEPROM_START) 
    {
			asm volatile ( "jmp 0x400");// jump to main()
	}		
	else
    {
        asm volatile ( "rjmp __ctors_end");
    }
}

inline static void boot_read_page (uint16_t page, uint8_t*buf)
{
	for (uint8_t i = 0; i<128; i++)
	{
		*buf++ = *(uint8_t *)(MAPPED_PROGMEM_START + page++);
	}
}

inline static nvmctrl_status_t Write_flash_stream(flash_adr_t flash_adr, uint8_t data)
{

	bool final_adr_in_page = ((flash_adr & (PROGMEM_PAGE_SIZE - 1)) == (PROGMEM_PAGE_SIZE - 1));

	// Write the new byte value to the correct address in the page buffer
	*(uint8_t *)(MAPPED_PROGMEM_START + flash_adr) = data;

	if (final_adr_in_page)
		// Erase and write the flash page
		ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);

	if (NVMCTRL.STATUS & NVMCTRL_WRERROR_bm)
		return NVM_ERROR;
	else
		return NVM_OK;
}

static void CRCSendPool(uint8_t len)
{
    *(uint16_t*)&Com.buf[len] = crc16(Com.buf, len);
    Com.SendPool(len+2);
}

int main(void)
{	
	while(1)
	{
		Com.readPool();
		
		if (Com.Count< (HEADER_LEN+CRC_LEN) || crc16(Com.buf, Com.Count) != 0)
		{
			continue;
		}
		
		switch(Com.buf[0] & 0x0F)
		{
			case CMD_BOOT_TEST:
			{
				CRCSendPool(Com.Count-2);
			}
			break;
			
			case CMD_BOOT_EXIT:

				CRCSendPool(HEADER_LEN);
                
                /* Clear page buffer */
                ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEBUFCLR_gc);
                *(uint8_t *) EEPROM_START = 0xFF;
                ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);
                while (NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm);


  				ccp_write_io((void *)&RSTCTRL.SWRR, 1);
				  
				while(1);
			break;
			
			case CMD_BOOT_READ:
			{
				boot_read_page((*(uint16_t*) &Com.buf[DATA_POS])+0x400, &Com.buf[DATA_POS+2]);
				//
				CRCSendPool(128+HEADER_LEN+2);
			}
			break;
			
			case CMD_BOOT_WRITE:
			
			if (Com.Count == 128+HEADER_LEN+2+2)
			{
				
				flash_adr_t adr = (*(flash_adr_t*) &Com.buf[DATA_POS]) + 0x400;

				uint8_t* ptr = &Com.buf[DATA_POS+2];

				uint16_t err = 0xFFFF;
                
                for(uint8_t i = 0; i<128; i++)
				{
					if (Write_flash_stream(adr+i, ptr[i]) != NVM_OK)
					{
						err = adr+i;
						break;
					}
				}
				*(uint16_t*) &Com.buf[DATA_POS+2] = err;				
				CRCSendPool(HEADER_LEN+4);
			}
			break;
		}
	}
}
void init9(void) __attribute__ ((naked)) __attribute__ ((section (".init9")));
void init9(void)
{
	main();
}

