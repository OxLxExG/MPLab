#pragma once

#include <avr/io.h>
#include <util/delay.h>
#include <usarts.hpp>
#include <getarg.h>
#include <protokol.h>
#include <clock.hpp>
#include <tools.h>
#include <crc.h>

#define  SPI_UART_N GETARG_5(MX25L256)
#if   SPI_UART_N == 0
#define MX25L256_Uart serial0
#elif SPI_UART_N == 1
//EXTERN_USART(1)
#define MX25L256_Uart serial1
#elif SPI_UART_N == 2
#define MX25L256_Uart serial2
#elif SPI_UART_N == 3
#define MX25L256_Uart serial3
#elif SPI_UART_N == 4
#define MX25L256_Uart serial4
#endif

/*	
	The Page Program (PP) instruction is for programming memory bits to "0". One to 256 bytes can be sent to the
	device to be programmed. A Write Enable (WREN) instruction must be executed to set the Write Enable Latch (WEL)
	bit before sending the Page Program (PP). If more than 256 data bytes are sent to the device, only the last 256
	data bytes will be accepted and the previous data bytes will be disregarded. The Page Program instruction requires
	that all the data bytes fall within the same 256-byte page. The low order address byte A[7:0] specifies the starting
	address within the selected page. Bytes that will cross a page boundary will wrap to the beginning of the selected
	page. The device can accept (256 minus A[7:0]) data bytes without wrapping. If 256 data bytes are going to be
	programmed, A[7:0] should be set to 0.
*/	

/*
  SPI in Normal mode!
*/

//Continuous array read (low frequency) 03H
#define FLASH_READ        0x13    //READ4B (1 x I/O with 4 byte address)
#define FLASH_CHIP_ERASE  0x60    //CE (Chip Erase) hex code: 60 or C7
#define FLASH_RDSR    0x05    //RDSR (Read Status Register)
#define FLASH_PP4B    0x12    //PP4B (page program with 4 byte address)
#define FLASH_WREN    0x06    //WREN (Write Enable)

#define STATUS_BIT_WIP  0
#define STATUS_BIT_WEL  1
#define STATUS_READ_READY 0
#define STATUS_WRITE_READY (1<<STATUS_BIT_WEL)
#define RAM_END 0x2000
#define PP_TIMAUT_MS(ms) (2*ms)

template <uint8_t spiNo, uint8_t pmuxAlt, uint8_t portCS, uint8_t bitCS, uint8_t serialNo>
class ram_t 
{
		#define SPI (*(SPI_t*) (0x0940 + 0x20 * spiNo))
		//#define TB (*(TCB_t*) (0x0B00 + 0x10 * timerNo))
		#define PCS (*(PORT_t*) (0x0400 + 0x20 * portCS))
private:
	unio32_t waddr;
	uint8_t page[256];
	uint16_t pageCnt;
	INLN PORT_t* getSPIPort(void)
	{
		if (spiNo == SP1)
		{
			switch (pmuxAlt)
			{
				case DEF0:
				return &PORTC;
				case ALT1:
				return &PORTC;
				case ALT2:
				return &PORTB;
			}
		} 
		else
		{
			switch (pmuxAlt)
			{
				case DEF0:
				return &PORTA;
				case ALT1:
				return &PORTE;
			}
		}
	}				
	INLN uint8_t getSPIbit(void)
	{
		if (spiNo == SP1)
		{
			switch (pmuxAlt)
			{
				case DEF0:
				return 0;
				case ALT1:
				return 4;
				case ALT2:
				return 4;
			}
		}
		else
		{
			switch (pmuxAlt)
			{
				case DEF0:
				return 4;
				case ALT1:
				return 0;
			}
		}
	}
    INLN void CS_on(void){PCS.OUTCLR = 1 << bitCS;};
    INLN void CS_off(void){PCS.OUTSET = 1 << bitCS;};
		
    INLN void init(void)
	{
			SPI.CTRLA = 1 << SPI_CLK2X_bp |    /* Enable Double Speed: enabled */
						0 << SPI_DORD_bp   /* Data Order Setting: disabled */
			| 1 << SPI_ENABLE_bp /* Enable Module: enabled */
			| 1 << SPI_MASTER_bp /* SPI module in master mode */
			| SPI_PRESC_DIV4_gc; /* System Clock / 4 */ // 2*2000000 = 4MHz
			
			SPI.INTFLAGS; 

	}
    INLN void deinit(void){SPI.CTRLA = 0; SPI.INTFLAGS;}
	INLN uint8_t spi(const uint8_t d)
	{
		SPI.DATA = d;
		while (!(SPI.INTFLAGS & SPI_IF_bm)); 
		return SPI.DATA;
	}	
	INLN void SPI_read(uint8_t *buff, uint8_t cnt)
	{
		do
		{
			*buff = spi(*buff);
			buff++;
			
		} while (--cnt > 0);
	}
	INLN void SPI_write(const uint8_t *buff, uint8_t cnt)
	{
		do
		{
			spi(*buff++);
			
		} while (--cnt > 0);
	}
	INLN bool waitRDY(uint16_t tmout, uint8_t status)
	{		
		CS_on();
		
		//PORTD.OUTSET = 1<< 1;
		
		for(uint8_t i = 0; i<tmout; i++)
		{
			//TB.CNT = 0;
			//TB.CCMP = 4000; //0.5ms
			//TB.CTRLA = TCB_ENABLE_bm;
			//while (!(TB.INTFLAGS & TCB_CAPT_bm)) ;			
			//TB.CTRLA = 0;
			_delay_us(500);

			spi(FLASH_RDSR);
			
			if (spi(0) == status)
			{				
				CS_off();				
				//PORTD.OUTCLR = 1<< 1;
				return true;
			}
		}		
		CS_off();		
		//PORTD.OUTCLR = 1<< 1;
		return false;
	}
	
	INLN bool startRead(uint8_t *addr)
	{
		init();		
		CS_on();
		
		spi(FLASH_READ);
		
		spi(((unio32_t*) addr)->B[3]);
		spi(((unio32_t*) addr)->B[2]);
		spi(((unio32_t*) addr)->B[1]);
		spi(((unio32_t*) addr)->B[0]);		
		
		return true;
	}
public:	
	INLN bool writePage(void)
	{
		init();
		
		// ожидаем c проверкой память (окончание предыдущей записи tPP 3ms max)
		if (waitRDY(PP_TIMAUT_MS(80), STATUS_READ_READY))
		{
			CS_on();
			
			spi(FLASH_WREN);
			
			CS_off();
			
			if (waitRDY(PP_TIMAUT_MS(80), STATUS_WRITE_READY))
			{
				CS_on();
				
				spi(FLASH_PP4B);
				
				spi(waddr.B[3]);
				spi(waddr.B[2]);
				spi(waddr.B[1]);
				spi(0);
				
				SPI_write(page, 0);
				
				CS_off();
				
				waddr.u32 += 0x100;
				
				deinit();
				
				return true;
			}
		}
		deinit();
		return false;
	}	
public:
	INLN ram_t(void)
	{
		//CHIP SELECT
	    SPI.CTRLB = 3;
		PCS.DIRSET = 1 << bitCS;
		CS_off();
		
		// spi pins
		// mux
		PORTMUX.SPIROUTEA &= ~(3 << (spiNo*2)); //clr mux
		PORTMUX.SPIROUTEA |= (pmuxAlt << (spiNo*2)); //set mux
		// MOSI MISO SCK
		uint8_t bit = getSPIbit();
		PORT_t* port = getSPIPort();
		volatile uint8_t *port_pin_ctrl = ((uint8_t *) port + 0x10 + bit+1);
		// MISO
		*port_pin_ctrl = PORT_ISC_INTDISABLE_gc | PORT_PULLUPEN_bm;
		// MOSI, SCK
		port->DIRSET = 5 << bit;

			SPI.CTRLB = 0
			//| 0 << SPI_BUFEN_bp   /* Buffer Mode Enable: disabled */
			//| 0 << SPI_BUFWR_bp /* Buffer Write Mode: disabled */
			//| SPI_MODE_3_gc     /* SPI Mode 3 */
			| 1 << SPI_SSD_bp;  /* Slave Select Disable: disabled */		
	}
	INLN void SetWritePage(uint8_t* page) {waddr.u32 = * (uint32_t*) page;}	
	INLN void ResetPageBase(void) {waddr.u32=0; pageCnt=0;}		
	INLN void read(uint8_t *addr, uint8_t *buf, const uint8_t cnt)
	{
		startRead(addr);
		
		SPI_read(buf, cnt);
		
		CS_off();
		
		deinit();
		
	}
	bool INLN Restore(uint16_t len, uint32_t* lastKadr)
	{
		uint32_t address = *lastKadr * len;
		uint32_t x;
		while (true) 
		{			
			read((uint8_t*)&address, (uint8_t*)&x, 4);
			
			if (x != *lastKadr+1) break;
			*lastKadr += 1;
			address += len;			
		}
		bool res = x == 0xFFFFFFFF;
		if (!res)
		{
			while (address < 0x02000000)
			{
				if (x == 0xFFFFFFFF) break;
				*lastKadr += 1;
				address += len;
			}
		} 		
		waddr.u32 = address;		
		pageCnt = waddr.B[0];
		return res;				
	}	
	
	INLN bool cleared(void)
	{	
		if (checkReadReady())
		{
			uint32_t x = 0;
			read((uint8_t*)&x,(uint8_t*)&x,4);
			return x == 0xFFFFFFFF;
		}
		else return false;	
	}
	INLN bool startClear(void)
	{
			init();
			
			CS_on();
			
			spi(FLASH_WREN);
			
			CS_off();
			
			uint8_t res = false;
			
			if (waitRDY(PP_TIMAUT_MS(1), STATUS_WRITE_READY))
			{
				CS_on();
				
				spi(FLASH_CHIP_ERASE);
				
				CS_off();
				
				res = true;
			}
			deinit();
			
			return res;
	}
	INLN bool checkReadReady(void)
	{
		init();
		//					0.1ms
		bool res = waitRDY(PP_TIMAUT_MS(1), STATUS_READ_READY);
		
		deinit();
		
		return res;
	}
	INLN void readAndSendUart(uint8_t* rd)
	{
		if (startRead(rd))
		{
			uint32_t n = ((ram_read_t*)rd)->len;
			uint16_t crc = 0xFFFF;
			
			if (!Clock.HiSpeedReady)
			{
				Clock.MAXInternalClock();
				//RUart.setBaud_MAXFSPU();
				MX25L256_Uart.setBaud_MAXFSPU();
			}
			
			//Clock.MAXInternalClock();
			//MX25L256_Uart.setBaud_MAXFSPU();
			MX25L256_Uart.noIntMode();
			MX25L256_Uart.disableRxD();
			MX25L256_Uart.enableTxD();			
			MX25L256_Uart.write(MX25L256_Uart.buf[0]);
			SPI.DATA = 0;
			crc = crc16next(crc, MX25L256_Uart.buf[0]);
			#ifdef BT2
			MX25L256_Uart.write(MX25L256_Uart.buf[1]);
			crc = crc16next(crc, MX25L256_Uart.buf[1]);
			#endif
			do{
				while (!(SPI.INTFLAGS & SPI_IF_bm)); 
				uint8_t d = SPI.DATA;
				SPI.DATA = 0;
				MX25L256_Uart.writeFast(d);
				crc = crc16next(crc, d);
				MX25L256_Uart.writeWait();				
			}while (--n > 0);
			MX25L256_Uart.write(crc);
			MX25L256_Uart.writeTxC2(crc >> 8);			
			MX25L256_Uart.disableTxD();
			MX25L256_Uart.intMode();
			MX25L256_Uart.enableRxD();
			if (!Clock.HiSpeedReady)
			{
				Clock.RestoreExternalClock();
			    MX25L256_Uart.resetBaud_MAXFSPU();
			}
			
			//Clock.RestoreExternalClock();
			//MX25L256_Uart.resetBaud_MAXFSPU();
		}
		CS_off();
		deinit();
	}

	INLN bool write(uint8_t *buf, uint8_t cnt)
	{
		bool res = true;
		if (pageCnt + cnt >= 0x100)
		{
			uint8_t pc = 0x100 - pageCnt;
			memcpy( &page[pageCnt], buf, pc);
			res = writePage();
			pageCnt = 0;
			cnt -= pc;
			buf += pc;					
		}
		memcpy( &page[pageCnt], buf,cnt);
		pageCnt += cnt;
			
		return res;
		
	}
};

#if !defined(MX25L256)
#error " Global define MX25L256=SPn,muxAlt,PORTCS,BITCS,uart " 
 // MX25L256=SP0,DEF0,PA,7,UART0
#endif

//extern ram_t<MX25L256> Ram;
