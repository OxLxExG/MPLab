#pragma once

#include <avr/io.h>
#include <tools.h>
#include <usarts.hpp>
#include <getarg.h>
#include <clock.hpp>

#define USE_INDIC GETARG_2(TURBO)

#if USE_INDIC == 1
#ifdef DIOD
 #include <Indicator.hpp>
#endif
#endif

#define  TURBO_UART_N GETARG_1(TURBO)
#if   TURBO_UART_N == 0
#define TuUart serial0
#elif TURBO_UART_N == 1
#define TuUart serial1
#elif TURBO_UART_N == 2
#define TuUart serial2
#elif TURBO_UART_N == 3
#define TuUart serial3
#elif TURBO_UART_N == 4
#define TuUart serial4
#endif

template <uint16_t DEFSpeed>
class turbo_t
{
private:
    uint8_t curTurbo;
    uint8_t TurboTimer;
    INLN void RestoreTurbo(void)
    {
        if(curTurbo >= 3)
        {
            Clock.RestoreExternalClock();
            Clock.HiSpeedReady = false;
        }
        curTurbo = 0;
        
        #if USE_INDIC == 1
        #ifdef DIOD
        Indicator.User = false;
        #endif
        #endif

        TurboTimer = 0;
    }
public: 
        
    INLN void Set(uint8_t turbo)
    {
        if (turbo)
        {
            curTurbo = turbo;
            TurboTimer =  4;
            if (turbo >= 3)
            {
                Clock.MAXInternalClock();
                Clock.HiSpeedReady = true;
            }
            #if USE_INDIC == 1
        #ifdef DIOD
            Indicator.User = true;
            Indicator.Off();
            #endif
            #endif
        }
        else RestoreTurbo();

        switch (turbo)
        {
            case 1:
            TuUart.setBaud(500);
            break;
            case 2:
            TuUart.setBaud(1000);
            break;
            case 3:
            TuUart.setBaudMaxClock(1500);
            break;
            case 4:
            TuUart.setBaudMaxClock(2000);
            break;
            case 6:
            TuUart.setBaudMaxClock(3000);
            break;
            default:
            TuUart.setBaud(DEFSpeed);
            break;
        }                    
    }
    INLN void RestartTimer(void)
    {
         if (TurboTimer > 0) TurboTimer = 4;
    }
    INLN void Handler2sec(void)
    {
        if (TurboTimer > 0)
        {
            if(--TurboTimer == 0) 
            {
                RestoreTurbo();

                TuUart.setBaud(DEFSpeed);
            }
        }
    }
};

#if !defined(TURBO)
# error "Global define TURBO=UART1,INDIC"
#endif


