#include "led.h"
#include "osKernel.h"
#include "timers.h"

void motor_run(void);
void motor_stop(void);
void value_open(void);
void value_close(void);

#define QUANTA		10 // ms

int32_t semaphore1, semaphore2;
typedef uint32_t TaskProfiler;

TaskProfiler	task0_prof, task1_prof, task2_prof, p_task3_prof, p_task4_prof;

void task0(void){
	while(1){
		task0_prof++;
		turn_blue_led_on();
		osThreadYield();
	}
}

void task1(void){
	while(1){
		osSemaphoreWait(&semaphore1);
		task1_prof++;
		turn_green_led_on();
		osThreadYield();
		osSemaphoreSet(&semaphore2);
	}
}

void task2(void){
	while(1){
		osSemaphoreWait(&semaphore2);
		task2_prof++;
		turn_blue_led_off();
		turn_green_led_off();
		osSemaphoreSet(&semaphore1);
	}
}

/*Periodic Thread*/
void task3(void){
	p_task3_prof++;
}

int main(void){
	/* Main loop */

	osSemaphore_init(&semaphore1, 1);
	osSemaphore_init(&semaphore2, 0);
	led_init();
	osKernelInit();
	osKernelAddThread(&task0, &task1, &task2);
	osKernelLaunch(QUANTA);
}

void TIM2_IRQHandler(void){
	/*Clear update interrupt flag*/


	/*----*/
	p_task4_prof++;
}






