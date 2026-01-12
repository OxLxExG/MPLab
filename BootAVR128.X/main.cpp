/*
 * boot_avr128_com1_pc4.cpp
 *
 * Created: 19.11.2021 13:45:36
 * Author : User
 */ 
#define DUPLEX

#include <avr/io.h>
#include <avr/sleep.h>
#include <crc.h>
#include <ccp.h>
#include <getarg.h>
#include <usarts.hpp>
#include <protokol.h>
#include <flash_n.hpp>
#include <avr/pgmspace.h>

	#ifdef USEUSART0
	 #define Com serial0
	#endif
	#ifdef USEUSART1
	 #define Com serial1
	#endif
	#ifdef USEUSART2
	 #define Com serial2
	#endif
	#ifdef USEUSART3
	 #define Com serial3
	#endif
	#ifdef USEUSART4
	 #define Com serial4
	#endif

inline static void boot_read_page (uint32_t page, uint8_t*buf)
{
	for (uint8_t i = 0; i<128; i++)
	{
		*buf++ = pgm_read_byte_far((flash_adr_t) page++);
	}
}

void __jumpMain     (void) __attribute__ ((naked)) __attribute__ ((section (".vectors")));
void __jumpMain(void)
{
	asm volatile ( "clr __zero_reg__" );        // r1 set to 0
	//SP = RAMEND; после ресета итак показывает
	ccp_write_io((void *)&(CLKCTRL.OSCHFCTRLA),
	CLKCTRL_FRQSEL_8M_gc       /* 8 */
	| 0 << CLKCTRL_AUTOTUNE_bp /* Auto-Tune enable: disabled */
	| 0 << CLKCTRL_RUNSTDBY_bp /* Run standby: disabled */);

	ccp_write_io((void *)&(CLKCTRL.MCLKCTRLA),
	CLKCTRL_CLKSEL_OSCHF_gc /* Internal clock */
	| 0 << CLKCTRL_CLKOUT_bp /* System clock out:  */);
		
	/* wait for system oscillator changing to finish */
	while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm);
		
	if ((pgm_read_dword_far(0x1FFE) == 0x940CFFFF) && *(uint8_t *)EEPROM_START)
    {
			asm volatile ( "jmp 0x2000");// jump to main()
	}		
	else
    {
        asm volatile ( "rjmp __ctors_end");
    }
}

static nvmctrl_status_t FLASH_write_flash_128(flash_adr_t flash_adr, uint8_t* data)
{
	/* check for a new page */
	if (flash_adr % PROGMEM_PAGE_SIZE == 0) {
		/* Erase the flash page and program with desired value(s) */
		/* Wait for completion of previous operation */
		while (NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm | NVMCTRL_FBUSY_bm))
		;

		/* Erase the flash page */
		ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_FLPER_gc);
		/* dummy write to start erase operation */
		FLASH_spm_write_word(flash_adr, 0);

		/* Wait for completion of previous operation */
		while (NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm | NVMCTRL_FBUSY_bm))
		;
		/*A change from one command to another must always go through NOCMD or NOOP*/
		ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);		

	}
	uint16_t *word_buffer = (uint16_t *)data;

	/* Program the page with desired value(s) */
	ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_FLWR_gc);
	/* Write data to the page buffer */
	for (uint16_t i = 0; i < 128 / 2; i++) {
		FLASH_spm_write_word(flash_adr + (i * 2), word_buffer[i]);
	}

	/* Clear the current command */
	ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);

	if (NVMCTRL.STATUS & NVMCTRL_ERROR_gm)
	return NVM_ERROR;
	else
	{
		return NVM_OK;		
	}
}

nvmctrl_status_t FLASH_write_eeprom_byte(eeprom_adr_t eeprom_adr, uint8_t data)
{
	/* Wait for completion of previous operation */
	while (NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm | NVMCTRL_FBUSY_bm))
		;

	/* Program the EEPROM with desired value(s) */
	ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_EEERWR_gc);

	/* Write byte to EEPROM */
	*(uint8_t *)(EEPROM_START + eeprom_adr) = data;

	/* Clear the current command */
	ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);

	/* Wait for completion of previous operation */
	while (NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm | NVMCTRL_FBUSY_bm))
		;
    
	return NVM_OK;
}

int main(void)
{
	Com.setRS485mode();
	#ifdef DUPLEX
   	Com.setFullDuplexmode();
	#endif
	Com.setBaud(125);
	Com.noIntMode();
	Com.enableRxD();
	
	while(1)
	{
		Com.readPool();
		
		if (crc16(Com.buf, Com.Count) != 0)
		{
			continue;
		}
		
		switch(Com.buf[0] & 0x0F)
		{
			case CMD_BOOT_TEST:
			{
				Com.CRCSendPool(Com.Count-2);
			}
			break;
			
			case CMD_BOOT_EXIT:

				Com.CRCSendPool(HEADER_LEN);	
					
				FLASH_write_eeprom_byte(0, 0xFF);
				
  				ccp_write_io((void *)&RSTCTRL.SWRR, 1);
				  
				while(1);
			break;
			
			case CMD_BOOT_READ:
			{
				boot_read_page((*(uint32_t*) &Com.buf[DATA_POS])+0x2000, &Com.buf[DATA_POS+4]);
				//
				Com.CRCSendPool(128+HEADER_LEN+4);
			}
			break;
			
			case CMD_BOOT_WRITE:
			
			if (Com.Count == 128+HEADER_LEN+4+2)
			{
				
				flash_adr_t adr = (*(flash_adr_t*) &Com.buf[DATA_POS]) + 0x2000;

				uint8_t* ptr = &Com.buf[DATA_POS+4];

				*(uint32_t*) &Com.buf[DATA_POS+4] = (FLASH_write_flash_128(adr, ptr) == NVM_OK) ? 0xFFFFFFFF : adr;
				
				Com.CRCSendPool(HEADER_LEN+8);
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

