#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <avr/io.h>


//PINMUX
#define ALT2 2
#define ALT1 1
#define DEF0 0

#define CMP0 0
#define CMP1 1
#define CMP2 2

#define AINP0 0
#define AINP1 1
#define AINP2 2
#define AINP3 3

// TIMER B Timout Mode
#define  TB0 0
#define  TB1 1
#define  TB2 2
#define  TB3 3
#define  SPI_MODE 4


#define SP0 0
#define SP1 1

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

#define INV 1
#define NOINV 0

#define T2C 2.097152
#define T64_TIK ((T2C*F_CPU)/64/1024) //32.768 ms
#define T64_TIK_MAX ((T2C*24000000)/64/1024)

#define CLKNOOUT 0
#define CLKOUT 1

#define CLKHT  0
#define CLKRTC 1
#define CLKRC  2

#define TA0  0
#define TA1  1
#define TRTC 2
#define TNOT 3

#define NOINDIC 0
#define INDIC   1


#define INLN __attribute__ ((__always_inline__)) inline
#define NO_INIT __attribute__ ((section(".noinit")))

typedef union
{
	uint8_t B[2];
	int16_t u16;
} unio16_t;

typedef union
{
	uint8_t B[4];
	int32_t u32;
} unio32_t;

typedef union
{
	uint8_t B[8];
	uint64_t u64;
} unio64_t;

#endif
