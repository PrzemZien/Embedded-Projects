/*
 * continuous_measurement_library.h
 *
 *  Created on: Jun 8, 2025
 *      Author: przem
 */

#ifndef INC_CONTINUOUS_MEASUREMENT_LIBRARY_H_
#define INC_CONTINUOUS_MEASUREMENT_LIBRARY_H_
#include "stm32l0xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include "usart.h"
#include "dht11_library.h"
#include "uart_library.h"
#include "spi.h"
#include "oledlib.h"
#include "delay_library.h"
// Struktury
typedef struct{
    float *previous_temp;
    float *previous_humd;
    uint8_t *pressed;
    state_machine_to_change_units *unit_state_machine;
    continous_measurment_machine_state *button_state_machine;
    continous_measurment_machine_state *machine_state_for_continous;
    message *message;
} strcture_for_continous_measure;

// Functions
void logic_for_a_button(
    volatile uint8_t *pressed,
    state_machine_to_change_units *unit_state_machine,
    continous_measurment_machine_state *button_state_machine
);
void data_change(DHT11_sensor *sensor_n, strcture_for_continous_measure *ctx);
//void main_loop_for_measuring_data( DHT11_sensor sensor_n
//);
#endif /* INC_CONTINUOUS_MEASUREMENT_LIBRARY_H_ */
