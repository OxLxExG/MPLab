#pragma once

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <ccp.h>
#include <string.h>

/** Datatype for flash address */
typedef uint32_t flash_adr_t;

/** Datatype for EEPROM address */
typedef uint16_t eeprom_adr_t;

/** Datatype for return status of NVMCTRL operations */
typedef enum {
	NVM_OK    = 0, ///< NVMCTRL free, no write ongoing.
	NVM_ERROR = 1, ///< NVMCTRL operation retsulted in error
	NVM_BUSY  = 2, ///< NVMCTRL busy, write ongoing.
} nvmctrl_status_t;


/**
 * \brief SPM based word write to flash
 *
 * \param[in] address   The word-address in flash to write
 * \param[in] word      The word data to write to the flash
 *
 * \return The read byte
 */

inline void FLASH_spm_write_word(uint32_t address, uint16_t word)
{
	__asm__ __volatile__("push r0\n\t"       /* back up R0*/
	                     "lds r0, %0\n\t"    /* back up RAMPZ*/
	                     "push r0\n\t"       /* back up RAMPZ*/
	                     "movw r0, %A2\n\t"  /* update R0,R1 pair with word*/
	                     "movw r30, %A1\n\t" /* update R30,R31 pair with address[Byte1,Byte0]*/
	                     "sts  %0, %C1\n\t"  /* update RAMPZ with address[Byte2]*/
	                     "spm \n\t"          /* spm */
	                     "clr r1\n\t"        /* R1 is always assumed to be zero by the compiler. Resetting R1 to zero*/
	                     "pop r0\n\t"        /* restore RAMPZ*/
	                     "sts %0, r0\n\t"    /* restore RAMPZ*/
	                     "pop r0 \n\t"       /* restore R0*/
	                     :
	                     : "i"(_SFR_MEM_ADDR(RAMPZ)), "r"((uint32_t)(address)), "r"((uint16_t)(word))
	                     : "r30", "r31" /* Clobber R30, R31 to indicate they are used here*/
	); 
}

inline nvmctrl_status_t FLASH_write_flash_n(uint16_t from,  uint8_t *data, uint16_t n, bool erase=true)
{
	uint16_t *word_buffer = (uint16_t *)data;

	/* Wait for completion of previous operation */
	while (NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm | NVMCTRL_FBUSY_bm))
	;
	if (erase)
	{
		/* Erase the flash page */
		ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_FLPER_gc);

		/* Perform a dummy write to this address to update the address register in NVMCTL */
		FLASH_spm_write_word(0x18000 + from, 0);

		/* Wait for completion of previous operation */
		while (NVMCTRL.STATUS & NVMCTRL_FBUSY_bm)
		;
	}

	/*A change from one command to another must always go through NOCMD or NOOP*/
	ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);

	/* Write the flash page */
	ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_FLWR_gc);

	/* Write data to the page buffer */
	for (uint16_t i = 0; i < n / 2; i++) {
		FLASH_spm_write_word(0x18000 + from + (i * 2), word_buffer[i]);
	}

	/* Clear the current command */
	ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);

	if (NVMCTRL.STATUS & NVMCTRL_ERROR_gm)
	return NVM_ERROR;
	else
	return NVM_OK;
}

///**
// * \brief Write a block to eeprom
// *
// * \param[in] eeprom_adr The byte-address in eeprom to write to
// * \param[in] data The buffer to write
// *
// * \return Status of write operation
// */
//inline nvmctrl_status_t FLASH_write_eeprom_block(eeprom_adr_t eeprom_adr, uint8_t *data, size_t size)
//{
//	uint8_t *write = (uint8_t *)(EEPROM_START + eeprom_adr);
//
//	/* Wait for completion of previous operation */
//	while (NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm | NVMCTRL_FBUSY_bm))
//		;
//
//	/* Program the EEPROM with desired value(s) */
//	ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_EEERWR_gc);
//
//	do {
//		/* Write byte to EEPROM */
//		*write++ = *data++;
//		size--;
//	} while (size != 0);
//
//	/* Clear the current command */
//	ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);
//
//	return NVM_OK;
//}
//
///**
// * \brief Check if the EEPROM can accept data to be read or written
// *
// * \return The status of EEPROM busy check
// * \retval false The EEPROM can not receive data to be read or written
// * \retval true The EEPROM can receive data to be read or written
// */
//inline bool FLASH_is_eeprom_ready()
//{
//	return (NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm | NVMCTRL_FBUSY_bm));
//}
//
///**
// * \brief Write a byte to eeprom
// *
// * \param[in] eeprom_adr The byte-address in eeprom to write to
// * \param[in] data The byte to write
// *
// * \return Status of write operation
// */
//inline nvmctrl_status_t FLASH_write_eeprom_byte(eeprom_adr_t eeprom_adr, uint8_t data)
//{
//	/* Wait for completion of previous operation */
//	while (NVMCTRL.STATUS & (NVMCTRL_EEBUSY_bm | NVMCTRL_FBUSY_bm))
//		;
//
//	/* Program the EEPROM with desired value(s) */
//	ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_EEERWR_gc);
//
//	/* Write byte to EEPROM */
//	*(uint8_t *)(EEPROM_START + eeprom_adr) = data;
//
//	/* Clear the current command */
//	ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_NONE_gc);
//
//	return NVM_OK;
//}
//
//inline void FLASH_read_eeprom_block(eeprom_adr_t eeprom_adr, uint8_t *data, size_t size)
//{
//// Read operation will be stalled by hardware if any write is in progress
//memcpy(data, (uint8_t *)(EEPROM_START + eeprom_adr), size);
//}
