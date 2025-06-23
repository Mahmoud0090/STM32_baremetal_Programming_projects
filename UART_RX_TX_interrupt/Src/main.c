#include "stm32f4xx.h"
#include <string.h>

#define GPIOAEN		(1U<<0)
#define USART2EN	(1U<<17)

#define USART2_SR_TXE	(1U<<7)
#define USART2_SR_RXNE	(1U<<5)


void uart2_init(void);
void uart2_write(char c);
char uart2_read(void);
void uart2_print(const char *s);
void process_line(void);

#define BUF_SIZE 100
volatile char rx_buffer[BUF_SIZE];
volatile int rx_index = 0;
volatile int line_ready = 0;

#define TX_BUF_SIZE 100
volatile char tx_buffer[TX_BUF_SIZE];
volatile int tx_index = 0;
volatile int tx_busy = 0;

int main(void)
{
	uart2_init();
	uart2_print("UART RX Interrupt Ready\r\n");

	while (1)
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
	//enable GPIOA clock
	RCC->AHB1ENR |= GPIOAEN;
	//enable USART2 clock
	RCC->APB1ENR |= USART2EN;
	//configure GPIOA mode to alternative for (PA2,PA3)
	GPIOA->MODER &= ~(3U<<(2*2));
	GPIOA->MODER &= ~(3U<<(2*3));

	GPIOA->MODER |= (2U<<(2*2));
	GPIOA->MODER |= (2U<<(2*3));

	//
	GPIOA->AFR[0] |= ((7U<<(4*2)) | (7U<<(4*3)));

	//configure usart2
	//baud rate
	USART2->BRR = 0x8B; //for baud rate 115200 @16Mhz
	//enable rx and tx
	USART2->CR1 |= ((1U<<2) | (1U<<3));
	//enable RXNEIE (rx interrupt)
	USART2->CR1 |= (1U<<5);
	//enable usart2
	USART2->CR1 |= (1U<<13);
	//enable usart2 in NVIC
	NVIC_EnableIRQ(USART2_IRQn);
}

void USART2_IRQHandler(void)
{
    // RX interrupt
    if (USART2->SR & USART_SR_RXNE)
    {
        char c = USART2->DR;

        if (c == '\r')
        {
            line_ready = 1;
        }
        else if (rx_index < BUF_SIZE - 1)
        {
            rx_buffer[rx_index++] = c;
        }
    }

    // TX interrupt
    if ((USART2->SR & USART_SR_TXE) && (USART2->CR1 & USART_CR1_TXEIE))
    {
        if (tx_buffer[tx_index] != '\0')
        {
            USART2->DR = tx_buffer[tx_index++];
        }
        else
        {
            USART2->CR1 &= ~USART_CR1_TXEIE; // Disable TX interrupt
            tx_busy = 0;
        }
    }
}

void uart2_write(char c)
{
	while(!(USART2->SR & USART2_SR_TXE));

	USART2->DR = c;
}

char uart2_read(void)
{
	while(!(USART2->SR & USART2_SR_RXNE));

	return USART2->DR;
}

void uart2_print(const char *s)
{
    while(tx_busy); // Wait for previous TX to finish

    strncpy((char*)tx_buffer, s, TX_BUF_SIZE - 1);
    tx_buffer[TX_BUF_SIZE - 1] = '\0'; // Ensure termination
    tx_index = 0;
    tx_busy = 1;

    // Send first byte to kick off
    USART2->DR = tx_buffer[tx_index++];
    USART2->CR1 |= (1U<<7); // Enable TX interrupt
}

// === Process full line ===
void process_line(void)
{
    uart2_print("You typed: ");
    uart2_print((char*)rx_buffer);
    uart2_print("\r\n");
}
