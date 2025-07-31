/*
 * dht11_library.h
 *
 *  Created on: Jul 8, 2025
 *      Author: przem
 */

#ifndef DHT11_LIBRARY_H_
#define DHT11_LIBRARY_H_


typedef struct DHT11_sensor{
	float temperature;
	float humidity;
	uint8_t  integral_RH_data, decimal_RH_data, integral_T_data, decimal_T_data; // from datasheet
	uint16_t RH_data_sum, T_data_sum, sum_data;
} DHT11_sensor;

void initialization_of_DHT11(DHT11_sensor *sensor_n);

#endif /* DHT11_LIBRARY_H_ */
