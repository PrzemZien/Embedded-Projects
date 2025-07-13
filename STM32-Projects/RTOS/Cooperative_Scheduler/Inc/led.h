/*
 * led.h
 *
 *  Created on: Jun 30, 2025
 *      Author: przem
 */

#ifndef LED_H_
#define LED_H_

// includes
#include "stm32f4xx.h"

// functions
void led_init(void);
void turn_blue_led_on(void);
void turn_blue_led_off(void);
void turn_green_led_on(void);
void turn_green_led_off(void);
#endif /* LED_H_ */
