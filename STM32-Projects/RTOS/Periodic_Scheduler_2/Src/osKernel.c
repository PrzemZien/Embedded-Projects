/*
 * osKernel.c
 *
 *  Created on: Jul 10, 2025
 *      Author: przem
 */
#include "stdint.h"
#include "stm32f4xx.h"
#include "osKernel.h"

#define NUMBER_OF_THREADS		3
#define STACK_SIZE				128
#define PSR_T_BIT				24
#define BUS_FREQ				16000000

#define SYST_CSR_COUNTFLAG			(1U<<16)	// Return 1 if timer counted to 0 since last time this was read.
#define SYST_CSR_CLK_SOURCE			(1U<<2)	// Clock source = processor clock.
#define SYST_CSR_TICKINT			(1U<<1)	// Enable SysTick exception request.
#define SYST_CSR_CLK_ENABLE			(1U<<0)	// Enable the counter.
#define SYST_RST					0
#define NEUTRAL_VAL					0xAAAAAAAA // maybe 0x00000000 would be better:)

#define ICSR_REGISTER				(*((volatile uint32_t*)0xE000ED04))
#define ICSR_PENDSTSET				(1U<<26)


/*Thread control block - blok sterowania wątkami*/

struct tcb{
	int32_t *stackPt;
	struct tcb *nextPt;
};

typedef struct tcb tcbType;

tcbType tcbs[NUMBER_OF_THREADS];
tcbType *currentPtr;
uint32_t MILIS_PRESCALER;
uint32_t period_tick;
void osSchedulerLaunch(void);
void osSchedulerRoundRobin(void);

/*Each thread will have stacksize of 100 i.e 400 bytes*/

int32_t TCB_STACK[NUMBER_OF_THREADS][STACK_SIZE];

void osKernelStack_init(int thread){

	/**/
	tcbs[thread].stackPt = &TCB_STACK[thread][STACK_SIZE - 16];

	/*Set bti21 (T-bit) int PSR to 1, to operate in thumb mode*/
    TCB_STACK[thread][STACK_SIZE-1] = (1U << PSR_T_BIT);  // PSR - Thumb bit
    TCB_STACK[thread][STACK_SIZE-2] = NEUTRAL_VAL; // PC (zostanie nadpisany)
    TCB_STACK[thread][STACK_SIZE-3] = NEUTRAL_VAL; // LR
    TCB_STACK[thread][STACK_SIZE-4] = NEUTRAL_VAL; // R12
    TCB_STACK[thread][STACK_SIZE-5] = NEUTRAL_VAL; // R3
    TCB_STACK[thread][STACK_SIZE-6] = NEUTRAL_VAL; // R2
    TCB_STACK[thread][STACK_SIZE-7] = NEUTRAL_VAL; // R1
    TCB_STACK[thread][STACK_SIZE-8] = NEUTRAL_VAL; // R0

    TCB_STACK[thread][STACK_SIZE-9] = NEUTRAL_VAL;// R11
    TCB_STACK[thread][STACK_SIZE-10] = NEUTRAL_VAL;// R10
    TCB_STACK[thread][STACK_SIZE-11] = NEUTRAL_VAL;// R9
    TCB_STACK[thread][STACK_SIZE-12] = NEUTRAL_VAL;// R8
    TCB_STACK[thread][STACK_SIZE-13] = NEUTRAL_VAL;// R7
    TCB_STACK[thread][STACK_SIZE-14] = NEUTRAL_VAL;// R6
    TCB_STACK[thread][STACK_SIZE-15] = NEUTRAL_VAL;// R5
    TCB_STACK[thread][STACK_SIZE-16] = NEUTRAL_VAL;// R4
}

uint8_t osKernelAddThread(void(*task0)(void),void(*task1)(void),void(*task2)(void)){

	/*Disbale global interrupts*/
	__disable_irq();
	tcbs[0].nextPt = &tcbs[1];
	tcbs[1].nextPt = &tcbs[2];
	tcbs[2].nextPt = &tcbs[0];

	/*Initialize stack for thread0*/
	osKernelStack_init(0);
	/*Initialize PC*/
	TCB_STACK[0][STACK_SIZE - 2] = (int32_t)(task0);

	/*Initialize stack for thread1*/
	osKernelStack_init(1);
	/*Initialize PC*/
	TCB_STACK[1][STACK_SIZE - 2] = (int32_t)(task1);

	/*Initialize stack for thread2*/
	osKernelStack_init(2);
	/*Initialize PC*/
	TCB_STACK[2][STACK_SIZE - 2] = (int32_t)(task2);

	currentPtr = &tcbs[0];

	/*Enable global intterrupts*/
	__enable_irq();
	return 1;
}

void osKernelInit(void){
	MILIS_PRESCALER = (BUS_FREQ/1000);
}

void osKernelLaunch(uint32_t quanta){
	/*Reset systick*/
	SysTick->CTRL = SYST_RST;

	/* Clear systick current value register*/
	SysTick->VAL = 0;

	/*Load quanta*/
	SysTick->LOAD = (quanta * MILIS_PRESCALER) - 1;

	/*Set systick to low priority*/
	NVIC_SetPriority(SysTick_IRQn,15);

	/*Enable systick, select internal clock*/
	SysTick->CTRL = SYST_CSR_CLK_SOURCE | SYST_CSR_CLK_ENABLE;

	/*Enable systick interrupt*/
	SysTick->CTRL |= SYST_CSR_TICKINT;

	/*Launch scheduler*/
	osSchedulerLaunch();

}

/*Thread switching function*/

__attribute__((naked)) void SysTick_Handler(void){

	/*Suspend current thread*/

	/*Disable global interrupt*/
	__asm("CPSID	I");

	/*Save R4-11*/
	__asm("PUSH	{R4-R11}");

	/*Load address of currentPt into R0*/
	__asm("LDR R0, =currentPtr");

	/*Load R1 from address equals R0, i.e R11 =currentPt*/
	__asm("LDR R1,[R0]");

	/*Store Cortex-M SP at address equals R1, i.e Save SP into TCB*/
	__asm("STR SP,[R1]");


	/*Choose next thread */

	__asm("PUSH		{R0,LR}");
	__asm("BL		osSchedulerRoundRobin");
	__asm("POP		{R0,LR}");

	/*R1 == currentPtr */
	__asm("LDR		R1,[R0]");

	/*sp = currentPt->StackPt*/
	__asm("LDR 		SP,[R1]");

	/*Restore R4-11*/
	__asm("POP {R4-R11}");

	/*Enable global interrupts*/
	__asm("CPSIE	I");

	/*Return from expection and restore R0,R1,R2,R3,R12,LR,PC,PSR*/
	__asm("BX	LR");

}

void osSchedulerLaunch(void){
	/*Load address of  currentPt into R0*/
	__asm("LDR R0,=currentPtr");

	/*Load R2 from address */
	__asm("LDR R2,[r0]");

	/*Load Cortex-M SP from address equals R2,i.e. SP = currentPt->stackPt*/
	__asm("LDR	SP,[R2]");

	/*Restore R4-11*/
	__asm("POP {R4-R11}");

	/*Restore Linked register (R12)*/
	__asm("POP {R12}");

	/*Restore R0-3*/
	__asm("POP	{R0-R3}");
	/*Skip LR*/
	__asm("ADD	SP,SP,#4");

	/*Create a nwe start location by popping LR*/
	__asm("POP {LR}");

	/*Skip PSR by adding 4 to SP*/
	__asm("ADD SP,SP,#4");

	/*Enable global interrupts*/
	__asm("CPSIE	I");

	/*Retun from exception*/
	__asm("BX	LR");
}

void osThreadYield(void){

	/*Clear Systick Current Value register*/
	SysTick->VAL = 0;

	/*Trigger SysTick*/
	/* 4.3.3 Interrupt Control and State Register -> str. 229*/
	ICSR_REGISTER = ICSR_PENDSTSET;

}

void osSchedulerRoundRobin(void){
	if((++period_tick) == PERIOD_VALUE_FOR_T3){
		(*task3)();

		/*Delete // below to exectue more than once or comment to execute task3 once*/
		period_tick = 0;
	}

	currentPtr = currentPtr->nextPt;
}

void osSemaphore_init(int32_t *semaphore, int32_t value){
	*semaphore = value;
}

void osSemaphoreSet(int32_t *semaphore){
	__disable_irq();
	*semaphore += 1;
	__enable_irq();
}

void osSemaphoreWait(int32_t *semaphore){
	__disable_irq();
	while(*semaphore <= 0){
		__disable_irq();
		__enable_irq();
	}
	*semaphore -= 1;
	__enable_irq();
}




