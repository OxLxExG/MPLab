#pragma once

#include <avr/io.h>
#include <tools.h>
#include <protokol.h>
#include <usarts.hpp>

#if BK128 == UART0
#define ComBK serial0
#elif BK128 == UART1
#define ComBK serial1
#elif BK128 == UART2
#define ComBK serial2
#elif BK128 == UART3
#define ComBK serial3
#elif BK128 == UART4
#define ComBK serial4
#endif

#define WAIT_BK 40 // приблизительно 1400 ms


typedef void (*bk_callback_t)(void* buf,  uint8_t size);

class bk_t
{
private:
    uint8_t timout;
    bk_callback_t cb;
public: 
    
    INLN void on (void)
    {
        timout = 0;
		ComBK.setBaud(500);
		ComBK.setFullDuplexmode();
		ComBK.intMode();
		ComBK.enableRxD();        
    }
    
    INLN void off(void)
    {
		timout = 0;
		ComBK.buf[0] = 0xA6;
		ComBK.startTxD(1);
		while (!ComBK.readyTxD());
		ComBK.disableRxD();		
    }
    
	INLN bool Cmd(bk_callback_t callback)
	{
		if (timout > 0) return false;				
		cb = callback;		
		timout = WAIT_BK;
		ComBK.buf[0] = 0xA5;
		ComBK.startTxD(1);
		return true;
	}		
    
	INLN void handler32ms(void)
	{
		if (timout > 0)
		{
			if (ComBK.checkReadyRxD())
			{
				if (cb != NULL) cb(&ComBK.buf[0], ComBK.Count);
				cb = NULL;
				timout = 0;
			}
			else if (--timout == 0)
			{
				if (cb != NULL) cb(NULL,0);
				cb = 0;
			}
		}
	}
};

#if !defined(BK128)
# error "Global define BK128=UARTn"
// INDUC=UART3,PE,3
#endif


