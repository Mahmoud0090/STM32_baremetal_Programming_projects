#include "stm32f4xx.h"
#include "uart.h"

#define GPIOAEN 		(1U<<0)
#define UART2EN			(1U<<17)
#define UART2_TX_EN		(1U<<3)
#define CR1_UART2EN		(1U<<13)
#define UART2_SR_TXE	(1U<<7)

void uart2_init(void)
{
	//enable GPIOA and uart2 clocks
	RCC->AHB1ENR |= GPIOAEN;
	RCC->APB1ENR |= UART2EN;

	//set PA2 as alternate function (as TX)
	GPIOA->MODER &= ~(3U<<(2 * 2));
	GPIOA->MODER |= (2U<<(2 * 2));

	//configure AFR to be (AF7)
	GPIOA->AFR[0] &= ~(0xF<<(2 * 4));
	GPIOA->AFR[0] |= (7U<<(2 * 4));

	//uart configuration
	//baud rate
	USART2->BRR = 0x0683; //for 9600 & 16Mhz

	//enable Tx
	USART2->CR1 |= UART2_TX_EN;

	//enable uart2
	USART2->CR1 |= CR1_UART2EN;
}

void uart2_write(char c)
{
	while(!(USART2->SR & UART2_SR_TXE)); // Wait until TX buffer empty

	USART2->DR = c;
}

void uart2_print(char* s)
{
	while(*s)
	{
		uart2_write(*s++);
	}
}

void uart2_print_uint32(uint32_t num)
{
    char buffer[11]; // Enough for 10 digits + null
    int i = 0;

    if (num == 0) {
        uart2_write('0');
        return;
    }

    while (num > 0 && i < 10) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    while (i > 0) {
        uart2_write(buffer[--i]);
    }
}
