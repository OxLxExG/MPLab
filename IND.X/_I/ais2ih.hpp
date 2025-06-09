#pragma  once

#include <avr/io.h>
#include <util/delay.h>
#include <getarg.h>
#include <protokol.h>
#include <clock.hpp>
#include <tools.h>
#include <Indicator.hpp>
 
//								           default								
#define AIS_OUT_T_L			0x0D // R 00001101 00000000 Temp sensor output
#define AIS_OUT_T_H			0x0E // R 00001110 00000000
#define AIS_WHO_AM_I		0x0F // R 00001111 01000100 Who am I ID
// RESERVED - 10-1F - RESERVED
#define AIS_CTRL1			0x20 //R/W 00100000 00000000 Control registers
typedef struct
{
	uint8_t lpmode: 2;
	uint8_t mode: 2;
	uint8_t odr: 4;	
} ctrl1_t;
#define AIS_MODE_HI	1
enum 
{
	ODR_12HZ=2,
	ODR_25HZ=3,
	ODR_50HZ=4,
	ODR_100HZ=5,
	ODR_200HZ=6,		
};

#define AIS_CTRL2			0x21 //R/W 00100001 00000100
#define CTRL2_BOOT			(1<<7)
#define CTRL2_RESET			(1<<6)
//#define CTRL2_BOOT (1<<5)
#define CTRL2_CS_PU_DISC	(1<<4)
#define CTRL2_BDU			(1<<3)
#define CTRL2_IF_ADD_INC	(1<<2)
#define CTRL2_I2C_DISABLE	(1<<1)
#define CTRL2_SIM			(1<<0)

#define AIS_CTRL3			0x22 //R/W 00100010 00000000
#define AIS_CTRL4_INT1_PAD_CTRL 0x23 //R/W 00100011 00000000
#define AIS_CTRL5_INT2_PAD_CTRL 0x24 //R/W 00100100 00000000

#define AIS_CTRL6			0x25 //R/W 00100101 00000000
typedef struct
{
	uint8_t reserv: 2;
	uint8_t lowNoise: 1;
	uint8_t fds: 1;
	uint8_t fs: 2;
	uint8_t bw: 2;
} ctrl6_t;
#define AIS_MODE_HI	1
enum
{
	BW_ODR_DIV_2=0,
	BW_ODR_DIV_4=1,
	BW_ODR_DIV_10=2,
	BW_ODR_DIV_20=3,
};
enum
{
	FS_2g=0,
	FS_4g=1,
	FS_8g=2,
	FS_16g=3,
};

#define AIS_OUT_T			0x26 //R 00100110 00000000 Temp sensor output
#define AIS_STATUS			0x27 //R 00100111 00000000 Status data register
// Output registers
#define AIS_OUT_X_L			0x28 // R00101000 00000000
#define AIS_OUT_X_H			0x29 //R 00101001 00000000
#define AIS_OUT_Y_L			0x2A //R 00101010 00000000
#define AIS_OUT_Y_H			0x2B //R 00101011 00000000
#define AIS_OUT_Z_L			0x2C //R 00101100 00000000
#define AIS_OUT_Z_H			0x2D //R 00101101 00000000

#define AIS_FIFO_CTRL		0x2E   //R/W 00101110 00000000 FIFO control register
#define AIS_FIFO_SAMPLES	0x2F   //R   00101111 00000000 Unread samples stored in FIFO
#define AIS_TAP_THS_X		0x30   //R/W 00110000 00000000
#define AIS_TAP_THS_Y		0x31   //R/W 00110000 00000000
#define AIS_TAP_THS_Z		0x32   //R/W 00110010 00000000
#define AIS_INT_DUR			0x33   //R/W 00110011 00000000 Interrupt duration
#define AIS_WAKE_UP_THS		0x34   //R/W 00110100 00000000
// Tap/double-tap selection,
// inactivity enable,
// wakeup threshold
#define AIS_WAKE_UP_DUR		0x35 //R/W 00110101 00000000 Wakeup duration
#define AIS_FREE_FALL		0x36 //R/W 00110110 00000000 Free-fall configuration
#define AIS_STATUS_DUP		0x37 //R   00110111 00000000 Status register
	#define FIFO_THS 0x80
	#define DRDY 0x01
#define AIS_WAKE_UP_SRC		0x38 //R   00111000 00000000 Wakeup source
#define AIS_TAP_SRC			0x39 //R   00111001 00000000 Tap source
#define AIS_SIXD_SRC		0x3A //R   00111010 00000000 6D source						
#define AIS_ALL_INT_SRC		0x3B //R   00111011 00000000
#define AIS_X_OFS_USR		0x3C //R/W 00111100 00000000
#define AIS_Y_OFS_USR		0x3D //R/W 00111110 00000000
#define AIS_Z_OFS_USR		0x3E //R/W 00000100 00000000
#define AIS_CTRL7			0x3F //R/W 00000100 00000000*/
#ifdef SPI
#undef  SPI
#endif // SPI


template <uint8_t spiNo, uint8_t pmuxAlt, uint8_t portCS, uint8_t bitCS>
class ais2ih_t
{
	#define SPI (*(SPI_t*) (0x0940 + 0x20 * spiNo))
	#define PCS (*(PORT_t*) (0x0400 + 0x20 * portCS))
private:
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
	INLN void CS_on(void)
	{
		PCS.OUTCLR = 1 << bitCS;
	}
	INLN void CS_off(void)
	{
		PCS.OUTSET = 1 << bitCS;
	}
	INLN void init(void)
	{
		//SPI.CTRLB = 
		  //SPI_SSD_bm   // SPI Select Disable bit mask.  
		//| 3; // mode 3
		
		SPI.CTRLA = 
		  1 << SPI_CLK2X_bp     // Enable Double Speed: enabled
		| 0 << SPI_DORD_bp   // Data Order Setting: disabled
		| 1 << SPI_ENABLE_bp // Enable Module: enabled
		| 1 << SPI_MASTER_bp // SPI module in master mode
		| SPI_PRESC_DIV4_gc; // System Clock / 4  // 2*2000000 = 4MHz		
		SPI.INTFLAGS;
	}
	INLN void deinit(void){SPI.CTRLA = 0; SPI.INTFLAGS;}
	INLN uint8_t spi(const uint8_t d)
	{
		SPI.DATA = d;
		while (!(SPI.INTFLAGS & SPI_IF_bm));
		return SPI.DATA;
	}
	void SPI_read(uint8_t *buff, uint16_t cnt)
	{
		do
		{
			*buff = spi(*buff);
			buff++;
			
		} while (--cnt > 0);
	}
	void SPI_write(const uint8_t *buff, uint16_t cnt)
	{
		do
		{
			spi(*buff++);
			
		} while (--cnt > 0);
	}
	void WriteAdr(uint8_t adr, uint8_t data)
	{
		CS_on();
		spi(adr);
		spi(data);
		CS_off();
	}
	uint8_t ReadAdr(uint8_t adr)
	{
		CS_on();
		spi(0x80|adr);
		uint8_t r = spi(0);
		CS_off();
		return r;
	}
public:
	ais2ih_t(void)
	{
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
		| 0 << SPI_BUFEN_bp   // Buffer Mode Enable: disabled
		| 0 << SPI_BUFWR_bp // Buffer Write Mode: disabled
		| SPI_MODE_3_gc     // SPI Mode 3
		| 1 << SPI_SSD_bp;  // Slave Select Disable: disabled		
	}
	INLN uint8_t WhoAmI(void)	{		init();		CS_on();		spi(0x80 | AIS_WHO_AM_I);		uint8_t res = spi(0xAA);		CS_off();		deinit();		return res;	}
	INLN bool InitAccel(void)	{		init();		uint8_t res = ReadAdr(AIS_WHO_AM_I);		ctrl1_t c1 ={.lpmode=0,.mode=AIS_MODE_HI,.odr=ODR_12HZ};		WriteAdr(AIS_CTRL1, *(uint8_t*) &c1);		WriteAdr(AIS_CTRL2, CTRL2_CS_PU_DISC|CTRL2_BDU|CTRL2_IF_ADD_INC|CTRL2_I2C_DISABLE);		ctrl6_t c6 = {.reserv=0,.lowNoise=1,.fds=0,.fs=FS_16g,.bw=BW_ODR_DIV_20};		WriteAdr(AIS_CTRL6, *(uint8_t*) &c6);		deinit();		return res == 0x44;	}	INLN int8_t ReadT(void)	{		init();		int8_t r = ReadAdr(AIS_OUT_T);		deinit();		return 25+r;	}

	INLN void ReadXYZ(void* xyz)	{		init();		CS_on();		spi(0x80|AIS_OUT_X_L);		SPI_read((uint8_t*)xyz, 2*3);		CS_off();		deinit();	}	
};

extern ais2ih_t<AIS2IH> Ais2ih;