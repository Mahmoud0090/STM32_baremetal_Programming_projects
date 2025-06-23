#include "stm32f4xx.h"

#define USART2EN	(1U<<17)
#define GPIOAEN		(1U<<0)

#define USART2_SR_RXNE	(1U<<5)
#define USART2_SR_TXE	(1U<<7)

void uart2_init(void);
void uart2_write(char c);
char uart2_read(void);
void uart2_print(const char *s);
void process_line(void);


#define BUF_SIZE 100

volatile char rx_buffer[BUF_SIZE];
volatile int rx_index = 0;
volatile int line_ready = 0;

int main(void)
{
	uart2_init();
	uart2_print("UART RX Interrupt Ready\r\n");

	while(1)
	{
		if (line_ready)
		{
			rx_buffer[rx_index] = '\0'; // Null-terminate the string
			process_line();
			rx_index = 0;
			line_ready = 0;
		}
	}
}


void uart2_init(void)
{
	//enable usart2 clock
	RCC->APB1ENR |= USART2EN;
	//enable GPIOA clock (for PA2 and PA3)
	RCC->AHB1ENR |= GPIOAEN;

	GPIOA->MODER &= ~(3U<<(2*2));
	GPIOA->MODER &= ~(3U<<(2*3));

	GPIOA->MODER |= (2U<<(2*2));
	GPIOA->MODER |= (2U<<(2*3));

	GPIOA->AFR[0] |= (7U<<(4*2));
	GPIOA->AFR[0] |= (7U<<(4*3));

	//usart configuration
	USART2->BRR = 0x0683; //baud rate 9600 @16Mhz
	USART2->CR1 |= (1U<<2) | (1U<<3); //enable tx and rx
	USART2->CR1 |= (1U<<5); //enable rx interrupt
	USART2->CR1 |= (1U<<13); //usart enable

	//enable usart2 in NVIC
	NVIC_EnableIRQ(USART2_IRQn);
}

void USART2_IRQHandler(void)
{
	if(USART2->SR & USART2_SR_RXNE)
	{
		char c = USART2->DR; // Reading DR clears RXNE

		if(c == '\r') //end of line
		{
			line_ready = 1;
		}
		else if (rx_index < BUF_SIZE - 1)
		{
			rx_buffer[rx_index++] = c;
		}
	}
}

// === UART Write Function ===
void uart2_write(char c)
{
	while(!(USART2->SR & USART2_SR_TXE));
	USART2->DR = c;
}

// === UART read Function ===
char uart2_read(void)
{
	while(!(USART2->SR & USART2_SR_RXNE));

	return USART2->DR;
}

// === UART Print Function ===
void uart2_print(const char *s)
{
    while (*s)
    {
        uart2_write(*s++);
    }
}

// === Process full line ===
void process_line(void)
{
    uart2_print("You typed: ");
    uart2_print((char*)rx_buffer);
    uart2_print("\r\n");
}
