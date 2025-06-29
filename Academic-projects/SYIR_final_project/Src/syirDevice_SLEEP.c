/* ************************************************************************* */
/*                                                                           */
/* SYIR - Systemy Internetu Rzeczy LAB                                       */
/*                                                                           */
/* Autorzy:  Arkadiusz Luczyk; Jakub Jasinski; Marek Niewiński               */
/* Email  :  arkadiusz.luczyk@pw.edu.pl                                      */
/*           jakub.jasinski@pw.edu.pl                                        */
/*           marek.niewinski@pw.edu.pl                                       */
/*                                                                           */
/*     Do wyłącznego użytku w ramach zajęć SYIR                              */
/*     Do wylacznego uzytku w ramach zajec SYIR                              */
/*                                                                           */
/* ************************************************************************* */

#include "syirDevice.h"
#include <avr/sleep.h>

typedef struct syirDevice_StatusSleep {

    volatile uint8_t sleeping;
    uint8_t          mode;

} syirDevice_StatusSleep_t;

syirDevice_StatusSleep_t statusSLEEP;

uint8_t syirDevice_INIT_sleep(syirDevice_SLEEP_t mode) {

    statusSLEEP.mode     = mode;

    switch (mode) {
        case SLEEP_IDLE:     
        case SLEEP_POWER_DOWN:       
        case SLEEP_POWER_SAVE:       
        case SLEEP_STANDBY:      
        case SLEEP_EXTENDED_STANDBY:     
            break;

        default: {
            cli(); 
            {
                SLEEP.CTRL          &= ~(1 << 0);
                statusSLEEP.sleeping = 0;
                statusSLEEP.mode     = SLEEP_NONE;
            }
            sei();
            return 0;
        }
    }

    cli(); 
    {
        statusSLEEP.sleeping = 0;
        SLEEP.CTRL           = (mode << 1);
    }
    sei();
    return 1;
}

void syirDevice_ENTER_sleep(void) {
    
    if (statusSLEEP.sleeping > 0) {
        // Zabezpieczenie przed wywolaniem w ISR
        cli();
        {
            SLEEP.CTRL &= ~(1 << 0);
            statusSLEEP.sleeping = 0;   
        }
        sei();
        return;
    }

    if (statusSLEEP.mode == SLEEP_NONE)
        return;

    cli();
    {
        SLEEP.CTRL          |= (1 << 0);
        statusSLEEP.sleeping = 1;
    }
    sei();

	sleep_cpu();
    
    cli();
    {
        SLEEP.CTRL &= ~(1 << 0);
        statusSLEEP.sleeping = 0;
    }
    sei();

}

uint8_t  syirDevice_STATUS_sleep(void) {
    return statusSLEEP.sleeping;
}
