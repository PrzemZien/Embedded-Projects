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

#ifndef SYIRDEVICE_CPUTEMP_H
#define SYIRDEVICE_CPUTEMP_H

    extern void     syirDevice_INIT_internalTemperature(uint8_t sample_rate);
    extern uint8_t  syirDevice_MEASURE_internalTemperature(uint16_t *temp);
    extern float    syirDevice_EVAL_internalTemperature(uint16_t temp);

#endif