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

#ifndef SYIRDEVICE_VDD_H
#define SYIRDEVICE_VDD_H

    typedef void (*syirDevice_vddEventFunction_t)(void);

    extern void     syirDevice_INIT_vdd(uint8_t sample_rate, uint8_t *full);
    extern uint8_t  syirDevice_MEASURE_vdd(uint16_t *vdd);
    extern uint8_t  syirDevice_FULL_vdd(uint8_t *full);
    extern float    syirDevice_EVAL_vdd(uint16_t vdd);

#endif