/*
 * uart_library.h
 *
 *  Created on: Jun 8, 2025
 *      Author: przem
 */

#ifndef INC_UART_LIBRARY_H_
#define INC_UART_LIBRARY_H_

#include "stm32l0xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include "dht11_library.h"
// defines
#define COMMUNICATION_BUFFER_LENGTH 128


// state machines
typedef enum continous_sending_machine_state{
	INIT_STOP,
	INIT_START,
	INIT_SETUP,
	INIT_READY,
	INIT_DELAY
}continous_sending_machine_state;

typedef enum continous_measurment_machine_state{
	DATA_HAS_CHANGED,
	NO_CHANGE,
	STATE_CHANGED
}continous_measurment_machine_state;

// maszyna stanów do przycisku, działa jak flaga dla stanów 0 i 1 przycisku
typedef enum state_machine_to_change_units{
	UNIT_SELECTED_FAHRENHEIT,
	UNIT_SELECTED_CELSIUS
}state_machine_to_change_units;

typedef enum state_machine_for_gps_uart_connection{
	GPS_START_MEASURE,
	GPS_MEASURED,
	GPS_STOP_MEASURING
}state_machine_for_gps_uart_connection;

typedef struct{
	char* const buffer;
	uint8_t head;
	uint8_t tail;
}cyclic_buffer_for_uart;

// functions
void circ_buffer_uart_send_char(cyclic_buffer_for_uart *q, char data, volatile uint8_t uart_tx_in_progress);
void uart_send_string(const char *s, cyclic_buffer_for_uart *uart_cyclic_buffer, volatile uint8_t uart_tx_in_progress);
void uart_send_float(float f, cyclic_buffer_for_uart *uart_cyclic_buffer, volatile uint8_t *uart_tx_in_progress);
void continous_measurment_at_current_place(DHT11_sensor sensor_n, cyclic_buffer_for_uart *uart_cyclic_buffer,volatile uint8_t *uart_tx_in_progress,uint16_t delay);
void communication_module(char character, uint8_t *command_ready, char *command_buffer, uint16_t *number_of_characters, char communication_buffor[]);
#endif /* INC_UART_LIBRARY_H_ */
