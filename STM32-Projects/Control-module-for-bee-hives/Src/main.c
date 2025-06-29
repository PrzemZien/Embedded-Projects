/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/*
*******************************************************************************
  * Autorzy:	Arseniy Sialitski
  *		Przemysław Ziencik
  * Ostatnia aktualizacja: 10.06.2025
  * Opis: Main naszego programu
  *
  *
******************************************************************************
*/


/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "dht11_library.h"
#include "oledlib.h"
#include "gpsparce.h"
#include "uart_library.h"
#include "continuous_measurement_library.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define COMMUNICATION_BUFFER_LENGTH 128																							         //--------------------------------------//
#define SENSOR_NAME_LENGTH 40
#define SENSOR_NAME "sensor_1" // czujnik ktorego aktualnie uzywamy
#define NUMBER_OF_SENSORS 1
#define GPS_BUFFER_LENGTH 300

DHT11_sensor* active_sensor;
DHT11_sensor* active_sensor_for_single_measure;

const char* sensors_names[NUMBER_OF_SENSORS] = {[0] = SENSOR_NAME}; // czujnik nalezy dodac do listy i odpowiednio zmienic NUMBER_OF_SENSORS

DHT11_sensor sensor_1 = {
		.sensor_GPIOx = DHT11_Connection_GPIO_Port,
		.sensor_GPIO_PIN = DHT11_Connection_Pin,
		.sensor_name = SENSOR_NAME
};


// Tutaj umieszczamy aktywne sensor -> dzieki temu w przypadku continous measurment jesteśmy w stanie wybrać sensor
sensor_init_data available_sensors[] = {
    {SENSOR_NAME, &sensor_1},
};

// maszyna stanów dla gps
state_machine_for_gps_uart_connection gps_state_machine;
// state machines
continous_sending_machine_state machine_state_init_status;

// GPS
uint8_t rx_buf[GPS_BUFFER_LENGTH] = {0};
char gps_line_buffer[COMMUNICATION_BUFFER_LENGTH];

// UART
volatile uint8_t znak;
char communication_buffor[COMMUNICATION_BUFFER_LENGTH];
uint16_t number_of_characters = 0;

// DHT11
char* matched_sensor;
uint32_t delay_ms = 2000;

// Debauncer

volatile uint8_t counter = 0;
volatile uint32_t tick = 0;

// Continous measure
message m1;
char longitude_direction;
char latitude_direction;
float previous_temp = 0.0f;
float previous_humd = 0.0f;
volatile uint8_t pressed = 0;
continous_measurment_machine_state button_state_machine;
continous_measurment_machine_state machine_state_for_continous;
state_machine_to_change_units unit_state_machine;
strcture_for_continous_measure struct_for_con_mea ={
		.previous_humd = &previous_humd,
		.previous_temp = &previous_temp,
		.button_state_machine = &button_state_machine,
		.machine_state_for_continous = &machine_state_for_continous,
		.unit_state_machine = &unit_state_machine,
		.pressed = &pressed,
		.message = &m1,
};

char uart_buffer[COMMUNICATION_BUFFER_LENGTH];
cyclic_buffer_for_uart uart_cyclic_buffer = {uart_buffer,0,0};

// uart
volatile uint8_t uart_tx_in_progress = 0;
volatile uint8_t uart_rx_queue[64];
volatile uint8_t uart_rx_head = 0, uart_rx_tail = 0;

// wysylanie samego tekstu
void uart_text_command(char* s)
{
    uart_send_string(s, &uart_cyclic_buffer, uart_tx_in_progress);
}
// comuncation module

// communication commands - pozwala nam na niezalezne odczyt danych mierzonych przez czujnik - to fix
uint8_t communication_module_for_dht_11(DHT11_sensor* sensor_n, const char* sensor_name, GPIO_TypeDef* GPIOx, uint16_t GPIO_PIN, const char* command) {
    measure_data_of_DHT11(sensor_n, GPIOx, GPIO_PIN);

    char expected_command[SENSOR_NAME_LENGTH + 16] = {0};

    snprintf(expected_command, sizeof(expected_command), "%s.temp", sensor_name);
    if (strcmp(command, expected_command) == 0) {
        uart_text_command("Pomiar temperatury... \n\r");
        uart_send_float((float)sensor_n->temperature, &uart_cyclic_buffer, &uart_tx_in_progress);
        return 1;
    }

    snprintf(expected_command, sizeof(expected_command), "%s.humd", sensor_name);
    if (strcmp(command, expected_command) == 0) {
        uart_text_command("Pomiar wilgoci... \n\r");
        uart_send_float((float)sensor_n->humidity, &uart_cyclic_buffer, &uart_tx_in_progress);
        return 1;
    }

    snprintf(expected_command, sizeof(expected_command), "%s.RH_data_sum", sensor_name);
    if (strcmp(command, expected_command) == 0) {
        uart_text_command("Wykonuje pomiar...\n\r");
        uart_send_float((float)sensor_n->RH_data_sum, &uart_cyclic_buffer, &uart_tx_in_progress);
        return 1;
    }

    snprintf(expected_command, sizeof(expected_command), "%s.T_data_sum", sensor_name);
    if (strcmp(command, expected_command) == 0) {
        uart_text_command("Wykonuje pomiar...\n\r");
        uart_send_float((float)sensor_n->T_data_sum, &uart_cyclic_buffer, &uart_tx_in_progress);
        return 1;
    }

    snprintf(expected_command, sizeof(expected_command), "%s.temp_f", sensor_name);
    if (strcmp(command, expected_command) == 0) {
        uart_text_command("Wykonuje pomiar...\n\r");
        uart_send_float((float)sensor_n->temperature_in_fahrenheits, &uart_cyclic_buffer, &uart_tx_in_progress);
        return 1;
    }

    return 0;
}

void commands(const char* command)
{
    // Komendy ogólne, działające niezależnie od stanu maszyny
    if(strcmp(command, "ld2_off") == 0){
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);
        uart_text_command("Dioda LD2 OFF\n\r");
    }
    else if(strcmp(command, "info") == 0){
        uart_text_command("Available commands:\n\r");
        uart_text_command("dht11.info - commands for DHT11 sensors\n\r");
        uart_text_command("loca - (location)\n\r");
        uart_text_command("temp - temperature\n\r");
        uart_text_command("humd - humidity\n\r");
        uart_text_command("start - start continuous data sending\n\r");
        uart_text_command("stop - stop continuous data sending\n\r");
        uart_text_command("gps.info - info about gps commands\n\r");
    }
    else if(strcmp(command, "gps.lat") == 0){
        uart_text_command("Sprawdzanie lokalizacji (latitude)...\n\r");
        uart_send_float((float) m1.gprmc.lat, &uart_cyclic_buffer, &uart_tx_in_progress);
    }
    else if(strcmp(command, "gps.lon") == 0){
        uart_text_command("Sprawdzanie lokalizacji (longitude)...\n\r");
        uart_send_float((float) m1.gprmc.lon, &uart_cyclic_buffer, &uart_tx_in_progress);
    }
    else if(strcmp(command, "gps.time") == 0){
        uart_text_command("Sprawdzanie czasu GPS...\n\r");
        uart_send_float((float) m1.gprmc.time + 20000, &uart_cyclic_buffer, &uart_tx_in_progress);
    }
    else if(strcmp(command, "temp") == 0){
        uart_text_command("Sprawdzanie temperatury...\n\r");
        uart_send_float((float) sensor_1.temperature, &uart_cyclic_buffer, &uart_tx_in_progress);
    }
    else if(strcmp(command, "temp_f") == 0){
        uart_text_command("Sprawdzanie temperatury...\n\r");
        uart_send_float((float) sensor_1.temperature_in_fahrenheits, &uart_cyclic_buffer, &uart_tx_in_progress);
    }
    else if(strcmp(command, "humd") == 0){
        uart_text_command("Sprawdzanie wilgotnosci...\n\r");
        uart_send_float((float) sensor_1.humidity, &uart_cyclic_buffer, &uart_tx_in_progress);
    }
    else if(strcmp(command, "location") == 0){
        uart_text_command("Sprawdzanie lokalizacji...\n\r");
        char temp_buffor_for_sending_location[COMMUNICATION_BUFFER_LENGTH];
        snprintf(temp_buffor_for_sending_location, sizeof(temp_buffor_for_sending_location),
                 " %.2f %c %.2f %c \n\r", m1.gprmc.lat, latitude_direction,
                 m1.gprmc.lon, longitude_direction);
        uart_send_string(temp_buffor_for_sending_location, &uart_cyclic_buffer, uart_tx_in_progress);
    }
    else if(strcmp(command, "gps.info") == 0){
        uart_text_command("Komendy GPS:\n\r");
        uart_text_command("gps.lat -> latitude\n\r");
        uart_text_command("gps.lon -> longitude\n\r");
        uart_text_command("gps.time -> time\n\r");
    }
    else if((strcmp(command, "start") == 0 || strcmp(command, "stop") == 0) &&
       machine_state_init_status != INIT_READY && machine_state_init_status != INIT_START){
        uart_text_command("Błąd: najpierw rozpocznij pomiar (pomiar_cont)\n\r");
        return;  // wychodzimy z funkcji - dalsza obsługa komendy nie ma sensu
    }
    // Tryb INIT_SETUP: wybór czujnika
    else if(machine_state_init_status == INIT_SETUP){
        DHT11_sensor* matched_sensor = check_sensor_name(command, available_sensors, NUMBER_OF_SENSORS);
        if(matched_sensor != NULL){
            active_sensor = matched_sensor;
            uart_text_command("Czujnik zostal wybrany poprawnie\n\r");
            machine_state_init_status = INIT_DELAY;
        	uart_text_command("Podaj delay:\n\r");
        } else {
            uart_text_command("Czujnik zostal wybrany niepoprawnie\n\r");
        }
    }
    // Tryb INIT_DELAY: wybór opóźnienia
    else if(machine_state_init_status == INIT_DELAY){
        uint8_t is_digit_flag = 1;
        for(int i = 0; i < strlen(command); i++){
            if(!isdigit(command[i])){
                is_digit_flag = 0;yyy
                break;
            }
        }
        if(is_digit_flag){////////////////////// sprawdzamy czy jest to liczba do fixa
            delay_ms = atoi(command);
            uart_text_command("Delay został ustawiony poprawnie\n\r");
            machine_state_init_status = INIT_READY;
        } else {
            uart_text_command("Nieprawidłowy delay\n\r");
        }
    }
    // Tryb INIT_READY: startuje pomiar ciagly
    else if(machine_state_init_status == INIT_READY){
        if(strcmp(command, "start") == 0){
            uart_text_command("Włączenie pomiarów...\n\r");
            if(active_sensor != NULL){  // sprawdź, czy sensor jest wybrany
                machine_state_init_status = INIT_START;
            } else {
                uart_text_command("Błąd: nie wybrano czujnika\n\r");
            }
        }
    }
    else if(machine_state_init_status == INIT_START){
        if(strcmp(command, "stop") == 0){
            uart_text_command("Wyłączenie pomiarów...\n\r");
            machine_state_init_status = INIT_STOP;
        }
    }
    // Rozpoczęcie całego procesu pomiaru
    else if(strcmp(command, "pomiar_cont") == 0){
        machine_state_init_status = INIT_SETUP;
        uart_text_command("Inicjalizacja pomiaru:\n\r");
        uart_text_command("Wpisz nazwę czujnika\n\r");
    }
    else if(strcmp(command, "pomiar_single") == 0){
    	uart_text_command("Wpisz nazwę czujnika\n\r");
    }
    // Nieznana komenda
    else{
        if(active_sensor_for_single_measure != NULL){
            if (communication_module_for_dht_11(active_sensor_for_single_measure, active_sensor_for_single_measure->sensor_name, active_sensor_for_single_measure->sensor_GPIOx, active_sensor_for_single_measure->sensor_GPIO_PIN, command) == 1) {
                return;
            }
        }
        uart_text_command("Nieznana komenda\n\r");
    }
}


uint8_t command_ready = 0;
char command_buffer[COMMUNICATION_BUFFER_LENGTH];
uint16_t gps_line_pos = 0;


void main_loop_for_measuring_data(DHT11_sensor sensor_n){
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

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_TIM7_Init();
  MX_SPI1_Init();
  MX_IWDG_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart2, &znak, 1);
 //HAL_UART_Receive(&huart1, rx_buf, 300);
  HAL_UART_Receive_DMA(&huart1, rx_buf, GPS_BUFFER_LENGTH);
  name_for_sensor(&sensor_1, SENSOR_NAME);
  measure_data_of_DHT11(&sensor_1, sensor_1.sensor_GPIOx, sensor_1.sensor_GPIO_PIN);
  beginOLED(&hspi1);
  uint32_t last_update_time = 0;
  uint32_t last_gps_update_time = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	   uint32_t current_time = HAL_GetTick();
	   // GPS
	   if(current_time - last_gps_update_time >= 50){
		   last_gps_update_time = current_time;
		   check_for_gps_line(&m1,gps_line_pos,rx_buf,gps_line_buffer);
		   //m, uint16_t gps_line_pos, uint8_t rx_buf[],char gps_line_buffer[]
	   }
	   HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	   if(current_time - last_update_time >= 1000){
	    last_update_time = current_time;
			if(machine_state_init_status == INIT_START){
				continous_measurment_at_current_place(*active_sensor, &uart_cyclic_buffer, &uart_tx_in_progress,delay_ms);
			}
		data_change(&sensor_1, &struct_for_con_mea);
		if(command_ready){
		    command_ready = 0;

		    commands(command_buffer);
		}
		HAL_IWDG_Refresh(&hiwdg);
	   }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
    if(huart->Instance == USART2){
    	uart_cyclic_buffer.tail++;
    	if(uart_cyclic_buffer.tail == COMMUNICATION_BUFFER_LENGTH){
    	    uart_cyclic_buffer.tail = 0;
    	}
    	if(!(uart_cyclic_buffer.tail == uart_cyclic_buffer.head)){
    		HAL_UART_Transmit_IT(&huart2, (uint8_t*) &uart_cyclic_buffer.buffer[uart_cyclic_buffer.tail], 1);
    	}else{
    		uart_tx_in_progress = 0;
    	}
    }
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART2){
    	communication_module(znak, &command_ready, command_buffer, &number_of_characters, communication_buffor);
        HAL_UART_Receive_IT(&huart2, &znak, 1); // odbierz kolejny znak
    }
    //else if(huart->Instance == USART1){
    //	parceMessage(&m1, rx_buf);
    //	HAL_UART_Receive_IT(&huart1, rx_buf, 1000);
    //}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(tick+250 <= HAL_GetTick()) {
		tick = HAL_GetTick();
		pressed=1;
		if(counter == 15) counter=0;
		else counter++;
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
