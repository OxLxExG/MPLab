/*
 * USARTS.h
 *
 * Created: 28.10.2021 13:32:29
 *  Author: oleg
 * однопроводной uart async 8 bit 1 stop P none
 * RS485 - опционально
  GPR0 - 5 бит флаги uart0..4
 */ 

//#pragma once

#ifndef USARTS_HPP_
#define USARTS_HPP_

#include <avr/io.h>
#include <crc.h>
#include <avr/interrupt.h>
#include <tools.h>
#include <string.h>
#include <getarg.h>
#include <protokol.h>


#ifdef USEUSART0
	#define BUF_LEN GETARG_2(USEUSART0)
	#if BUF_LEN > 255
		#define LONG_BUF
	#endif	
	#undef BUF_LEN
#endif
#ifndef LONG_BUF
	#ifdef USEUSART1
		#define BUF_LEN GETARG_2(USEUSART1)
		#if (BUF_LEN > 255)
			#define LONG_BUF
		#endif	
		#undef BUF_LEN
	#endif
#endif
#ifndef LONG_BUF
	#ifdef USEUSART2
		#define BUF_LEN GETARG_2(USEUSART2)
		#if (BUF_LEN > 255)
			#define LONG_BUF
		#endif	
		#undef BUF_LEN
	#endif
#endif
#ifndef LONG_BUF
	#ifdef USEUSART3
		#define BUF_LEN GETARG_2(USEUSART3)
		#if (BUF_LEN > 255)
			#define LONG_BUF
		#endif	
		#undef BUF_LEN
	#endif
#endif
#ifndef LONG_BUF
	#ifdef USEUSART4
		#define BUF_LEN GETARG_2(USEUSART4)
		#if (BUF_LEN > 255)
			#define LONG_BUF
		#endif	
		#undef BUF_LEN
	#endif
#endif

#ifdef LONG_BUF
	typedef uint16_t buf_len_t; 
	#define CRC(b,l) crc16Long(b,l)
#else
	typedef uint8_t buf_len_t;
	#define CRC(b,l) crc16(b,l)
#endif

#define B125000 125
#define B500000 500
#define B1000000 1000

#define USART_RS485_bm  0x01  /* RS485 Mode internal transmitter bit mask. */

// Сообщения разделяются по паузе в линии как в Modbus RTU. 
// Сообщение должно начинаться и заканчиваться интервалом тишины, длительностью не менее 3,5 символов при данной скорости передачи. 
// Во время передачи сообщения не должно быть пауз длительностью более 1,5 символа. 
// Для скоростей 500000 и более бод интервал тишины 128 мкс. 
// Проверка целостности осуществляется с помощью контрольной суммы CRC (длиной в два байта) как в Modbus RTU.

#ifdef UART
#undef  UART
#endif // UART
template <uint8_t usartNo, uint8_t pmuxAlt, buf_len_t buffLen, uint8_t timerNo> 
class usart_t 
{
	#define UART (*(USART_t*) (0x0800 + 0x20 * usartNo))
	#define PINS (pmuxAlt*4)
	#define TB (*(TCB_t*) (0x0B00 + 0x10 * timerNo)) 
	
public:
//    USART_t& UART()
   //bool tx;
	INLN PORT_t* getUsartPort(void)
	{
		switch (usartNo)
		{
			case 0:
			return &PORTA;
			case 1:
			return &PORTC;
			case 2:
			return &PORTF;
			case 3:
			return &PORTB;
			case 4:
			return &PORTE;
		}
	}
	
	INLN void lock(void)
	{
		GPR.GPR1 |= (1 << usartNo);
	}
	INLN void unlock(void)
	{
		GPR.GPR1 &= ~(1 << usartNo);
	}
	INLN void setReadyRxD(void)
	{
		GPR.GPR0 |= (1 << usartNo);
	}
	INLN void clrReadyRxD(void)
	{
		GPR.GPR0 &= ~(1 << usartNo);
	}
	INLN register8_t* getPinCtl(void)
	{
		PORT_t* p = getUsartPort();
		register8_t* ctrlpin = &p->PIN0CTRL;
		ctrlpin += PINS;
		return ctrlpin;
	}
	INLN void startTimout(void)
	{
		TB.CNT = 0x0; /* Count: 0x0 */
		TB.CTRLA = TCB_CLKSEL_DIV1_gc     /* CLK_PER */
		| 1 << TCB_ENABLE_bp   /* Enable: enabled */
		| 1 << TCB_RUNSTDBY_bp /* Run Standby: enabled */
		| 0 << TCB_SYNCUPD_bp  /* Synchronize Update: disabled */
		| 0 << TCB_CASCADE_bp; /* Cascade Two Timer/Counters: disabled */
	}
	INLN void waitSF(void)
	{
		UART.CTRLB |= USART_SFDEN_bm;
	}
	INLN void nowaitSF(void)
	{
		UART.CTRLB &= ~USART_SFDEN_bm;
	}
	INLN uint8_t read(void)
	{
		while (!(UART.STATUS & USART_RXCIF_bm));
		return UART.RXDATAL;
	}
		
public:	
	uint8_t buf[buffLen];
	buf_len_t cnt;
	buf_len_t Count;
	
	INLN USART_t& Uart(void){return UART;}
		
    INLN uint8_t* SpiData(void)
	{
		return &buf[buffLen / 2];
	}
	// data: burr[cnt] ready input data: @buf[buffLen / 2][cnt]  
	INLN void SpiTransaction(uint8_t cnt)
	{
		uint8_t* ptr = SpiData(); 
		UART.TXDATAL = buf[0];
		for(buf_len_t i = 1; i < cnt; i++)
		{
			while (!(UART.STATUS & USART_DREIF_bm));
			UART.TXDATAL = buf[i];
			if (UART.STATUS & USART_RXCIF_bm) *ptr++ = UART.RXDATAL;
		}
		if (UART.STATUS & USART_RXCIF_bm) *ptr++ = UART.RXDATAL;
		if (UART.STATUS & USART_RXCIF_bm) *ptr++ = UART.RXDATAL;
		if (UART.STATUS & USART_RXCIF_bm) *ptr++ = UART.RXDATAL;
	}
	INLN void enableRxD(void)
	{
		#ifndef NOINT_UART
		UART.CTRLB |= USART_RXEN_bm | USART_SFDEN_bm;
		#else
		UART.CTRLB |= USART_RXEN_bm;
		#endif
	}
	INLN void disableRxD(void)
	{
		UART.CTRLB &= ~(USART_RXEN_bm | USART_SFDEN_bm);	
		unlock();
		//stop timer
		TB.CTRLA = 0;
		TB.INTFLAGS = TCB_CAPT_bm;
	}
	INLN void enableTxD(void)
	{
		if (!(UART.CTRLA & USART_LBME_bm))
		{
			PORT_t* p = getUsartPort();
			p->DIRSET = 1 << PINS;
		}
		UART.CTRLB |= USART_TXEN_bm;
		//if (UART.CTRLA & USART_RS485_bm)
		//{
			//p->DIRCLR = 1 << PINS;			
		//}
		//else 
		//{
			//p->DIRSET = 1 << PINS;			
		//}
	}
	INLN void disableTxD(void)
	{
		UART.CTRLB &= ~USART_TXEN_bm;
		if (!(UART.CTRLA & USART_LBME_bm))
		{
			PORT_t* p = getUsartPort();
			p->DIRCLR = 1 << PINS;
		}		
	}	
	INLN bool readyTxD(void)
	{
	    return	!(UART.CTRLB & USART_TXEN_bm);
	}
	INLN bool isReadyRxD(void)
	{
		return GPR.GPR0 & (1 << usartNo);
	}
	
	INLN bool checkReadyRxD(void)
	{
		
		if (GPR.GPR0 & (1 << usartNo))
		{
			clrReadyRxD();
			return true;
		}
		else return false;
	}
	
	
	INLN usart_t(void)
	{   
		// port mux
		register8_t* mux = (usartNo < 4) ? &PORTMUX.USARTROUTEA : &PORTMUX.USARTROUTEB;
		uint8_t alt = 1 << ((usartNo % 4)*2);				
		if (pmuxAlt) *mux |= alt;
		else *mux &= ~alt;
		
		if(timerNo != SPI_MODE)
		{
			register8_t* ctrlpin = getPinCtl();
			*ctrlpin = PORT_ISC_INTDISABLE_gc | PORT_PULLUPEN_bm;
			ctrlpin++;
			*ctrlpin = PORT_ISC_INTDISABLE_gc | PORT_PULLUPEN_bm;

			UART.CTRLA = USART_LBME_bm;
			// async 8 bit 1 stop P none
			//UART.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_CHSIZE_8BIT_gc | USART_PMODE_DISABLED_gc | USART_SBMODE_1BIT_gc;
			#ifndef NOINT_UART
			UART.CTRLB = USART_ODME_bm | USART_SFDEN_bm;
			TB.INTCTRL = TCB_CAPT_bm;
			#else
			UART.CTRLB = USART_ODME_bm;
			#endif
			TB.CNT = 0;
		}
		else
		{
			PORT_t* p = getUsartPort();			
			p->DIRSET = (1 << PINS)|(1 << (PINS+2));
			p->OUTSET = 1 << PINS;
			register8_t* ctrlpin = getPinCtl();
			ctrlpin++;
			*ctrlpin = PORT_ISC_INTDISABLE_gc;
			ctrlpin++;
			*ctrlpin = PORT_ISC_INTDISABLE_gc;
			
			UART.CTRLB = 0 << USART_MPCM_bp       /* Multi-processor Communication Mode: disabled */
			| 0 << USART_ODME_bp     /* Open Drain Mode Enable: disabled */
			| 1 << USART_RXEN_bp     /* Receiver Enable: enabled */
			| USART_RXMODE_NORMAL_gc /* Normal mode */
			| 0 << USART_SFDEN_bp    /* Start Frame Detection Enable: disabled */
			| 1 << USART_TXEN_bp;    /* Transmitter Enable: enabled */

			UART.CTRLC = USART_CMODE_MSPI_gc; /* Synchronous Mode */
			//| USART_CHSIZE_8BIT_gc     /* Character size: 8 bit */
			//| USART_PMODE_DISABLED_gc  /* No Parity */
			//| USART_SBMODE_1BIT_gc;    /* 1 stop bit */
			
		}
		
	}
	INLN void errRS485DirReset(void)
	{
		PORT_t* p = getUsartPort();
		p->OUTCLR = 1 << (PINS + 3);		
	}
	INLN void setFullDuplexmode(void)
	{
		UART.CTRLA &= ~USART_LBME_bm;
		UART.CTRLB &= ~USART_ODME_bm;
		PORT_t* p = getUsartPort();
		p->DIRSET = 1 << PINS;
		p->OUTSET = 1 << PINS;		
	}
	INLN void setRS485mode(void)
	{
		UART.CTRLA |= USART_RS485_bm;
		PORT_t* p = getUsartPort();
		p->DIRSET = 1 << (PINS + 3);
	}
	INLN bool setBaud_MAXFSPU(void)
	{
		UART.BAUD = UART.BAUD * (24000000/F_CPU);
		return true;
	}
	INLN uint16_t getRegBAUD(void)
	{
		return UART.BAUD;
	}
	INLN void setRegBAUD(uint16_t b)
	{
		UART.BAUD = b;
	}
	
	INLN bool resetBaud_MAXFSPU(void)
	{
		UART.BAUD = UART.BAUD / (24000000/F_CPU);
		return true;
	}
	bool setBaudMaxClock(uint16_t fbaudDiv1000)
	{
		if(timerNo != SPI_MODE)
		{
			uint16_t baud = (24000000/1e3)*4/fbaudDiv1000;
			uint8_t X2 = 0;
			
			if (baud < 64)
			{
				baud = (24000000/1e3)*8/fbaudDiv1000;

				if (baud < 64) return false;
				
				X2=1;
			}
			
			UART.BAUD = baud;

			if (X2) UART.CTRLB |= USART_RXMODE_CLK2X_gc;
			else UART.CTRLB &= ~(0x03<<1);

			//#ifndef NOINT_UART            //1024
			TB.CCMP = (fbaudDiv1000 >= 500) ? 3*1024 : 35*(24000000/1e3)/fbaudDiv1000;
			TB.CNT = 0;
			//#endif
		}
		else
		{
			uint16_t baud = (24000000/1e3)/2/fbaudDiv1000;
			baud <<=6;
			
			if (baud < 64) return false;
			
			UART.BAUD = baud;
		}
		return true;
	}
	
    void setTimout(uint16_t ccmp)
    {
		TB.CCMP = ccmp;
		TB.CNT = 0;
    }
    uint16_t getTimout(void)
    {
	  return TB.CCMP;
    }
    
	bool setBaud(uint16_t fbaudDiv1000)
	{
		if(timerNo != SPI_MODE)
		{
			uint16_t baud = (F_CPU/1e3)*4/fbaudDiv1000;
			uint8_t X2 = 0;
		
			if (baud < 64)
			{
				baud = (F_CPU/1e3)*8/fbaudDiv1000;

				if (baud < 64) return false;
			
				X2=1;
			}
		
			UART.BAUD = baud;

			if (X2) UART.CTRLB |= USART_RXMODE_CLK2X_gc;
			else UART.CTRLB &= ~(0x03<<1);

			//#ifndef NOINT_UART            //1024  
			TB.CCMP = (fbaudDiv1000 >= 500) ? 1024 : 35*(F_CPU/1e3)/fbaudDiv1000;
			TB.CNT = 0;
			//#endif
		}
		else
		{
			uint16_t baud = (F_CPU/1e3)/2/fbaudDiv1000;
			baud <<=6;
			
			if (baud < 64) return false;
			
			UART.BAUD = baud;			
		}			
		return true;
	}

	INLN void _rxc(void)
	{	
		nowaitSF();
		//stop timer
		TB.CTRLA = 0;
		// update buffer		
		uint8_t d = UART.RXDATAL;
		if (cnt < buffLen) buf[cnt++] = d;
		// Uart Busy
		lock();
		startTimout();
	}
	INLN void _timOut(void)
	{
		//stop timer
		TB.INTFLAGS = TCB_CAPT_bm;
		TB.CTRLA = 0;
		// update counts
		Count = cnt;
		cnt = 0;	
		// restore Uart
		waitSF();
		unlock();
		setReadyRxD();
	}
	
	INLN void startTxD(const buf_len_t count)
	{
		if(timerNo != SPI_MODE)
		{		
			disableRxD(); // flash all Rx buffers stop rx
			clrReadyRxD();
			enableTxD();
			lock();
			cnt = 1;
			Count = count;
			UART.TXDATAL = buf[0];
		}
		else
		{
			lock();
			cnt = 1;
			Count = count;
			UART.TXDATAL = buf[0];
		}
	}
	INLN void _txc_spi(void)
	{
		UART.STATUS = USART_TXCIE_bm;
		buf[buffLen/2 + cnt] = UART.RXDATAL;
		if (cnt < Count)
		{
			UART.TXDATAL = buf[cnt++];
		}
		else
		{
			cnt = 0;
			unlock();
			setReadyRxD();
		}
	}	
	INLN void _txc(void)
	{
		UART.STATUS = USART_TXCIE_bm;
		if (cnt < Count)
		{
			UART.TXDATAL = buf[cnt++];
		}
		else
		{
			cnt = 0;
			disableTxD();
			unlock();
			enableRxD();
		}
	}
    
	INLN void CRCSendPool(buf_len_t n)
    {
	    disableRxD();		
		enableTxD();
	    *(uint16_t*)&buf[n] = CRC(buf, n);
		for(buf_len_t i=0; i<n+2;i++) wait_writeTxC(buf[i]);		
		//waitTxC();
		disableTxD();
		enableRxD();		
    }
	INLN void readPool(void)
	{
		cnt = 1;
		while (!(UART.STATUS & USART_RXCIF_bm));		
		buf[0] = UART.RXDATAL;
		TB.INTFLAGS = TCB_CAPT_bm;
		startTimout();
		do
		{
		 if (UART.STATUS & USART_RXCIF_bm) 
		 {
			TB.CTRLA = 0;
			buf[cnt++] = UART.RXDATAL;
			if (cnt == buffLen)
			{
				Count = cnt;
				return;
			}
			TB.INTFLAGS = TCB_CAPT_bm;
			startTimout();
		 }		 
		} while(! (TB.INTFLAGS & TCB_CAPT_bm) );
		TB.CTRLA = 0;
		Count = cnt;
	}

    INLN void Send(buf_len_t n)
    {
		disableRxD();
	    startTxD(n);
    }
	
	
    INLN void CRCSend(buf_len_t n)
    {
		disableRxD();
		uint16_t* pn = (uint16_t*)(&buf[n]);		
	    *pn = CRC(buf, n);
	    startTxD(n+2);
    }
    INLN void CRCSend(void* src, buf_len_t n)
    {
		disableRxD();
	    memcpy(&buf[DATA_POS], src, n);
	    CRCSend(n+HEADER_LEN);
	}//*/
	INLN void noIntMode(void)
	{
		UART.CTRLA &= ~(
		  USART_DREIE_bm    /* Data Register Empty Interrupt Enable: disabled */
		| USART_RXCIE_bm    /* Receive Complete Interrupt Enable: disabled */
		| USART_TXCIE_bm   /* Transmit Complete Interrupt Enable: disabled */
		);
	}
	INLN void intMode(void)
	{
		UART.STATUS = USART_RXSIE_bm | USART_RXCIE_bm | USART_TXCIE_bm;
		UART.CTRLA |= USART_RXCIE_bm | USART_TXCIE_bm;
	}
	//INLN void write1(const uint8_t data)
	//{
		//UART.TXDATAL = data;
	//}
	INLN void writeTxC(const uint8_t data)
	{
		while (!(UART.STATUS & USART_TXCIF_bm));
		UART.TXDATAL = data;
		UART.STATUS = USART_TXCIF_bm;
	}
	INLN void writeTxC2(const uint8_t data)
	{
		while (!(UART.STATUS & USART_TXCIF_bm));
		UART.STATUS = USART_TXCIF_bm;
		UART.TXDATAL = data;
		while (!(UART.STATUS & USART_TXCIF_bm));
		UART.STATUS = USART_TXCIF_bm;
	}
	INLN void wait_writeTxC(const uint8_t data)
	{
		//_delay_us(TB.CCMP/10);
		UART.STATUS = USART_TXCIF_bm;
		UART.TXDATAL = data;
		while (!(UART.STATUS & USART_TXCIF_bm));
		//UART.STATUS = USART_TXCIF_bm;
	}
	INLN void write(const uint8_t data)
	{
		UART.TXDATAL = data;
		UART.STATUS = USART_TXCIF_bm;
		while (!(UART.STATUS & USART_DREIF_bm));
	}
	INLN void writeFast(const uint8_t data)
	{
		UART.TXDATAL = data;
		UART.STATUS = USART_TXCIF_bm;
	}
	INLN void writeWait(void)
	{
//		UART.STATUS = USART_TXCIF_bm;
		while (!(UART.STATUS & USART_DREIF_bm));
	}
	//INLN void waitTxC(void)
	//{
		//while (!(UART.STATUS & USART_TXCIF_bm));
	//}
}; 

#define EXTERN_USART(n) extern usart_t<n,USEUSART##n> serial##n;
#ifdef USEUSART0
	EXTERN_USART(0)
#endif
#ifdef USEUSART1
	EXTERN_USART(1)
#endif
#ifdef USEUSART2
	EXTERN_USART(2)
#endif
#ifdef USEUSART3
	EXTERN_USART(3)
#endif
#ifdef USEUSART4
	EXTERN_USART(4)
#endif


#endif /* USARTS_H_ */