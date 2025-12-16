#pragma once

#include <avr/io.h>
#include <string.h>
#include <getarg.h>
#include <usarts.hpp>
#include <tools.h>


//#define UART0 0
//#define UART1 1
//#define UART2 2
//#define UART3 3
//#define UART4 4
//
//#define PA 0
//#define PB 1
//#define PC 2
//#define PD 3
//#define PE 4
//#define PF 5

#ifdef Uart
#undef Uart
#endif

#define  UART_N GETARG_1(GGTINY1616)
#if   UART_N == 0
#define Uart serial0
#elif UART_N == 1
#define Uart serial1
#elif UART_N == 2
#define Uart serial2
#elif UART_N == 3
#define Uart serial3
#elif UART_N == 4
#define Uart serial4
#endif
#define INLN __attribute__ ((__always_inline__)) inline

#define WAIT_GGTINY1616 40 // приблизительно 1400 ms

typedef void (*inclcallback_t)(uint8_t size);

class GGTiny1616_t
{
private:
	inclcallback_t cbf;
	uint8_t timout;

public:
	bool isOn;
	
	INLN void on(void)
	{
		isOn = true;
		Uart.setRS485mode();		
		Uart.setBaud(500);		
		Uart.intMode();
		Uart.enableRxD();
	}
	INLN void off(void)
	{
		cbf = 0;
		timout = 0;
		Uart.disableRxD();		
		isOn = false;
	}
	INLN bool ready(void)
	{
		return timout == 0;
	}
	INLN void dataReq(inclcallback_t callback)
	{
		Uart.disableRxD();
		cbf = callback;
		timout = WAIT_GGTINY1616;
		Uart.buf[0] = 3;
		Uart.startTxD(1);		
	}
	INLN void initDAC(uint16_t ch1, uint16_t ch2)
	{
		Uart.disableRxD();
		timout = WAIT_GGTINY1616;
		cbf = 0;
		Uart.buf[0] = 0x01;
		*(uint16_t*) &Uart.buf[1] = ch1;
		*(uint16_t*) &Uart.buf[3] = ch2;
		Uart.startTxD(5);
	}
	
	INLN void dataReqStd(uint8_t wait, uint8_t sz, uint8_t* data, inclcallback_t callback)
	{
		Uart.disableRxD();
		cbf = callback;
		timout = wait;
		memcpy(Uart.buf,data,sz);
		Uart.startTxD(sz);
	}	
	INLN bool isReadyRxD(void)
	{
		return Uart.isReadyRxD();
	}
    INLN void get_data(void* buff, uint8_t len)
	{
        memcpy(buff, Uart.buf, len);
	}	
	INLN void handler70ms(void)
	{
		if (timout > 0)
		{
			if (Uart.checkReadyRxD())
			{
				if (cbf) cbf(Uart.Count);
				cbf = 0;
				timout = 0;
			}
			else if (--timout == 0)
			{
				if (cbf) cbf(0);
				cbf = 0;
			}
		}
	}
	INLN uint8_t* buf(void)
	{
		return Uart.buf;
	}
};


//extern GGTiny1616_t<GETARG_23(GGTINY1616)> GGKP;

#if !defined(GGTINY1616)
# error "Global define GGTINY1616=UARTn,PwrPort,PwrPin"
// GGTINY1616=UART1
#endif

