/*
 * osKernel.h
 *
 *  Created on: Jul 10, 2025
 *      Author: przem
 */

#ifndef OSKERNEL_H_
#define OSKERNEL_H_
#include "stm32f4xx.h"
#include "stdint.h"

#define PERIOD_VALUE_FOR_T3			100

void osKernelInit(void);
uint8_t osKernelAddThread(void(*task0)(void),void(*task1)(void),void(*task2)(void));
void osKernelLaunch(uint32_t quanta);
void osThreadYield(void);
void task3(void);

void osSemaphore_init(int32_t *semaphore, int32_t value);
void osSemaphoreSet(int32_t *semaphore);
void osSemaphoreWait(int32_t *semaphore);

#endif /* OSKERNEL_H_ */
