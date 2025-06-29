/* ************************************************************************* */
/*                                                                           */
/* SYIR - Systemy Internetu Rzeczy LAB                                       */
/*                                                                           */
/* Autorzy:  Arkadiusz Luczyk; Jakub Jasinski; Marek Niewi?ski               */
/* Email  :  arkadiusz.luczyk@pw.edu.pl                                      */
/*           jakub.jasinski@pw.edu.pl                                        */
/*           marek.niewinski@pw.edu.pl                                       */
/*                                                                           */
/*     Do wy??cznego u?ytku w ramach zaj?? SYIR                              */
/*     Do wylacznego uzytku w ramach zajec SYIR                              */
/*                                                                           */
/* ************************************************************************* */

#include "syirDevice.h"

typedef struct syirDevice_StatusTIME {

    volatile syirDeviceTIMEticks_t tick;
                          uint16_t factor;
} syirDevice_StatusTIME_t;

syirDevice_StatusTIME_t statusTIME;

uint8_t syirDevice_PERIOD_time(uint32_t period) {

    uint16_t timeout    = 0;
    
    uint32_t cnt = period * statusTIME.factor;
    
    RTC32.CTRL = 0;
    while (RTC32.SYNCCTRL & (1 << 0)) {
        timeout++;
            if (timeout == 0)
                return 0;
    }
    cli();
    {
        RTC32.PER  = cnt;
	    RTC32.COMP = cnt;
	    RTC32.CNT  = 0;
        RTC32.CTRL |= (1 << 0);
        RTC32.INTCTRL = (3 << 0);
    }
    sei();
    return 1;
    
}

void delay_us(uint16_t delay)
	{
	#define CLK_SPEED_MHZ 32
	uint32_t counter;
	for (counter = 0; counter < delay * CLK_SPEED_MHZ/8 ; counter++)
		{
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		}
	return;
}

uint8_t syirDevice_INIT_time(syirDevice_TIMEunit_t unit) 
{
    uint16_t timeout    = 0;
    uint8_t notUse1Hz   = 0;
    
    statusTIME.tick     = 0;
    statusTIME.factor   = 1;

    switch (unit) {
        
        case TIME_TICK_MILI_SECONDS: 
        {
            notUse1Hz = 1;
            break;
        }

        case TIME_TICK_SECONDS:
            break;

        case TIME_TICK_MINUTES: {
            statusTIME.factor   = 60;
            break;
        }
        case TIME_TICK_HOURS: 
        {
            statusTIME.factor   = 60*60;
            break;
        }
        default:
            return 0;
    }

	VBAT.CTRL |= (1 << 1);          // Dostęp do VBAT
	CCP = 0xD8;                     // Odblokowanie zabezpieczonych rejestrow IO
	VBAT.CTRL = (1 << 0);           // Reset VBAT
	VBAT.CTRL |= (1 << 2);          // VBAT XOSCFDEN
	VBAT.CTRL |= (notUse1Hz << 4);

	delay_us(400);

	VBAT.CTRL |= (1 << 3);  // RTC OSCX RUN

	while (!(VBAT.STATUS & (1 << 3)))
    {
        timeout++;
        if (timeout == 0)
            return 0;
    }
    return 1;
}

syirDeviceTIMEticks_t syirDevice_CHECK_time (syirDeviceTIMEticks_t *prev) {
    syirDeviceTIMEticks_t tick = statusTIME.tick;
    if (tick == *prev)
        return 0;
    
    *prev = tick;
    return 1;  
}

syirDeviceTIMEticks_t syirDevice_CHECK_N_time (syirDeviceTIMEticks_t N, syirDeviceTIMEticks_t *prev)
{
    syirDeviceTIMEticks_t tick = statusTIME.tick;
    syirDeviceTIMEticks_t k = 0;
    syirDeviceTIMEticks_t old = *prev;

    if (tick > old)
        k = tick - old;

    if (tick < old)
        k = (0xFFFF - old) + tick;

    if (k < N ) 
        return 0;
    
    *prev = tick;
    return 1;
    
}

ISR(RTC32_OVF_vect)
{
    statusTIME.tick++;
}