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

#include <xc.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include <syirDevice.h>

typedef struct syirDevice_StatusAmbientTemp {
    volatile uint8_t    state;   // 
             uint8_t    rate;    // Sample rate - PRESCALER
             uint16_t * ptr;
} syirDevice_StatusAmbientTemp_t;

syirDevice_StatusAmbientTemp_t statusAmbientTemp;


void syirDevice_INIT_ambientTemperature(uint8_t sample_rate)
{
    statusAmbientTemp.state = 0;
    statusAmbientTemp.rate  = sample_rate;
    statusAmbientTemp.ptr   = NULL;

    cli();
    {   // Power up Thermistor
        PORTA.DIR       |= (1 << 0);
        PORTA.OUT       |= (1 << 0);
        PORTA.PIN1CTRL  |= (7 << 0);    // Set PIN1 as analog sens input
    }
    sei();
}


uint8_t syirDevice_MEASURE_ambientTemperature(uint16_t *temp)
{
    switch (statusAmbientTemp.state)
    {
        case 0: {
            statusAmbientTemp.ptr   = temp;
            statusAmbientTemp.state = 1;
            cli();
            {
                ADCA.PRESCALER      = statusAmbientTemp.rate;   // 
                ADCA.CTRLB          &= ~(1<<4);                 // Conversion unsigned Mode
                ADCA.CTRLB          = (0<<1);                   // Resolution 12-bit
                ADCA.REFCTRL        = (1 << 1);                 // BandGap reference
                ADCA.CTRLA          = (1 << 0);                 // Enable ADCA
        
                ADCA.CH1.MUXCTRL    = (1 << 3);                 //  Positive reference is PIN1
        
                ADCA.CH1.INTCTRL    = (0 << 2) | (1 << 0);      // Int mode = complete ;INT level = low
                ADCA.CH1.CTRL       = (1 << 7);                 // ADCA Ch1 start
            }            
            sei();
            return 0;
        }
        case 2: {
            cli();
            {
                *statusAmbientTemp.ptr = ((ADCA.CH1.RESH << 8) | ADCA.CH1.RESL);                
            }
            sei();
            statusAmbientTemp.state = 0;
            return 1;
        }
        default: 
            return 0;
    }
    return 0;

}

float syirDevice_EVAL_ambientTemperature(uint16_t temp)
{
    #define AD_CONV_RESOLUTION		(1.0/4096)
    #define MCP_ZERO_TEMP_VOLTAGE	(0.40)	
    #define MCP_TEMP_COEFFICIENT	(51.28)

    float meas  = ((float)temp);
    float scale = MCP_TEMP_COEFFICIENT * AD_CONV_RESOLUTION;
    meas = meas * scale - MCP_ZERO_TEMP_VOLTAGE;

    return meas;
}

ISR(ADCA_CH1_vect)
{
    statusAmbientTemp.state++;
}
