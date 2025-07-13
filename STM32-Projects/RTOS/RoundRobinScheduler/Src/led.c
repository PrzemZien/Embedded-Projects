/*
 * led.c
 *
 *  Created on: Jun 30, 2025
 *      Author: przem
 */

/**/
/*------------------------------*/


#include "led.h"
#include "stm32f4xx.h"
#define GPIODEN				(1U<<3) // IO port D clock enabled
#define BLUE_LED_PIN		(1U<<15) // GPIO port mode register
#define GREEN_LED_PIN		(1U<<12) // GPIO port mode register

/*--------------Functions-------------------*/

void led_init(void){
	/* Enables clock access to led port (Port D)*/
	RCC->AHB1ENR |= GPIODEN; // this enables clock access to AHB1 and sets 3 bit to 1
	/* Sets led pin as an output pin*/
	GPIOD->MODER |= (1U<<30); // this set 1 on bit 30 bit in moder15
	GPIOD->MODER &= ~(1U<<31); // this set 0 on bit 31 bit in moder15
	GPIOD->MODER |= (1U<<24);
	GPIOD->MODER &= ~(1U<<25);
}

void turn_blue_led_on(void){
	/* Sets led pin - HIGH (PD15)*/
	GPIOD->ODR |= BLUE_LED_PIN;
}

void turn_blue_led_off(void){
	/* Sets led pin - LOW (PD15)*/
	GPIOD->ODR &= ~BLUE_LED_PIN;
}

void turn_green_led_on(void){
	GPIOD->ODR |= GREEN_LED_PIN;
}
void turn_green_led_off(void){
	GPIOD->ODR &= ~GREEN_LED_PIN;
}
/*-----------------------------------------*/
