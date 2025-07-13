/*
 * uart.c
 *
 *  Created on: Jul 2, 2025
 *      Author: przem
 */

#include "uart.h"
#include "stm32f4xx.h"
#include "stdio.h"

#define GPIOAEN				(1U<<0) // IO port A clock enabled
#define PA2_USART_TX		(1U<<2)
#define UART2EN				(1U<<17)

#define SYS_FREQ			16000000
#define APB1_CLK			SYS_FREQ
#define USART2_BAUDRATE		9600

#define CR1_TE		(1U<<3)
#define CR1_UE 		(1U<<13)
#define SR_TXE		(1U<<7)

static void uart_set_baudrate(uint32_t periph_clk_freq, uint32_t baudrate);
static uint32_t compute_bd_value(uint32_t periph_clk_freq , uint32_t baudrate);
static void uart_write(int sign);

/*--------------Functions-----------------*/

void uart_tx_init(void){
	/* Enables clock access to GPIOA*/
	RCC->AHB1ENR |= GPIOAEN;

	/* Set pin PA2 to alternative function mode*/
	GPIOA->MODER |= (1U<<5);
	GPIOA->MODER &= ~(1U<<4);

	/* Set alternate function to AF07*/
	// AFRL1 represents all pin ones
	GPIOA->AFR[0] |=(1U<<8); // [0] -> because I use AFRL
	GPIOA->AFR[0] |=(1U<<9);
	GPIOA->AFR[0] |=(1U<<10);
	GPIOA->AFR[0] &= ~(1U<<11);

	/* Enables clock access to USART2 -> APB1*/
	RCC->APB1ENR |= UART2EN;

	/*Configured baudrate*/
	uart_set_baudrate(APB1_CLK, USART2_BAUDRATE);

	/*Configured transfer direction*/
	// Control register 1 -> page 547
	USART2->CR1 = CR1_TE; // Transmitter enable

	/*Enables Uart mode*/
	USART2->CR1 |= CR1_UE;

}

static void uart_write(int sign){
	/*Make sure the transmit data register is empty*/
	// status reigster - page 544
	while(!(USART2->SR & SR_TXE )){}
	/*Write to transmit data register*/
	USART2->DR = (sign & 0xFF);
}

int __io_putchar(int sign){
	uart_write(sign);
	return sign;
}

static void uart_set_baudrate(uint32_t periph_clk_freq, uint32_t baudrate){
	USART2->BRR = compute_bd_value(periph_clk_freq, baudrate);
}


static uint32_t compute_bd_value(uint32_t periph_clk_freq , uint32_t baudrate){
	uint32_t baudrate_set;
	baudrate_set = ((periph_clk_freq + (baudrate/2U))/baudrate);
	return baudrate_set;
}

/*-----------------------------------------*/


