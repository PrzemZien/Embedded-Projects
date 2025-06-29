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


#ifndef SYIRDEVICE_TIME_H
#define SYIRDEVICE_TIME_H

typedef enum {
    TIME_TICK_MILI_SECONDS  = 0,
    TIME_TICK_SECONDS       = 1,
    TIME_TICK_MINUTES       = 2,
    TIME_TICK_HOURS         = 3,
} syirDevice_TIMEunit_t; 

    typedef uint16_t syirDeviceTIMEticks_t;

    extern uint8_t               syirDevice_INIT_time(syirDevice_TIMEunit_t unit);
    extern uint8_t               syirDevice_PERIOD_time(uint32_t period);
    extern syirDeviceTIMEticks_t syirDevice_CHECK_time (syirDeviceTIMEticks_t *prev);
    extern syirDeviceTIMEticks_t syirDevice_CHECK_N_time (syirDeviceTIMEticks_t N, syirDeviceTIMEticks_t *prev);

#endif