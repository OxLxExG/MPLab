#pragma once

#include <avr/io.h>
#include <string.h>
#include <getarg.h>
#include <usarts.hpp>
#include <tools.h>

#ifdef CartUart
#undef CartUart
#endif

#define  UART_N GETARG_1(CART)
#if   UART_N == 0
#define CartUart serial0
#elif UART_N == 1
#define CartUart serial1
#elif UART_N == 2
#define CartUart serial2
#elif UART_N == 3
#define CartUart serial3
#elif UART_N == 4
#define CartUart serial4
#endif

#define WAIT_CART 40 // приблизительно 1400 ms


typedef void (*Cartcallback_t)(buf_len_t size);

class Cart_c
{	
private:
	Cartcallback_t cbf;
	uint8_t timout;

public:
	INLN void on(void)
	{
		CartUart.setBaud(125);
//		CartUart.setRegBAUD(278);
		CartUart.setFullDuplexmode();
		CartUart.intMode();
		CartUart.enableRxD();
		timout = 0;//WAIT_IND;		
	}
	INLN void off(void)
	{
		cbf = 0;
		timout = 0;
		CartUart.disableRxD();		
	}
	INLN bool ready(void)
	{
		return timout == 0;
	}
	INLN void DataReq(Cartcallback_t callback)
	{
		CartUart.disableRxD();
		cbf = callback;
		timout = WAIT_CART;
		CartUart.buf[0] = 0x11;
		*(uint16_t*)(&CartUart.buf[1]) = 5;
		CartUart.CRCSend(3);
	}
    INLN void get_data(void* buff, uint16_t len)
	{
        memcpy(buff, &CartUart.buf[3], len);
	}	

//	INLN void getMetr(induccallback_t callback)
//	{
//		IndUart.disableRxD();
//		cbf = callback;
//		timout = WAIT_IND;
//		IndUart.buf[0] = 0x13;
//		*(uint16_t*)(&IndUart.buf[1]) = 5;
//		IndUart.CRCSend(3);
//	}
//	INLN void setMetr(uint8_t*buf,uint8_t cnt,  induccallback_t callback)
//	{
//		IndUart.disableRxD();
//		cbf = callback;
//		timout = WAIT_IND;
//		IndUart.buf[0] = 0x12;
//		*(uint16_t*)(&IndUart.buf[1]) = cnt+3+2;		
//		memcpy(&IndUart.buf[3],buf,cnt);
//		IndUart.CRCSend(cnt+3);
//	}
	INLN void dataReqStd(uint8_t wait, uint16_t sz, uint8_t* data, Cartcallback_t callback)
	{
		CartUart.disableRxD();
		cbf = callback;
		timout = wait;
		memcpy(CartUart.buf,data,sz);
		CartUart.startTxD(sz);
	}	
	INLN bool isReadyRxD(void)
	{
		return CartUart.isReadyRxD();
	}
	
	INLN void handler70ms(void)
	{
		if (timout > 0)
		{
			if (CartUart.checkReadyRxD())
			{
				if (cbf) cbf(CartUart.Count);
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
		return CartUart.buf;
	}
};

#if !defined(CART)
# error "Global define CART=UARTn"
// INDUC=UART3
#endif
