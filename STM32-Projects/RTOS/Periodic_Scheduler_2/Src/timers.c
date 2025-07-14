/*
 * timers.c
 *
 *  Created on: Jul 13, 2025
 *      Author: przem
 */
#include "stm32f4xx.h"
#include "stdint.h"

#define TIM_2_EN			(1U<<0)
#define CR1_CEN				(1U<<0)
#define DIER_UIE			(1U<<0)
#define SR_UIF				(1U<<0)

void timer2_init(void){

	/*Enable clock access to tim2*/
	// str 114 rm

	RCC->APB1ENR |= TIM_2_EN;

	/*Set timer prescaler*/
	TIM2->PSC = 1600 - 1;

	/*Set auto-reload value*/
	TIM2->ARR = 10000 - 1;

	/*Clear counter*/
	TIM2->CNT = 0;

	/*Enable timer*/
	TIM2->CR1 = CR1_CEN;

	/*Enable timer interrupt*/
	TIM2->DIER |= DIER_UIE;

	/*Enable timer interrupt in NVIC*/
	NVIC_EnableIRQ(TIM2_IRQn);
}
