/*
 * dht11_library.h
 *
 *  Created on: May 12, 2025
 *      Author: przem
 */

#ifndef SRC_DHT11_LIBRARY_H_
#define SRC_DHT11_LIBRARY_H_

#include "stm32l0xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include "delay_library.h"

typedef struct DHT11_sensor{
	float temperature;
	float humidity;
	uint8_t  integral_RH_data, decimal_RH_data, integral_T_data, decimal_T_data; // from datasheet
	uint16_t RH_data_sum, T_data_sum, sum_data;
	char sensor_name[40];
	float temperature_in_fahrenheits;
	GPIO_TypeDef* sensor_GPIOx;
	uint16_t sensor_GPIO_PIN;
} DHT11_sensor;

typedef struct{
	const char * sensor_name;
	DHT11_sensor* sensor;
}sensor_init_data;

void Set_Pin_as_Output(GPIO_TypeDef* GPIOx, uint16_t GPIO_PIN);
void Set_Pin_as_Input(GPIO_TypeDef* GPIOx, uint16_t GPIO_PIN);
void DHT11_Start_Signal(GPIO_TypeDef* GPIOx, uint16_t GPIO_PIN);
void DHT11_Response(DHT11_sensor *sensor_n,GPIO_TypeDef* GPIOx, uint16_t GPIO_PIN);
uint8_t DHT11_Data(GPIO_TypeDef* GPIOx, uint16_t GPIO_PIN);
void measure_data_of_DHT11(DHT11_sensor *sensor_n, GPIO_TypeDef* GPIOx, uint16_t GPIO_PIN);
DHT11_sensor* check_sensor_name(char* entered_sensor_name, sensor_init_data* sensors, int number_of_sensors);
#endif /* SRC_DHT11_LIBRARY_H_ */
