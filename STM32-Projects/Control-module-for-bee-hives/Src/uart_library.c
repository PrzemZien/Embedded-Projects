/*
*******************************************************************************
  * Autorzy:	Przemysław Ziencik
  * Ostatnia aktualizacja: 10.06.2025
  * Opis: Biblioteka z funkcjami do obsługi komunikacji typu uart
  *
  *
******************************************************************************
*/
#include "stm32l0xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include "usart.h"
#include "dht11_library.h"
#define COMMUNICATION_BUFFER_LENGTH 128
// variables

// state machines

// maszyna stanów dla przechodzenia pomiędzy stanami w trybie pomiaru coninous
typedef enum continous_sending_machine_state{
	INIT_STOP,
	INIT_START,
	INIT_SETUP,
	INIT_READY,
	INIT_DELAY
}continous_sending_machine_state;

// maszyna stanów dla zmiany wartość temperatury i wilgotności
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


// Funkcja bufor cykliczny
// paramtery: flaga dla uarta
void circ_buffer_uart_send_char(cyclic_buffer_for_uart *q, char data, volatile uint8_t uart_tx_in_progress){
	uint16_t next_head = q->head + 1;
	if(next_head == COMMUNICATION_BUFFER_LENGTH){
		next_head = 0;
	}
	if(next_head != q->tail){
		q->buffer[q->head] = data;
		q->head = next_head;
		if(!uart_tx_in_progress){
			uart_tx_in_progress = 1;
			HAL_UART_Transmit_IT(&huart2, (uint8_t*)&q->buffer[q->tail], 1);
		}
	}
}
// Functions:
// Funkcje do wysyłania odpowiedniego typu danych przez uart
// char *s -> wskaźnik na string który wyślemy, bufor cykliczny, flaga dla uarta
void uart_send_string(const char *s, cyclic_buffer_for_uart *uart_cyclic_buffer, volatile uint8_t uart_tx_in_progress){
	while(*s){
		circ_buffer_uart_send_char(uart_cyclic_buffer, *s++, uart_tx_in_progress);
	}
}
// funkcja pozwalająca wysłać poprzez uart float
// paramtery: float które wyślemy, bufor cykliczny poprzez który wyślemy, flaga dla uarta
void uart_send_float(float f, cyclic_buffer_for_uart *uart_cyclic_buffer, volatile uint8_t *uart_tx_in_progress){
    char buf[COMMUNICATION_BUFFER_LENGTH];
    snprintf(buf, sizeof(buf), " %.2f\r\n", f);
    uart_send_string(buf, uart_cyclic_buffer, uart_tx_in_progress);
}
// Funkcje do continous measurment
// parametry funkcji: sensor_n -> czujnik którego użyjemy, bufor cykliczny, flaga do uarta
void continous_measurment_at_current_place(DHT11_sensor sensor_n, cyclic_buffer_for_uart *uart_cyclic_buffer,volatile uint8_t *uart_tx_in_progress,uint16_t delay){
    uart_text_command("temp - temperature\n", uart_cyclic_buffer, uart_tx_in_progress);
    uart_send_float((float)sensor_n.temperature, uart_cyclic_buffer, uart_tx_in_progress);
    uart_text_command("humd - humidity\n", uart_cyclic_buffer, uart_tx_in_progress);
    uart_send_float((float)sensor_n.humidity, uart_cyclic_buffer, uart_tx_in_progress);
    HAL_Delay(delay);
}

// funkcja do odbierania danych z przerwania
void communication_module(char character, uint8_t *command_ready, char *command_buffer, uint16_t *number_of_characters, char communication_buffor[]){
    if(character == '\n' || character == '\r'){
        if(*number_of_characters > 0){
            communication_buffor[*number_of_characters] = '\0';
            strncpy(command_buffer, communication_buffor, COMMUNICATION_BUFFER_LENGTH);
            *command_ready = 1;
            *number_of_characters = 0;
        }
    } else {
        if(*number_of_characters < COMMUNICATION_BUFFER_LENGTH - 1){
            communication_buffor[(*number_of_characters)++] = character;
        }else{
            *number_of_characters = 0;
        }
    }
}



