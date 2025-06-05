#ifndef _PINOUT_HPP_
#define _PINOUT_HPP_

#include <tools.h>

#ifdef PINOUTDEF
# undef PINOUTDEF
#endif

template <uint8_t port, uint8_t bit>
class pinout_t
{
	#define PINOUTDEF (*(PORT_t*) (0x0400 + 0x20 * port))	
public:
	
	INLN pinout_t(void)
	{
		Off();
		PINOUTDEF.DIRSET = 1 << bit;
	}
	INLN void On(void)
	{
		PINOUTDEF.OUTSET = 1 << bit;
	}
	INLN void Off(void)
	{
		PINOUTDEF.OUTCLR = 1 << bit;
	}
	INLN bool IsOn(void)
	{
		return PINOUTDEF.OUT & (1 << bit);
	}
};

template <uint8_t port, uint8_t bit>
class pinoutOn_t
{
	#define PINOUTDEF (*(PORT_t*) (0x0400 + 0x20 * port))
	public:
	
	INLN pinoutOn_t(void)
	{
		On();
		PINOUTDEF.DIRSET = 1 << bit;
	}
	INLN void On(void)
	{
		PINOUTDEF.OUTSET = 1 << bit;
	}
	INLN void Off(void)
	{
		PINOUTDEF.OUTCLR = 1 << bit;
	}
	INLN bool IsOn(void)
	{
		return PINOUTDEF.OUT & (1 << bit);
	}
};

#endif