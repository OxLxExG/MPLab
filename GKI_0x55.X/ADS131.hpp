#ifndef _ADS131_HPP_
#define _ADS131_HPP_

#include <avr/io.h>
#include <util/delay.h>
#include <usarts.hpp>
#include <getarg.h>
#include <tools.h>
#include "ADS131_user_data_ready.hpp"

#if !defined(ADS131)
# error "Global define ADS131=uart_spi_mode,CSport,CSpin,RDYport,RDYpin,SYNCport,SYNCpin"
// UART3,PB,4,PB,3,PB,5
#endif


#define ADS131_INIT_STD  ADS131_udr ads131_udr; ads131_t<ADS131> ads131;

#define ADS131_RDY GETARG_4(ADS131)
#if   ADS131_RDY == PA
#define ADS131_INIT ADS131_INIT_STD ISR(PORTA_PORT_vect) {ads131._DataReady();}
#elif ADS131_RDY == PB
#define ADS131_INIT ADS131_INIT_STD ISR(PORTB_PORT_vect) {ads131._DataReady();}
#elif ADS131_RDY == PC
#define ADS131_INIT ADS131_INIT_STD ISR(PORTC_PORT_vect) {ads131._DataReady();}
#elif ADS131_RDY == PD
#define ADS131_INIT ADS131_INIT_STD ISR(PORTD_PORT_vect) {ads131._DataReady();}
#elif ADS131_RDY == PE
#define ADS131_INIT ADS131_INIT_STD ISR(PORTE_PORT_vect) {ads131._DataReady();}
#elif ADS131_RDY == PF
#define ADS131_INIT ADS131_INIT_STD ISR(PORTF_PORT_vect) {ads131._DataReady();}
#endif


#ifdef Uart
#undef Uart
#endif

#define  ADS131_UART_N GETARG_1(ADS131)
#if   ADS131_UART_N == 0
#define Uart serial0
#elif ADS131_UART_N == 1
//EXTERN_USART(1)
#define Uart serial1
#elif ADS131_UART_N == 2
#define Uart serial2
#elif ADS131_UART_N == 3
#define Uart serial3
#elif ADS131_UART_N == 4
#define Uart serial4
#endif
//     GPIO0
// GPIO.7- 2.097152 interval 2c
// GPIO.6 - 64 times per 2c HTC
// GPIO.6 - 69 times per 2c RTC
// 0..4 - uart
// 5 будем испрользовать 

// команда записи чтения 
#define PRE_RREG 0b101
#define PRE_WREG 0b011
struct rwreg
{
	uint16_t cnt: 7;
	uint16_t adr: 6;
	uint16_t preambula: 3;
};
union rwreg_u
{
	struct rwreg	wr;
	uint8_t bt[2];
};
///
struct cfgreg
{
	uint16_t cd_en: 1;
	uint16_t cd_len: 3;
	uint16_t cd_num: 3;
	uint16_t cd_allch: 1;
	uint16_t gc_en: 1;
	uint16_t gc_delay: 4; 	
};
union cfgreg_u
{
	struct cfgreg	cfg;
	uint8_t bt[2];
};
///
struct clockreg
{
	uint16_t pwr: 2;
	uint16_t osr: 3;	
	uint16_t rezerv1: 1;
	uint16_t extref_en: 1;
	uint16_t xtal_dis: 1;
	uint16_t ch0_en: 1;
	uint16_t ch1_en: 1;
	uint16_t ch2_en: 1;
	uint16_t ch3_en: 1;
	uint16_t ch4_en: 1;
	uint16_t ch5_en: 1;
	uint16_t ch6_en: 1;
	uint16_t ch7_en: 1;
};
union clockreg_u
{
	struct clockreg	clk;
	uint8_t bt[2];
};
///
struct gain1reg
{
	uint16_t pgagain0: 3;
	uint16_t rezerv0: 1;
	uint16_t pgagain1: 3;
	uint16_t rezerv1: 1;
	uint16_t pgagain2: 3;
	uint16_t rezerv2: 1;
	uint16_t pgagain3: 3;
	uint16_t rezerv3: 1;
};
struct gain2reg
{
	uint16_t pgagain4: 3;
	uint16_t rezerv4: 1;
	uint16_t pgagain5: 3;
	uint16_t rezerv5: 1;
	uint16_t pgagain6: 3;
	uint16_t rezerv6: 1;
	uint16_t pgagain7: 3;
	uint16_t rezerv7: 1;
};
union gain1reg_u
{
	struct gain1reg	gain;
	uint8_t bt[2];
};
union gain2reg_u
{
	struct gain2reg	gain;
	uint8_t bt[2];
};

typedef void (*adscallback_t)(uint8_t usercmd);

template <uint8_t serialNo, uint8_t portCS, uint8_t bitCS, uint8_t portRDY, uint8_t bitRDY, uint8_t portSYNC, uint8_t bitSYNC>
class ads131_t
{
	#define URT (*(USART_t*) (0x0800 + 0x20 * serialNo))	
	#define PCS (*(PORT_t*) (0x0400 + 0x20 * portCS))
	#define PRDY (*(PORT_t*) (0x0400 + 0x20 * portRDY))
	#define PSNC (*(PORT_t*) (0x0400 + 0x20 * portSYNC))
private:
	uint8_t UserCmd;
	adscallback_t cbf;
	INLN void DataReadyIntEnable(void)
	{
		register8_t* rdy = &PRDY.PIN0CTRL;
		rdy += bitRDY;
		*rdy = PORT_ISC_FALLING_gc;
	}
	INLN void DataReadyIntDesable(void)
	{
		register8_t* rdy = &PRDY.PIN0CTRL;
		rdy += bitRDY;
		*rdy = PORT_ISC_INTDISABLE_gc;
	}
	INLN int32_t _convert(uint8_t* ptr)
	{
		unio32_t res;
		res.B[2] = *ptr++;
		res.B[1] = *ptr++;
		res.B[0] = *ptr++;
		res.B[3] = (res.B[2] & 0x80)? 0xFF:0;
		return res.u32;
	}
public:	
	int32_t data[8];
	uint8_t IsOn;
	
	INLN ads131_t(void)
	{
		PCS.DIRSET = 1<< bitCS;
		CS_off();
		PSNC.DIRSET = 1<< bitSYNC;
		SNC_off();
	}
    INLN USART_t& urt(void){return (*(USART_t*) (0x0800 + 0x20 * serialNo));}
		
	INLN void Reset(void)
	{
		// reset
		SNC_on();
		_delay_ms(1);
		SNC_off();
		IsOn = 1;		
	}
	
	/// Reset, setup Uart, Enable DataReady Interrupt, wait Ready
	INLN void Init(void)
	{
		DataReadyIntDesable();
		Reset();
		//SPI settings are CPOL = 0 and CPHA = 1
		URT.STATUS = USART_RXSIE_bm | USART_RXCIE_bm | USART_TXCIE_bm;
		URT.CTRLC = USART_CMODE_MSPI_gc | USART_UCPHA_bm;// | USART_UDORD_bm ;
		Uart.setBaud((F_CPU/1e3)/2);
		while(!(PRDY.IN | (1 << bitRDY)));
		DataReadyIntEnable();
	}
	// выполняется вконце фрейма (по даташиту)
	INLN void AddSyncFrameStandBy(void)
	{
		cmdStandBy = 1;
	}
	// выполняется сразу асинхронно  !!!
	INLN void WakeUp(void)
	{
		Uart.buf[0] = 0;
		Uart.buf[1] = 0x33;
		Uart.buf[2] = 0;
		CS_on();
		Uart.SpiTransaction(3);
		Uart.buf[1] = 0;
		CS_off();
		IsOn = 1;
	}
	// выполняется синхронно с данными фрейма (по даташиту)	
	// WakeUp() нельзя использовать сразу после AddSyncFrameUserCmd
	// callback: использование - асинхронное чтение регистров после фрейма
	INLN void AddSyncFrameUserCmd(uint8_t cmd, adscallback_t callback)
	{
		UserCmd = cmd;
		cbf = callback;
	}	
	INLN void Transaction(uint8_t n)
	{
			CS_on();
			Uart.SpiTransaction(n*3);
			CS_off();
	}
	
	// прерывание (начало фрейма)
	INLN void _DataReady(void)
	{
		ads131_udr.UserDataReady();
		PRDY.INTFLAGS = 0xFF;
		// (флаг фрейма)
		GPR.GPR0 |= 0x20;
	}
	// основной цикл (начало фрейма)
	INLN bool checkDataReady(void)
	{
		if (GPR.GPR0 & 0x20)
		{
			GPR.GPR0 &= ~0x20;
			return true;
		}
		else return false;
	}
	
	
	INLN void DataReadyHandler(void)
	{			
		Transaction(10);
		
		uint8_t* ptr = Uart.SpiData();
		ptr += 3;
		for (uint8_t i = 0; i<8; i++)
		{
			data[i] = _convert(ptr);
			
			ptr += 3;
		}
		if (UserCmd)
		{
			uint8_t s = UserCmd;
			UserCmd = 0;
			if (cbf)
			{
				cbf(s);
				cbf =0;
			}
			Uart.buf[0] = 0;
			Uart.buf[1] = 0;			
		}
		if (cmdStandBy)
		{
			cmdStandBy = 0;
			Uart.buf[0] = 0;
			Uart.buf[1] = 0x22;
			Transaction(1);
			Uart.buf[1] = 0;
			IsOn = 0;
		}
	}
	
    INLN void CS_on(void){PCS.OUTCLR = 1 << bitCS;};
    INLN void CS_off(void){PCS.OUTSET = 1 << bitCS;};
private:
	uint8_t cmdStandBy;
    INLN void SNC_on(void){PSNC.OUTCLR = 1 << bitSYNC;};
    INLN void SNC_off(void){PSNC.OUTSET = 1 << bitSYNC;};
};

extern ads131_t<ADS131> ads131;

#endif
