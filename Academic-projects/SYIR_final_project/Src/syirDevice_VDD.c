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

#include <xc.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include <syirDevice.h>


typedef struct syirDevice_StatusVDD {
    volatile uint8_t    state;
    volatile uint8_t    full;
             uint8_t    rate;
             uint16_t * ptr;   
} syirDevice_StatusVDD_t;


syirDevice_StatusVDD_t statusVDD;


void syirDevice_INIT_vdd(uint8_t sample_rate, uint8_t *full)
{
    statusVDD.state         = 0;
    statusVDD.full          = 0;
    statusVDD.rate          = sample_rate;
    statusVDD.ptr           = NULL;

    cli();
    {   
        PORTA.DIR       |= (1 << 3);    // Zalacza dzielnik pomiarowy
        PORTA.OUT       |= (1 << 3);    

        PORTA.PIN4CTRL  |= (7 << 0);    // Set PIN4 as analog sens input

        // PORTA.0 - interrupt setting
        PORTA.DIR      &= ~(6 << 0);

        if (PORTA.IN & (1 << 6))   // Jesli jest 1 to naladowana
        {
            PORTA.PIN6CTRL = 0; // Sens RISING/FALLING
            statusVDD.full  = 1;
            *full = 1;
        }
        else        // W przeciwnym wypadku moze sie laduje
        {
            PORTA.PIN6CTRL = 0; // Sens RISING/FALLING
            statusVDD.full = 0;
            *full = 0;
        }
            
        PORTA.INT0MASK |= (1 << 6); // PIN 6
        PORTA.INTCTRL  |= (1 << 0); // LOW level INT 0        
    }
    sei();
}

uint8_t syirDevice_FULL_vdd(uint8_t *full) {
    if (statusVDD.full % 2 != *full)
    {
        *full = statusVDD.full % 2;
        return 1;
    }
    return 0;
}

uint8_t syirDevice_MEASURE_vdd(uint16_t *vdd)
{
    switch (statusVDD.state) 
    {
        case 0: {
            statusVDD.state = 1;
            statusVDD.ptr   = vdd;
            cli();
            {
                ADCA.PRESCALER      = statusVDD.rate;   // 
                ADCA.CTRLB          = (1<<4);           // Conversion signed Mode, 12-bit resolution

                ADCA.REFCTRL        = (1 << 1);         // BandGap reference
                ADCA.CTRLA          = (1 << 0);         // enable ADCA
                ADCA.CH2.INTCTRL    = (1 << 0);         // INT level = low
                
                ADCA.CH2.MUXCTRL    = (4 << 3) | (2 << 0);  //  Positive reference is PIN4 Neg - PIN 2
                ADCA.CH2.CTRL       = (2 << 0);         // Input diff
                
                ADCA.CH2.CTRL       |= (1 << 7);
            }              
            sei();
            return 0;
        }
        case 2: {
            cli();
            {
                *statusVDD.ptr = ((ADCA.CH2.RESH << 8) | ADCA.CH2.RESL);
            }
            sei();
            
            statusVDD.state = 0;
            return 1;
        }
        default: 
            return 0;
    }
    return 0;
}

float syirDevice_EVAL_vdd(uint16_t vdd) {
    
    #define AD_CONV_RESOLUTION		(2.0/4096)
    #define SCALE                   (124.0 / 24.00)   
    
    float meas = ((float)vdd) * AD_CONV_RESOLUTION * SCALE;
    return meas;    
}

ISR(ADCA_CH2_vect)
{
    statusVDD.state++;
}

ISR(PORTA_INT0_vect)    // Triggered at RISING / FALLING
{
    statusVDD.full++;
}
