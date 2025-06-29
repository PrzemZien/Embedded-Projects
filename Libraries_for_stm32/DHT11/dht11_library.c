/*
 * dht11_library.c
 *
 *  Created on: May 12, 2025
 *      Author: przem
 */
#include "stm32l0xx_hal.h"
#include "stdio.h"

//-----------------------------------------------------------------------------------------------------------
/* You need to write: “htimn” ( n - the number of the timer you are using) -> TIM_HandleTypeDef time htimn */

extern TIM_HandleTypeDef htim7;

//-----------------------------------------------------------------------------------------------------------

enum DHT11_states{
	CHECK1, CHECK2, CHECK3,ERROR1,ERROR2
};

typedef struct DHT11_sensor{
	float temperature;
	float humidity;
	uint8_t  integral_RH_data, decimal_RH_data, integral_T_data, decimal_T_data; // from datasheet
	uint16_t RH_data_sum, T_data_sum, sum_data;
	enum DHT11_states state_of_sensor;
} DHT11_sensor;


void delay(uint16_t us){
      __HAL_TIM_SET_COUNTER(&htim7, 0);  // reset counter
      HAL_TIM_Base_Start(&htim7);
      while (__HAL_TIM_GET_COUNTER(&htim7) < us);
      HAL_TIM_Base_Stop(&htim7);
  }

  // FUnkcje przelaczania pinu (gpio)
  void Set_Pin_as_Output(GPIO_TypeDef* GPIOx, uint16_t GPIO_PIN){
  	GPIO_InitTypeDef GPIO_Pin_struct = {0};
  	GPIO_Pin_struct.Pin = GPIO_PIN;
  	GPIO_Pin_struct.Mode = GPIO_MODE_OUTPUT_PP; // GPIO Mode
  	GPIO_Pin_struct.Speed = GPIO_SPEED_LOW; // Maxiumum Output Speed
  	HAL_GPIO_Init(GPIOx, &GPIO_Pin_struct);
  }

  void Set_Pin_as_Input(GPIO_TypeDef* GPIOx, uint16_t GPIO_PIN){
  	GPIO_InitTypeDef GPIO_Pin_struct = {0};
  	GPIO_Pin_struct.Pin = GPIO_PIN;
  	GPIO_Pin_struct.Mode = GPIO_MODE_INPUT; // GPIO Mode
  	GPIO_Pin_struct.Pull = GPIO_NOPULL; // GPIO Pull-up/Pull-down
  	HAL_GPIO_Init(GPIOx, &GPIO_Pin_struct);
  }
  // funkcje do DHT11
  void DHT11_Start_Signal(GPIO_TypeDef* GPIOx, uint16_t GPIO_PIN){
  	Set_Pin_as_Output(GPIOx, GPIO_PIN);
  	HAL_GPIO_WritePin(GPIOx, GPIO_PIN, 0);
  	delay(18000); // T = 1 us => ( We need 18 ms)
  	HAL_GPIO_WritePin(GPIOx,GPIO_PIN, 1);
  	delay(20);
  	Set_Pin_as_Input(GPIOx, GPIO_PIN); // DHT sends out response signal ...
  }

  void DHT11_Response(DHT11_sensor *sensor_n,GPIO_TypeDef* GPIOx, uint16_t GPIO_PIN){
  	int Response = 0;
  	delay(40);
  	if(!(HAL_GPIO_ReadPin(GPIOx, GPIO_PIN))){ // it should be 0 at this point
  		Response = 1;
  		delay(80);
  		if(HAL_GPIO_ReadPin(GPIOx, GPIO_PIN)){
  			Response = 1;
  		}
  		else{
  			Response = 0;
  			printf("DHT11 nie odbiera: %d\n",Response);
  		}
  	}
  	while(HAL_GPIO_ReadPin(GPIOx, GPIO_PIN)); // it should be 0 at this point
  }


  uint8_t DHT11_Data(GPIO_TypeDef* GPIOx, uint16_t GPIO_PIN){
  	uint8_t Data,i;
  	for(i=0;i<8;i++){ // its 8 bits
  			while(!(HAL_GPIO_ReadPin(GPIOx, GPIO_PIN))); // wait for 1 (typical 50 us)
  			delay(40);
  			if(!(HAL_GPIO_ReadPin(GPIOx, GPIO_PIN))){
  				Data &= ~(1 << (7 - i));
  			}
  			else{
  				Data |= (1 << (7 - i)); //
  			}
  			while((HAL_GPIO_ReadPin(GPIOx, GPIO_PIN)));
  	}
  	return Data;
  }

  void initialization_of_DHT11(DHT11_sensor *sensor_n, GPIO_TypeDef* GPIOx, uint16_t GPIO_PIN){
	  DHT11_Start_Signal(GPIOx,GPIO_PIN);
	  DHT11_Response(sensor_n,GPIOx,GPIO_PIN);
	  sensor_n->integral_RH_data = DHT11_Data(GPIOx,GPIO_PIN);
	  sensor_n->decimal_RH_data = DHT11_Data(GPIOx,GPIO_PIN);
	  sensor_n->integral_T_data = DHT11_Data(GPIOx,GPIO_PIN);
	  sensor_n->decimal_T_data = DHT11_Data(GPIOx,GPIO_PIN);
	  sensor_n->sum_data = DHT11_Data(GPIOx,GPIO_PIN);
	  sensor_n->T_data_sum = sensor_n->integral_T_data;
	  sensor_n->RH_data_sum = sensor_n->integral_RH_data;
	  sensor_n->temperature = (float) sensor_n->T_data_sum;
	  sensor_n->humidity = (float) sensor_n->RH_data_sum;
  }

