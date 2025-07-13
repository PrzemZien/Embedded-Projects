/*
 * timebase.h
 *
 *  Created on: Jul 4, 2025
 *      Author: przem
 */

#ifndef TIMEBASE_H_
#define TIMEBASE_H_

#include "stdint.h"

uint32_t get_tick(void);
void Delay_s(uint32_t delay_us);
void Delay_us(uint32_t delay_us);
void Delay_ms(uint32_t delay_ms);
void timebase_init(void);

#endif /* TIMEBASE_H_ */
