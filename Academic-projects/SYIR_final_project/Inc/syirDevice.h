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

#ifndef SYIR_DEVICE_H
#define SYIR_DEVICE_H

#include <xc.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include "syirDevice_CLOCKS.h"
#include "syirDevice_LEDS.h"
#include "syirDevice_LOG.h"
//#include "syirDevice_QID.h"
#include "syirDevice_VDD.h"
#include "syirDevice_SLEEP.h"
#include "syirDevice_TIME.h"
#include "syirDevice_CPUT.h"
#include "syirDevice_ACC.h"
#include "syirDevice_AMBT.h"


    extern void     syirDevice_OFF_ports             (void);

#endif