#pragma once

#include <avr/io.h>
#include <stddef.h>
#include <getarg.h>
#include <tools.h>
#include <clock.hpp>


#define I2C0 0
#define I2C1 1

#define I2C_EVENTS  (9)

/**
 * @ingroup i2c_host_interrupt
 * @typedef enum i2c_host_error_t
 * @brief Enumeration for the possible errors in the I2C host driver.
 */ 
typedef enum
{
    I2C_ERROR_NONE, /**< Indicates no error */
    I2C_ERROR_ADDR_NACK, /**< Indicates that the I2C client did not acknowledge the address */
    I2C_ERROR_DATA_NACK, /**< Indicates that the I2C client did not acknowledge the data */
    I2C_ERROR_BUS_COLLISION, /**< Indicates an error caused due to bus collision */
}i2c_host_error_t;

/**
 * @ingroup i2c_host_interrupt
 * @typedef enum i2c_event_states_t
 * @brief Enumeration representing the states of host events.
 */
typedef enum
{
    I2C_STATE_IDLE = 0, /**< State indicates that the I2C bus is in an idle state */
    I2C_STATE_SEND_RD_ADDR, /**< State indicates the transmission of the address for a read operation */
    I2C_STATE_SEND_WR_ADDR, /**< State indicates the transmission of the address for a write operation*/
    I2C_STATE_TX, /**< State indicates the data transmission */
    I2C_STATE_RX, /**< State indicates the data reception */
    I2C_STATE_NACK, /**< State indicates that a negative acknowledgment was received from the I2C client */
    I2C_STATE_ERROR, /**< State indicates an error */
    I2C_STATE_STOP, /**< State indicates stop operation */
    I2C_STATE_RESET  /**< State indicates the reset event */
} i2c_event_states_t;

//typedef void (*twi_callback_t)(void);

template <uint8_t twiNo, uint8_t altNo>
class twi_im_t 
{
		#define TWI (*(TWI_t *) (0x0900 + 20*twiNo))
		#define TWI_BAUD(F_SCL, T_RISE)((((((float)F_CPU / (float)(F_SCL))) - 10 - ((float)F_CPU * (T_RISE) / 1000000))) / 2)
private:
   // bool busy;					/**< A flag that signifies the host is currently busy */
    uint16_t address;				/**< I2C client address */
    uint8_t *writePtr;				/**< Pointer to the buffer used for transmission */
    uint8_t writeLength;			/**< The number of bytes to be written or sent to the client */
    uint8_t *readPtr;				/**< Pointer to the buffer for received data */
    uint8_t readLength;				/**< Number of bytes to be read from the client */
    bool switchToRead;				/**< Boolean flag to indicate switching to read mode */
    i2c_host_error_t errorState;	/**< An instance of i2c_host_error_t */
    i2c_event_states_t state;		/**< An instance of i2c_host_event_states_t */
	void (*Callback)(void);
	INLN uint8_t _busy(void)
	{
		return GPR.GPR1 & (1 << (twiNo+5));
	}
	INLN void _busy_on(void)
	{
		GPR.GPR1 |= (1 << (twiNo+5));
	}
	INLN void _busy_off(void)
	{
		GPR.GPR1 &= ~ (1 << (twiNo+5));
	}
	
	INLN PORT_t* Port(void)
	{
		return (twiNo == 0) ? (altNo == ALT2 ? &PORTC : &PORTA) : (altNo == ALT2 ? &PORTB : &PORTF);
	}
public:
		INLN twi_im_t(void)
		{
			PORTMUX.TWIROUTEA |= altNo << (twiNo*2);	   			PORT_t* p = Port();			p->PIN2CTRL = PORT_ISC_INTDISABLE_gc;
			p->PIN3CTRL = PORT_ISC_INTDISABLE_gc;		
			
			Deinitialize();
		}
void Initialize(void)
{
	
	// Disable the peripheral
	TWI.MCTRLA &= ~(1 << TWI_ENABLE_bp);

	// FMPEN OFF; INPUTLVL I2C; SDAHOLD OFF; SDASETUP 4CYC;
	TWI.CTRLA = 0x0;

	// Debug Run
	TWI.DBGCTRL = 0x0;

	// Host Baud Rate Control
	TWI.MBAUD = (uint8_t)TWI_BAUD(101522, 0.1);

	// Host Address
	TWI.MADDR = 0x0;

	// Host Data
	TWI.MDATA = 0x0;
	
	// ARBLOST disabled; BUSERR disabled; BUSSTATE UNKNOWN; CLKHOLD disabled; RIF disabled; WIF disabled;
	TWI.MSTATUS = 0x0;

	// ACKACT ACK; FLUSH disabled; MCMD NOACT;
	TWI.MCTRLB = 0x0;
	
	// ENABLE enabled; QCEN disabled; RIEN enabled; SMEN disabled; TIMEOUT DISABLED; WIEN enabled;
	TWI.MCTRLA = 0xC1;

	// Force bus to go in idle state
	TWI.MSTATUS = TWI_BUSSTATE_IDLE_gc;

	//Callback = NULL;
}

void Deinitialize(void)
{
	// FMPEN OFF; INPUTLVL I2C; SDAHOLD OFF; SDASETUP 4CYC;
	TWI.CTRLA = 0x00;
	
	// Debug Run
	TWI.DBGCTRL = 0x00;
	
	// Host Baud Rate Control
	TWI.MBAUD = (uint8_t)TWI_BAUD(101522, 0.1);
	
	// ENABLE enabled; QCEN disabled; RIEN enabled; SMEN disabled; TIMEOUT DISABLED; WIEN enabled;
	TWI.MCTRLA = 0x00;
	
	// ARBLOST disabled; BUSERR disabled; BUSSTATE UNKNOWN; CLKHOLD disabled; RIF disabled; WIF disabled;
	TWI.MSTATUS = 0x00;

	// Host Address
	TWI.MADDR = 0x00;
	
	// ACKACT ACK; FLUSH disabled; MCMD NOACT;
	TWI.MCTRLB = 0x00;
	
	// Host Data
	TWI.MDATA = 0x00;

	TWI.MCTRLA &= ~(1 << TWI_ENABLE_bp);
	// Force bus to go in idle state
	TWI.MSTATUS = TWI_BUSSTATE_IDLE_gc;	
}

bool Write(uint16_t addr, uint8_t *data, uint8_t dataLength)
{
	bool retStatus = false;
	
	if (!IsBusy())
	{
		_busy_on(); //busy = true;
		address = addr;
		switchToRead = false;
		writePtr = data;
		writeLength = dataLength;
		readPtr = NULL;
		readLength = 0;
		errorState = I2C_ERROR_NONE;
		WriteStart();
		retStatus = true;
	}
	
	return retStatus;
}

bool Read(uint16_t addr, uint8_t *data, uint8_t dataLength)
{
	bool retStatus = false;
	
	if (!IsBusy())
	{
		_busy_on(); //busy = true;
		address = addr;
		switchToRead = false;
		readPtr = data;
		readLength = dataLength;
		writePtr = NULL;
		writeLength = 0;
		errorState = I2C_ERROR_NONE;
		ReadStart();
		retStatus = true;
	}
	
	return retStatus;
}

bool WriteRead(uint16_t addr, uint8_t *writeData, uint8_t wl, uint8_t *readData, uint8_t rl)
{
	bool retStatus = false;
	
	if (!IsBusy())
	{
		_busy_on(); //busy = true;
		address = addr;
		switchToRead = true;
		writePtr = writeData;
		writeLength = wl;
		readPtr = readData;
		readLength = rl;
		errorState = I2C_ERROR_NONE;
		WriteStart();
		retStatus = true;
	}
	
	return retStatus;
}

i2c_host_error_t ErrorGet(void)
{
	i2c_host_error_t retErrorState = errorState;
	errorState = I2C_ERROR_NONE;
	
	return retErrorState;
}

bool IsBusy(void)
{
	return (_busy() || ((TWI.MSTATUS & TWI_BUSSTATE_gm) != TWI_BUSSTATE_IDLE_gc));
}

void CallbackRegister(void (*callbackHandler)(void))
{
	if (callbackHandler != NULL)
	{
		Callback = callbackHandler;
	}
}
INLN void _isr(void)
{/* cppcheck-suppress misra-c2012-5.5 */
	if (0U != (TWI0.MSTATUS & (TWI_RXACK_bm | TWI_BUSERR_bm | TWI_ARBLOST_bm)))
	{
		ErrorEventHandler();
	}
	else if (0U != (TWI0.MSTATUS & (TWI_RIF_bm | TWI_WIF_bm | TWI_CLKHOLD_bm)))
	{
		EventHandler();
	}
	else
	{
		// No action required
	}
}

private:

void Close(void)
{
	_busy_off(); //busy = false;
	address = 0xFF;
	writePtr = NULL;
	readPtr = NULL;
	state = I2C_STATE_IDLE;
	// Clear interrupt status
	TWI.MSTATUS = (TWI_RIF_bm | TWI_WIF_bm);
	TWI.MSTATUS = TWI_BUSSTATE_IDLE_gc;
}

 void ReadStart(void)
{
	state = I2C_EVENT_SEND_RD_ADDR();
}

 void WriteStart(void)
{
	state = I2C_EVENT_SEND_WR_ADDR();
}

i2c_event_states_t TWI_eventTable(i2c_event_states_t s)
{
	switch (s)
	{
     case I2C_STATE_IDLE: return I2C_EVENT_IDLE();
     case I2C_STATE_SEND_RD_ADDR: return I2C_EVENT_SEND_RD_ADDR();
     case I2C_STATE_SEND_WR_ADDR: return I2C_EVENT_SEND_WR_ADDR();
     case I2C_STATE_TX: return I2C_EVENT_TX();
     case I2C_STATE_RX: return I2C_EVENT_RX();
     case I2C_STATE_NACK: return I2C_EVENT_NACK();
     case I2C_STATE_ERROR: return I2C_EVENT_ERROR();
     case I2C_STATE_STOP: return I2C_EVENT_STOP();
     case I2C_STATE_RESET: return I2C_EVENT_RESET();
	 default: return I2C_STATE_IDLE;		
	}
}

 void EventHandler(void)
{
	state = TWI_eventTable(state);
}

 void ErrorEventHandler(void)
{
	// Check the bus error
	if ((TWI.MSTATUS & TWI_BUSERR_bm) || (TWI.MSTATUS & TWI_ARBLOST_bm))
	{
		state = I2C_STATE_ERROR;
		errorState = I2C_ERROR_BUS_COLLISION;
		
		// Clear the error flags
		TWI.MSTATUS = (TWI_BUSERR_bm | TWI_ARBLOST_bm);
	}
	// Check if address NAK
	else if ((TWI.MADDR) && (TWI.MSTATUS & TWI_RXACK_bm))
	{
		state = I2C_STATE_NACK;
		errorState = I2C_ERROR_ADDR_NACK;
	}
	// Check if data NAK
	else if ((TWI.MDATA) && (TWI.MSTATUS & TWI_RXACK_bm))
	{
		state = I2C_STATE_NACK;
		errorState = I2C_ERROR_DATA_NACK;
	}
	else
	{
		// No action required
	}
	
	state = TWI_eventTable(state);
	
	if(errorState != I2C_ERROR_NONE)
	{
		if (Callback != NULL)
		{
			Callback();
		}
	}
}

/* I2C Event interfaces */
 i2c_event_states_t I2C_EVENT_IDLE(void)
{
	_busy_off(); //busy = false;
	// Force bus to go in idle state
	TWI.MSTATUS = TWI_BUSSTATE_IDLE_gc;
	
	return I2C_STATE_RESET;
}

 i2c_event_states_t I2C_EVENT_SEND_RD_ADDR(void)
{
	TWI.MADDR = (uint8_t) ((address << 1) | 1U);
	return I2C_STATE_RX;
}

 i2c_event_states_t I2C_EVENT_SEND_WR_ADDR(void)
{
	TWI.MADDR = (uint8_t) (address << 1);
	
	return I2C_STATE_TX;
}

 i2c_event_states_t I2C_EVENT_TX(void)
{
	i2c_event_states_t retEventState = I2C_STATE_TX;

	if (0U != writeLength)
	{
		writeLength--;
		TWI.MDATA = *writePtr;
		writePtr++;
		retEventState = I2C_STATE_TX;
	}
	else
	{
		if (switchToRead)
		{
			switchToRead = false;
			retEventState = I2C_STATE_SEND_RD_ADDR;
		}
		else
		{
			retEventState = I2C_EVENT_STOP();
		}
	}

	return retEventState;
}

 i2c_event_states_t I2C_EVENT_RX(void)
{
	i2c_event_states_t retEventState = I2C_STATE_RX;

	if (readLength == 1U)
	{
		// Next byte will be last to be received, setup NACK
		TWI.MCTRLB |= TWI_ACKACT_bm;
	}
	else
	{
		// More bytes to receive, setup ACK
		TWI.MCTRLB &= ~TWI_ACKACT_bm;
	}

	if (0U != --readLength)
	{
		*readPtr = TWI.MDATA;
		readPtr++;
		// Execute Acknowledge Action followed by a byte read operation
		CommandUpdate(TWI_MCMD_RECVTRANS_gc);
		retEventState = I2C_STATE_RX;
	}
	else
	{
		*readPtr = TWI.MDATA;
		readPtr++;
		// Next byte will be last to be received, setup NACK
		TWI.MCTRLB |= TWI_ACKACT_bm;
		retEventState = I2C_EVENT_STOP();
	}

	return retEventState;
}

 i2c_event_states_t I2C_EVENT_NACK(void)
{
	i2c_event_states_t retEventState = I2C_STATE_NACK;
	retEventState = I2C_EVENT_STOP();
	
	return retEventState;
}

 i2c_event_states_t I2C_EVENT_ERROR(void)
{
	// Clear bus collision status flag
	i2c_event_states_t retEventState = I2C_STATE_ERROR;
	// Clear interrupt status
	TWI.MSTATUS = (TWI_RIF_bm | TWI_WIF_bm);
	retEventState = I2C_EVENT_RESET();
	
	return retEventState;
}

 i2c_event_states_t I2C_EVENT_STOP(void)
{
	// Send stop
	CommandUpdate(TWI_MCMD_STOP_gc);
	Close();
	
	return I2C_STATE_IDLE;
}

 i2c_event_states_t I2C_EVENT_RESET(void)
{
	// Reset the bus
	TWI.MCTRLA &= ~(1 << TWI_ENABLE_bp);
	TWI.MCTRLA |= 1 << TWI_ENABLE_bp;

	_busy_off(); //busy = false;
	
	return I2C_STATE_IDLE;
}

INLN void CommandUpdate(TWI_MCMD_t cmd)
{
	TWI.MCTRLB = (TWI.MCTRLB & ~TWI_MCMD_gm) | cmd;
}
	
};