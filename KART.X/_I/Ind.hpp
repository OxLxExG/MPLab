#pragma once

#if !defined(INDUC)
# error "Global define INDUC=UARTn,PwrPort,PwrPin"
// INDUC=UART3,PE,3
#endif

#include <avr/io.h>
#include <string.h>
#include <getarg.h>
#include <usarts.hpp>


#define UART0 0
#define UART1 1
#define UART2 2
#define UART3 3
#define UART4 4

#define PA 0
#define PB 1
#define PC 2
#define PD 3
#define PE 4
#define PF 5

#ifdef IndUart
#undef IndUart
#endif

#define  UART_N GETARG_1(INDUC)
#if   UART_N == 0
#define IndUart serial0
#elif UART_N == 1
#define IndUart serial1
#elif UART_N == 2
#define IndUart serial2
#elif UART_N == 3
#define IndUart serial3
#elif UART_N == 4
#define IndUart serial4
#endif

#define INLN __attribute__ ((__always_inline__)) inline

#define WAIT_IND 40 // приблизительно 1400 ms


typedef void (*induccallback_t)(buf_len_t size);

template <uint8_t uartN, uint8_t portPW, uint8_t bitPW>
class Ind_t
{	
	#define PPW (*(PORT_t*) (0x0400 + 0x20 * portPW))

private:
	induccallback_t cbf;
	uint8_t timout;

public:
	bool isOn;	
	INLN Ind_t(void)
	{
		PPW.DIRSET = 1 << bitPW;
		PPW.OUTCLR = 1 << bitPW;
	}
	INLN void on(void)
	{
		isOn = true;
		PPW.OUTSET = 1 << bitPW;
		//IndUart.setRS485mode();		
		IndUart.setBaud(125);
		IndUart.setRegBAUD(278);
		IndUart.setFullDuplexmode();
		IndUart.intMode();
		IndUart.enableRxD();
		timout = 0;//WAIT_IND;		
	}
	INLN void off(void)
	{
		cbf = 0;
		timout = 0;
//		IndUart.buf[0] = 0xA6;
//		IndUart.startTxD(1);
//		while (!IndUart.readyTxD());
		IndUart.disableRxD();		
		PPW.OUTCLR = 1 << bitPW;
		isOn = false;
	}
	INLN bool ready(void)
	{
		return timout == 0;
	}
	INLN void getData(induccallback_t callback)
	{
		IndUart.disableRxD();
		cbf = callback;
		timout = WAIT_IND;
		IndUart.buf[0] = 0x11;
		*(uint16_t*)(&IndUart.buf[1]) = 5;
		IndUart.CRCSend(3);
	}
	INLN void getMetr(induccallback_t callback)
	{
		IndUart.disableRxD();
		cbf = callback;
		timout = WAIT_IND;
		IndUart.buf[0] = 0x13;
		*(uint16_t*)(&IndUart.buf[1]) = 5;
		IndUart.CRCSend(3);
	}
	INLN void setMetr(uint8_t*buf,uint8_t cnt,  induccallback_t callback)
	{
		IndUart.disableRxD();
		cbf = callback;
		timout = WAIT_IND;
		IndUart.buf[0] = 0x12;
		*(uint16_t*)(&IndUart.buf[1]) = cnt+3+2;		
		memcpy(&IndUart.buf[3],buf,cnt);
		IndUart.CRCSend(cnt+3);
	}
	INLN void dataReqStd(uint8_t wait, uint8_t sz, uint8_t* data, induccallback_t callback)
	{
		IndUart.disableRxD();
		cbf = callback;
		timout = wait;
		memcpy(IndUart.buf,data,sz);
		IndUart.startTxD(sz);
	}	
	INLN bool isReadyRxD(void)
	{
		return IndUart.isReadyRxD();
	}
	
	INLN void handler70ms(void)
	{
		if (timout > 0)
		{
			if (IndUart.checkReadyRxD())
			{
				if (cbf) cbf(IndUart.Count);
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
		return IndUart.buf;
	}
};
