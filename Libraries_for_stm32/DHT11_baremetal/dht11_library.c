/*
 * dht11_library.c
 *
 *  Created on: Jul 8, 2025
 *      Author: przem
 */
#include <stdint.h>
#include "stm32f4xx.h"
#include "stdio.h"
#include "timebase.h"

#define GPIOEEN				(1U<<4)
#define PE2_PIN				(1U<<2)

typedef enum{
		PIN_SET_AS_INPUT,
		PIN_SET_AS_OUTPUT
}Pin_state;

Pin_state pin_status;

/*--------------Functions-------------------*/

void PE2_Pin_init(void){
	RCC->AHB1ENR |= GPIOEEN;
}

void PE2_Pin_set_as_input(void){
	PE2_Pin_init();

	/* Sets PE2 as an input */
	GPIOE->MODER &= ~(1U<<5);
	GPIOE->MODER &= ~(1U<<4);

	/* Sets on PE2 -> No-Pull-Up */
	GPIOE->PUPDR &= ~(1U<<5);
	GPIOE->PUPDR &= ~(1U<<4);

	pin_status = PIN_SET_AS_INPUT;

}

void PE2_Pin_set_as_output(void){
	PE2_Pin_init();

	/* Sets PE2 as an output */
	GPIOE->MODER |= (1U<<5);
	GPIOE->MODER &= ~(1U<<4);

	/* Sets PE2 -> LOW SPEED */
	GPIOE->OSPEEDR &= ~(1U<<4);
	GPIOE->OSPEEDR &= ~(1U<<5);

	/* Sets PE2 -> Output PP */
	GPIOE->OTYPER  &= ~(1U<<2);

	pin_status = PIN_SET_AS_OUTPUT;
}

void PE2_Write_Pin(uint32_t state){
	if(pin_status == PIN_SET_AS_OUTPUT){
		if(state == 1){
			GPIOE->ODR |= PE2_PIN;
		}else if(state == 0){
			GPIOE->ODR &= ~PE2_PIN;
		}
	}
}

uint8_t PE2_Read_Pin(void){
	if(pin_status == PIN_SET_AS_INPUT){
		if(GPIOE->IDR & PE2_PIN){
			return 1;
		}else{
			return 0;
		}
	}else{
		return 0xFF;
	}
}


typedef struct DHT11_sensor{
	float temperature;
	float humidity;
	uint8_t  integral_RH_data, decimal_RH_data, integral_T_data, decimal_T_data; // from datasheet
	uint16_t RH_data_sum, T_data_sum, sum_data;
} DHT11_sensor;


static void DHT11_Start_Signal(void){
	PE2_Pin_init();
	PE2_Pin_set_as_output();
	PE2_Write_Pin(0);
	Delay_ms(18);
	PE2_Write_Pin(1);
	Delay_us(20);
	PE2_Pin_set_as_input();
}

static void DHT11_Response(void){
  	Delay_us(40);
  	if(!(PE2_Read_Pin())){ // it should be 0 at this point
  		Delay_us(80);
  	}
  	while(PE2_Read_Pin()); // it should be 0 at this point
}


static uint8_t DHT11_Data(void){
  	uint8_t Data = 0,i;
  	for(i=0;i<8;i++){ // its 8 bits
  			while(!PE2_Read_Pin()); // wait for 1 (typical 50 us)
  			Delay_us(40);
  			if(!(PE2_Read_Pin())){
  				Data &= ~(1 << (7 - i));
  			}
  			else{
  				Data |= (1 << (7 - i)); //
  			}
  			while((PE2_Read_Pin()));
  	}
  	return Data;
}

void initialization_of_DHT11(DHT11_sensor *sensor_n){
	  DHT11_Start_Signal();
	  DHT11_Response();
	  sensor_n->integral_RH_data = DHT11_Data();
	  sensor_n->decimal_RH_data = DHT11_Data();
	  sensor_n->integral_T_data = DHT11_Data();
	  sensor_n->decimal_T_data = DHT11_Data();
	  sensor_n->sum_data = DHT11_Data();
	  sensor_n->T_data_sum = sensor_n->integral_T_data;
	  sensor_n->RH_data_sum = sensor_n->integral_RH_data;
	  sensor_n->temperature = (float) sensor_n->T_data_sum;
	  sensor_n->humidity = (float) sensor_n->RH_data_sum;
}

/*-----------------------------------------*/

