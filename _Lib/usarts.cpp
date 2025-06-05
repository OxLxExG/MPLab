/*
 * CPPFile1.cpp
 *
 * Created: 28.10.2021 13:31:25
 *  Author: User
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <usarts.hpp>
#include <getarg.h>

#define INIT_USART_NOINT(n) usart_t<n,USEUSART##n> serial##n;

	//ISR(USART##n##_DRE_vect){ serial##n._dre(); } 

#define INIT_USART_INT(n,m) \
	usart_t<n,USEUSART##n> serial##n; \
	ISR(USART##n##_RXC_vect){ serial##n._rxc(); } \
    ISR(USART##n##_TXC_vect){ serial##n._txc(); } \
	ISR(TCB##m##_INT_vect){ serial##n._timOut(); }	

#define INIT_USART_INT_SPI(n) usart_t<n,USEUSART##n> serial##n;
// ISR(USART##n##_TXC_vect){ serial##n._txc_spi(); } 

#ifdef NOINT_UART
	#ifdef USEUSART0
	INIT_USART_NOINT(0)
	#endif
	#ifdef USEUSART1
	INIT_USART_NOINT(1)
	#endif
	#ifdef USEUSART2
	INIT_USART_NOINT(2)
	#endif
	#ifdef USEUSART3
	INIT_USART_NOINT(3)
	#endif
	#ifdef USEUSART4
	INIT_USART_NOINT(4)
	#endif
#else
	#ifdef USEUSART0
		#define TMR0 GETARG_3(USEUSART0)
		#if   (TMR0 == TB0)
		INIT_USART_INT(0,0)
		#elif (TMR0 == TB1)
		INIT_USART_INT(0,1)
		#elif (TMR0 == TB2)
		INIT_USART_INT(0,2)
		#elif (TMR0 == TB3)
		INIT_USART_INT(0,3)
		#elif (TMR0 == SPI_MODE)
		INIT_USART_INT_SPI(0)
		#endif
	#endif
	
	#ifdef USEUSART1
	  #define TMR1 GETARG_3(USEUSART1)
	  #if   (TMR1 == TB0)
		INIT_USART_INT(1,0)
	  #elif (TMR1 == TB1)
		INIT_USART_INT(1,1)
	  #elif (TMR1 == TB2)
		INIT_USART_INT(1,2)
	  #elif (TMR1 == TB3)
		INIT_USART_INT(1,3)
		#elif (TMR1 == SPI_MODE)
		INIT_USART_INT_SPI(1)
	  #endif
	#endif
	
	#ifdef USEUSART2
		#define TMR2 GETARG_3(USEUSART2)
		#if   (TMR2 == TB0)
		INIT_USART_INT(2,0)
		#elif (TMR2 == TB1)
		INIT_USART_INT(2,1)
		#elif (TMR2 == TB2)
		INIT_USART_INT(2,2)
		#elif (TMR2 == TB3)
		INIT_USART_INT(2,3)
		#elif (TMR2 == SPI_MODE)
		INIT_USART_INT_SPI(2)
		#endif
	#endif
	
	#ifdef USEUSART3
		#define TMR3 GETARG_3(USEUSART3)
		#if   (TMR3 == TB0)
		INIT_USART_INT(3,0)
		#elif (TMR3 == TB1)
		INIT_USART_INT(3,1)
		#elif (TMR3 == TB2)
		INIT_USART_INT(3,2)
		#elif (TMR3 == TB3)
		INIT_USART_INT(3,3)
		#elif (TMR3 == SPI_MODE)
		INIT_USART_INT_SPI(3)
		#endif
	#endif
	  
	#ifdef USEUSART4
		#define TMR4 GETARG_3(USEUSART4)
		#if   (TMR4 == TB0)
		INIT_USART_INT(4,0)
		#elif (TMR4 == TB1)
		INIT_USART_INT(4,1)
		#elif (TMR4 == TB2)
		INIT_USART_INT(4,2)
		#elif (TMR4 == TB3)
		INIT_USART_INT(4,3)
		#elif (TMR4 == SPI_MODE)
		INIT_USART_INT_SPI(4)
		#endif
	#endif
#endif // NOINT_UART

