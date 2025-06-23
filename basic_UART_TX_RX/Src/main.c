#include "stm32f4xx.h"


#define USART2EN	(1U<<17)
#define GPIOAEN		(1U<<0)

#define USART2_TXE	(1U<<7)
#define USART2_RXNE	(1U<<5)

void uart2_init(void);
void uart2_write(char c);
char uart2_read(void);
void uart2_print(char* s);
void process_line(void);

#define BUFFER_SIZE 100
char rx_buffer[BUFFER_SIZE];
int buf_index = 0;


int main(void)
{
	uart2_init();
	uart2_print("Hello!\n\r");

	while(1)
	{
		char received = uart2_read();   // Wait for input
		uart2_write(received);          // Echo it back

		if(received == '\r')			// Enter key sends '\r'
		{
			uart2_print("\n\r");
			rx_buffer[buf_index] = '\0';	// Null-terminate the string
			process_line();
			buf_index = 0;					// Reset buffer
		}
		else if(buf_index < BUFFER_SIZE - 1)
		{
			rx_buffer[buf_index++] = received; // Store char
		}
	}
}

void uart2_init(void)
{
	/*enable uart2 clock*/
	RCC->APB1ENR |= USART2EN;

	/*enable GPIOA clock (it is used for PA2 and PA3 as TX and RX)*/
	RCC->AHB1ENR |= GPIOAEN;

	/*clean gpio for PA3 and PA2*/
	GPIOA->MODER &= ~(3U<<(2*2)); // clear PA2
	GPIOA->MODER &= ~(3U<<(3*2)); //clear PA3

	GPIOA->MODER |= (2U<<(2*2)); // alternate func for PA2
	GPIOA->MODER |= (2U<<(2*3)); // alternate func for PA3

	GPIOA->AFR[0] |= (7U<<(4*2)); // AF7 for PA2
	GPIOA->AFR[0] |= (7U<<(4*3)); //AF7 for PA3

	/*usart2 baud rate*/
	USART2->BRR = 0x0683; // Baud = 9600 @16MHz

	/*usart2 tx and rx enable*/
	USART2->CR1 |= (1U<<2) | (1U<<3);

	/*enable usart2*/
	USART2->CR1 |= (1U<<13);
}

// Transmit one byte
void uart2_write(char c)
{
	while(!(USART2->SR & USART2_TXE)); // Wait until TX buffer empty

	USART2->DR = c;
}
// Receive one byte
char uart2_read(void)
{
	while(!(USART2->SR & USART2_RXNE));

	return USART2->DR;
}

// Print a string via UART
void uart2_print(char* s)
{
	while(*s)
	{
		uart2_write(*s++);
	}
}

// === Process complete input line ===
void process_line(void)
{
	uart2_print("you typed : ");
	uart2_print(rx_buffer);
	uart2_print("\n\r");
}
