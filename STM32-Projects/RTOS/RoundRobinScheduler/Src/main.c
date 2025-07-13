#include "led.h"
#include "osKernel.h"

void motor_run(void);
void motor_stop(void);
void value_open(void);
void value_close(void);

#define QUANTA		10 // ms

typedef uint32_t TaskProfiler;

TaskProfiler	task0_prof, task1_prof, task2_prof;

void task0(void){
	while(1){
		task0_prof++;
		turn_blue_led_on();
	}
}

void task1(void){
	while(1){
		task1_prof++;
		turn_green_led_on();
	}
}

void task2(void){
	while(1){
		task2_prof++;
		turn_blue_led_off();
		turn_green_led_off();
	}
}

int main(void){
	/* Main loop */
	led_init();
	osKernelInit();
	osKernelAddThread(&task0, &task1, &task2);
	osKernelLaunch(QUANTA);
}

