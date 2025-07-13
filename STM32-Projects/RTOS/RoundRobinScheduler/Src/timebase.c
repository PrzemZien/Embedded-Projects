/*
 * timebase.c
 *
 *  Created on: Jul 4, 2025
 *      Author: przem
 */

#include "timebase.h"
#include "stm32f4xx.h"

#define SYST_CSR_COUNTFLAG			(1U<<16)
#define SYST_CSR_CLK_SOURCE			(1U<<2)
#define SYST_CSR_TICKINT			(1U<<1)
#define SYST_CSR_CLK_ENABLE			(1U<<0)

#define DWT_COREDEBUG_TRCENA_EN		(1U<<24)
#define DWT_CTRL_CYCCT_ENABLE		(1U<<0)

#define ONE_SEC_LOAD				16000000
#define MAX_DELAY_VALUE				0xFFFFFFFFU

volatile uint32_t current_tick;
volatile uint32_t current_tick_freq = 1;
volatile uint32_t current_tick_p;

/*--------------Functions-----------------*/

void timebase_init(void){

	/*Reloads the timer with number of cyclers for per second*/
	SysTick->LOAD = ONE_SEC_LOAD - 1;

	/*Clears Systick current value register*/
	SysTick->VAL = 0;

	/*Selects internal clock source*/
	SysTick->CTRL = SYST_CSR_CLK_SOURCE;

	/*Enables interrupt*/
	SysTick->CTRL |= SYST_CSR_TICKINT;

	/*Enables systick*/
	SysTick->CTRL |= SYST_CSR_CLK_ENABLE;

	/*Enables DWT*/
	CoreDebug->DEMCR |= DWT_COREDEBUG_TRCENA_EN;

	/* Clears CYCCNT counter */
	DWT->CYCCNT = 0;

	/*Enables counter*/
	DWT->CTRL |= DWT_CTRL_CYCCT_ENABLE;

	__enable_irq();

}

static void tick_increment(void){
	current_tick += current_tick_freq;
}

uint32_t get_tick(void){
	__disable_irq(); // turns off global interrupts
	current_tick_p = current_tick;
	__enable_irq(); // turns on global interrupts
	return current_tick_p;
}

static void SysTick_Handler(void){
	tick_increment();
}

void Delay_s(uint32_t delay_s){
	uint32_t tick_start_value = get_tick();
	uint32_t wait = delay_s;
	if(wait < MAX_DELAY_VALUE){
		wait+=(uint32_t)(current_tick_freq);
	}
	while((get_tick() - tick_start_value) < wait){}
}

void Delay_ms(uint32_t delay_ms){
	uint32_t tick_start_value_ms = DWT->CYCCNT;
	uint32_t tick_value = (ONE_SEC_LOAD / 1000 ) * delay_ms;
	while((DWT->CYCCNT - tick_start_value_ms) < tick_value){}
}

void Delay_us(uint32_t delay_us){
	uint32_t tick_start_value_us = DWT->CYCCNT;
	uint32_t tick_value = (ONE_SEC_LOAD / 1000000 ) * delay_us;
	while((DWT->CYCCNT - tick_start_value_us) < tick_value){}
}

/*-----------------------------------------*/

