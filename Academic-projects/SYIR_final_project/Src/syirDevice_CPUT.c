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

typedef struct syirDevice_StatusInternalTemp {
    volatile uint8_t    state;     // Stan pomiaru
             uint8_t    rate;       // Sample rate - PRESCALER
             float      scale;     // Wspolczynnik skalowania probek
             uint16_t * ptr;
} syirDevice_StatusInternalTemp_t;

syirDevice_StatusInternalTemp_t statusInternalTemp;


void syirDevice_INIT_internalTemperature(uint8_t sample_rate)
{
    #define TEMPSENSE0	(0x2E)
    #define TEMPSENSE1	(0x2F)

    int32_t scale = 0;
  
    cli();
    { // Odczyt z pamieci EEPROM wspolczynnikow skalowania
        NVM.CMD = NVM_CMD_READ_CALIB_ROW_gc;
        scale   = pgm_read_byte(TEMPSENSE0);
        scale  += (pgm_read_byte(TEMPSENSE1) << 8);
        NVM.CMD = NVM_CMD_NO_OPERATION_gc;
    }
    sei();
    
    // Junction temperature scale factor (TEMPSENSE): 85C
    statusInternalTemp.scale    = (85.00) / (float)scale;
    statusInternalTemp.state    = 0;
    statusInternalTemp.rate     = sample_rate;  
}

uint8_t syirDevice_MEASURE_internalTemperature(uint16_t *temp) 
{
    switch (statusInternalTemp.state)  {
        case 0: {
                statusInternalTemp.ptr   = temp;
                statusInternalTemp.state = 1;
                /* ADC Configuration*/
                cli();
                {
                    ADCA.PRESCALER      = statusInternalTemp.rate;  // 
                    ADCA.CTRLB          &= ~(1<<4);                 // Conversion unsigned Mode
                    ADCA.REFCTRL         = (1 << 1) | (1<<0);       // Internal Temperature Measure Enable
                    ADCA.CTRLA          = (1 << 0);                 // Enable ADCA
                    
                    ADCA.CH0.MUXCTRL     = 0;           //  Positive reference is TEMP

                    ADCA.CH0.INTCTRL    = (1 << 0);     // INT level = low  
                    ADCA.CH0.CTRL       = (1 << 7);     // Start Measurement
                }     
                sei();       
                return 0;
        }
        case 1: return 0;
        case 2: {
                cli();
                {
                    *statusInternalTemp.ptr = ((ADCA.CH0.RESH << 8) | ADCA.CH0.RESL);
                }
                sei();
                statusInternalTemp.state = 0;
            return 1;
        }

    }
    return 0;
   
}

float syirDevice_EVAL_internalTemperature(uint16_t temp) {
    float meas  = ((float)(temp));
    float scale = (float)statusInternalTemp.scale;
    meas = (meas * scale);
    return meas;
}

ISR(ADCA_CH0_vect)
{
    statusInternalTemp.state++;
}