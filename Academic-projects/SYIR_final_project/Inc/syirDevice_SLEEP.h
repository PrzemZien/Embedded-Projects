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

#ifndef SYIRDEVICE_SLEEP_H
#define SYIRDEVICE_SLEEP_H

typedef enum {
    SLEEP_NONE              = 0b001,
    SLEEP_IDLE              = 0b000,
    SLEEP_POWER_DOWN        = 0b010,
    SLEEP_POWER_SAVE        = 0b011,
    SLEEP_STANDBY           = 0b110,
    SLEEP_EXTENDED_STANDBY  = 0b111,
} syirDevice_SLEEP_t; 

    extern uint8_t  syirDevice_INIT_sleep(syirDevice_SLEEP_t mode);
    extern void     syirDevice_ENTER_sleep(void);
    extern uint8_t  syirDevice_STATUS_sleep(void);

#endif