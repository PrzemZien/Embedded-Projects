/*
*******************************************************************************
  * Autorzy:	Arseniy Sialitski
  *		Przemysław Ziencik
  * Ostatnia aktualizacja: 10.06.2025
  * Opis: Funckja delay
  *
  *
******************************************************************************
*/
#include "stm32l0xx_hal.h"
#include "stdio.h"
#include "uart_library.h"

extern TIM_HandleTypeDef htim7;
void delay(uint16_t us){
      __HAL_TIM_SET_COUNTER(&htim7, 0);  // reset counter
      HAL_TIM_Base_Start(&htim7);
      while (__HAL_TIM_GET_COUNTER(&htim7) < us);
      HAL_TIM_Base_Stop(&htim7);
}


