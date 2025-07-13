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

void osKernelInit(void);
uint8_t osKernelAddThread(void(*task0)(void),void(*task1)(void),void(*task2)(void));
void osKernelLaunch(uint32_t quanta);
void osThreadYield(void);

#endif /* OSKERNEL_H_ */
