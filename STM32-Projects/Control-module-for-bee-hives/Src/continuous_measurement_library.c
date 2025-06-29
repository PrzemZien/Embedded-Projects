/*
 * continuous_measurement_library.c
 *
 *  Created on: Jun 8, 2025
 *      Author: przem
 */
#include "stm32l0xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include "usart.h"
#include "dht11_library.h"
#include "uart_library.h"
#include "spi.h"
#include "oledlib.h"
#include "gpsparce.h"
#include "delay_library.h"

#define COMMUNICATION_BUFFER_LENGTH 128
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

// Funkcje

// Funkcja która obsługuje logikę przełączania stanów ( z C na F )
void logic_for_a_button(
    volatile uint8_t *pressed,
    state_machine_to_change_units *unit_state_machine,
    continous_measurment_machine_state *button_state_machine
){
    static uint8_t button_toggle = 0;
    if (*pressed){
        *pressed = 0;  // wyczyść flagę
        button_toggle ^= 1;  // przełącz stan
        if (button_toggle){
            *unit_state_machine = UNIT_SELECTED_FAHRENHEIT;
            *button_state_machine = STATE_CHANGED;
        }else{
            *unit_state_machine = UNIT_SELECTED_CELSIUS;
            *button_state_machine = STATE_CHANGED;
        }
    }
}

// Funkcja do zmiany danych wyświetlanych na wyświetlaczu w przypadku kiedy wartości temp i humd zmienią się
void data_change(DHT11_sensor *sensor_n, strcture_for_continous_measure *ctx){
    logic_for_a_button(ctx->pressed, ctx->unit_state_machine, ctx->button_state_machine);
    delay(2000);
    measure_data_of_DHT11(sensor_n, DHT11_Connection_GPIO_Port, DHT11_Connection_Pin);
    if ((sensor_n->temperature != *(ctx->previous_temp)) || *(ctx->button_state_machine) == STATE_CHANGED) {
        *(ctx->machine_state_for_continous) = DATA_HAS_CHANGED;
        main_loop_for_measuring_data(*sensor_n);
        *(ctx->previous_temp) = sensor_n->temperature;
        *(ctx->previous_humd) = sensor_n->humidity;
        *(ctx->button_state_machine) = NO_CHANGE;
    } else {
        *(ctx->machine_state_for_continous) = NO_CHANGE;
    }
}
/*Funckja wyglądała wcześnie tak i działa tak samo tylko że dodałem strukturę dlatego tak nieładnie to wygląda
 *
 * void data_change(DHT11_sensor* sensor_n){
	logic_for_a_button();
	delay(2000);
	measure_data_of_DHT11(sensor_n, DHT11_Connection_GPIO_Port, DHT11_Connection_Pin);
	if((sensor_n->temperature != previous_temp) || button_state_machine == STATE_CHANGED){
		machine_state_for_continous = DATA_HAS_CHANGED;
		main_loop_for_measuring_data(*sensor_n);
		previous_temp = sensor_n->temperature;
		previous_humd = sensor_n->humidity;
		button_state_machine = NO_CHANGE;
	}else{
		machine_state_for_continous = NO_CHANGE;
	}
}
 *
 *
 */





/*
void main_loop_for_measuring_data(
		DHT11_sensor sensor_n, message m1,
		char longitude_direction,
		char latitude_direction,
		state_machine_to_change_units unit_state_machine
		){
		char temp_buffor_for_temp[COMMUNICATION_BUFFER_LENGTH];
		char temp_buffor_for_humd[COMMUNICATION_BUFFER_LENGTH];
		char temp_buffor_for_temp_F[COMMUNICATION_BUFFER_LENGTH];
		char buffer_for_longitude[COMMUNICATION_BUFFER_LENGTH];
		char buffer_for_latitude[COMMUNICATION_BUFFER_LENGTH];
		// gps
		clearGDDRAM(&hspi1);
		if(unit_state_machine == UNIT_SELECTED_CELSIUS){
			longitude_direction = getLonDir(&m1);
			latitude_direction = getLatDir(&m1);
			snprintf(temp_buffor_for_temp, sizeof(temp_buffor_for_temp), "%.2f *C \r\n", sensor_n.temperature);
			snprintf(temp_buffor_for_humd, sizeof(temp_buffor_for_humd), "%.2f %% \r\n", sensor_n.humidity);
			snprintf(buffer_for_longitude, sizeof(buffer_for_longitude), "%.2f %c - lon \r\n", m1.gprmc.lon,longitude_direction);
			snprintf(buffer_for_latitude, sizeof(buffer_for_latitude), "%.2f %c - lat \r\n", m1.gprmc.lat,latitude_direction);
			clearBuffer();
			printString(0, 0, temp_buffor_for_temp);
			printString(0, 10, temp_buffor_for_humd);
			printString(0, 20, buffer_for_longitude);
			printString(0, 30, buffer_for_latitude);
			frameTrans(&hspi1);
		}
		else if(unit_state_machine == UNIT_SELECTED_FAHRENHEIT){
			longitude_direction = getLonDir(&m1);
			latitude_direction = getLatDir(&m1);
			snprintf(temp_buffor_for_temp_F, sizeof(temp_buffor_for_temp_F), "%.2f F \r\n", sensor_n.temperature_in_fahrenheits);
			snprintf(temp_buffor_for_humd, sizeof(temp_buffor_for_humd), "%.2f %% \r\n", sensor_n.humidity);
			snprintf(buffer_for_longitude, sizeof(buffer_for_longitude), "%.2f %c - lon \r\n", m1.gprmc.lon,longitude_direction);
			snprintf(buffer_for_latitude, sizeof(buffer_for_latitude), "%.2f %c - lat \r\n", m1.gprmc.lat,latitude_direction);
			clearBuffer();
			printString(0, 0, temp_buffor_for_temp_F);
			printString(0, 10, temp_buffor_for_humd);
			printString(0, 20, buffer_for_longitude);
			printString(0, 30, buffer_for_latitude);
			frameTrans(&hspi1);
		}else{
			snprintf(temp_buffor_for_temp, sizeof(temp_buffor_for_temp), "NO DATA\r\n");
			clearBuffer();
			printString(0, 0, temp_buffor_for_temp);
			frameTrans(&hspi1);
		}
}
*/



