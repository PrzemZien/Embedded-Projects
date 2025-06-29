/*
*******************************************************************************
  * Autorzy:	Arseniy Sialitski
  *		Przemysław Ziencik
  * Ostatnia aktualizacja: 10.06.2025
  * Opis: Biblioteka z funkcjami do obsługi czujnika DHT11
  *
  *
******************************************************************************
*/
#include "stm32l0xx_hal.h"
#include "stdio.h"
#include "delay_library.h"
#define SENSOR_NAME_LENGTH 40
//-----------------------------------------------------------------------------------------------------------
/* You need to write: “htimn” ( n - the number of the timer you are using) -> TIM_HandleTypeDef time htimn */


//-----------------------------------------------------------------------------------------------------------

typedef struct DHT11_sensor{
	float temperature;
	float humidity;
	uint8_t  integral_RH_data, decimal_RH_data, integral_T_data, decimal_T_data; // z datasheeta zmienne które odbieram -> kazda jest po 8 bitów
	uint16_t RH_data_sum, T_data_sum, sum_data;
	char sensor_name[40];
	float temperature_in_fahrenheits;
	GPIO_TypeDef* sensor_GPIOx;
	uint16_t sensor_GPIO_PIN;
}DHT11_sensor;

// struktura do inicjalizacji sensora

typedef struct{
	const char * sensor_name;
	DHT11_sensor* sensor;
}sensor_init_data;


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
  // funkcja do sprawdzania odpowiedzi czujnika
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

  // funkcja zbierajaca dane w przypadku polaczenia z czujnikiem
  uint8_t DHT11_Data(GPIO_TypeDef* GPIOx, uint16_t GPIO_PIN){
  	uint8_t Data = 0,i;
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
  // funkcja która uruchamia pomiar i wszytskie pozostałe funkcje które znajdują się wyżej
  // parametry funkcji to: sensor -> czyli cała struktura która znajduje się na początku tej biblioteki
  // GPIOx i GPIO_Pin -> są to piny do których podłączony jest czujnik
  void measure_data_of_DHT11(DHT11_sensor *sensor_n, GPIO_TypeDef* GPIOx, uint16_t GPIO_PIN){
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
	  sensor_n->temperature_in_fahrenheits = (sensor_n->temperature * (9.0f/5.0f)) + 32.0f;
  }
  // Nie jest to funkcja obowiązkowa w przypadku braku komunikacji UART
  // Nadaje nazwe czujnikowi, nazwa musi być typu: sensor_1, sensor_23 itd..
  void name_for_sensor(DHT11_sensor *sensor_n, const char* name_for_sensor){
      strncpy(sensor_n->sensor_name, name_for_sensor, sizeof(sensor_n->sensor_name) - 1);
      sensor_n->sensor_name[sizeof(sensor_n->sensor_name) - 1] = '\0'; // zawsze zakończ napisem null
  }


  DHT11_sensor* check_sensor_name(char* entered_sensor_name, sensor_init_data* sensors, int number_of_sensors){
      for(int i = 0; i < number_of_sensors; i++){
          if(sensors[i].sensor_name != NULL && strcmp(entered_sensor_name, sensors[i].sensor_name) == 0 ){
          	return sensors[i].sensor;
          }
      }
      return NULL;
  }




