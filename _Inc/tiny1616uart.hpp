/* 
 * File:   tiny1616uart.h
 * Author: User
 *
 * Created on 20 ноября 2025 г., 14:19
 */

#ifndef TINY1616UART_H
#define	TINY1616UART_H

#include <avr/io.h>

#define UART_BAUD 500000

template <uint8_t pmuxAlt> 
class usart_pol_t 
{
public:
    uint8_t Count;
    uint8_t buf[255];

    INLN usart_pol_t(void)
    {
        if (pmuxAlt)
        {
            PORTMUX.CTRLB = 1;
            PORTA.PIN1CTRL = PORT_ISC_INTDISABLE_gc | PORT_PULLUPEN_bm;
        }
        else
        {
            PORTB.PIN3CTRL = PORT_ISC_INTDISABLE_gc | PORT_PULLUPEN_bm;
        }
        USART0.BAUD = 4* F_CPU / UART_BAUD;
        USART0.CTRLA = USART_LBME_bm;
        USART0.CTRLB = USART_RXEN_bm | USART_ODME_bm;
                
    }
    void SendPool(uint8_t len)
    {
        USART0.CTRLB = USART_TXEN_bm | USART_ODME_bm;
        for (uint8_t i = 0; i < len; i++)
        {
            USART0.STATUS = USART_TXCIF_bm;
            USART0.TXDATAL = buf[i];
            while (!(USART0.STATUS & USART_TXCIF_bm));
        }
        USART0.CTRLB = USART_RXEN_bm | USART_ODME_bm;
    }
    void readPool(void)
    {
        Count = 0;
        while (1)
        {
            uint8_t Delay_3_5symb = F_CPU / (UART_BAUD/35)/6; //6-команд выполняется цикл ниже при Os
            
 //         90 91 04 08 	lds	r25, 0x0804	
 //     	97 fd       	sbrc	r25, 7
 //         0d c0       	rjmp	.+26    
 //         81 50       	subi	r24, 0x01
 //         d1 f7       	brne	.-12     
            
            while ((!(USART0.STATUS & USART_RXCIF_bm)) && --Delay_3_5symb);
            
            if (Delay_3_5symb == 0) 
            {
                break;
            }

            uint8_t data = USART0.RXDATAL;

            buf[Count++] = data;

            if (Count >= sizeof(buf)) 
            {
                break;
            }
        }
    }    
};

#endif	/* TINY1616UART_H */

