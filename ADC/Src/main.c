#include "stm32f4xx.h"
#include <stdio.h>

void uart2_init(void);
void uart2_write(char c);
void uart2_print(const char* s);
void adc1_init(void);
uint16_t adc1_read(void);


int main(void)
{
	char buffer[64];
	uint16_t adc_value;

	uart2_init();
	adc1_init();

	uart2_print("Starting ADC read...\r\n");

	while(1)
	{
		adc_value = adc1_read();
		sprintf(buffer, "ADC Value: %u\r\n", adc_value);
		uart2_print(buffer);

		for (volatile int i = 0; i < 100000; i++); // delay
	}
}

// === UART2 (PA2 - TX) ===
void uart2_init(void)
{
	//enable GPIOA and USART2 clock
	RCC->AHB1ENR |= (1U<<0);
	RCC->APB1ENR |= (1U<<17);
	//configure mode as alternate function
	GPIOA->MODER &= ~(3U<<(2*2));
	GPIOA->MODER |= (2U<<(2*2));

	//AF7 for PA2
	GPIOA->AFR[0] &= ~(0xF<<(4*2));
	GPIOA->AFR[0] |= (7U<<(4*2));

	//configure uart2
	USART2->BRR = 0x0683; // 9600 baud @16MHz
	USART2->CR1 |= (1U<<3); //Tx enable
	USART2->CR1 |= (1U<<13); //USART enable
}

void uart2_write(char c)
{
	while(!(USART2->SR & (1U<<7))); // wait for TXE bit
	USART2->DR = c;
}

void uart2_print(const char* s)
{
	while(*s)
		uart2_write(*s++);
}

// === ADC1 - PA1 (Channel 1) ===
void adc1_init(void)
{
	// 1. Enable clock to GPIOA (if you're using PA1 for example)
	RCC->AHB1ENR |= (1U << 0);   // Enable GPIOA clock

	// 2. Set PA1 to Analog mode
	GPIOA->MODER |= (3U << (2 * 1)); // MODER1[1:0] = 11 => Analog mode
	GPIOA->PUPDR &= ~(3U << (2 * 1)); // No pull-up or pull-down

	// 3. Enable ADC1 clock
	RCC->APB2ENR |= (1U << 8); // Enable ADC1 clock

	// 4. Set ADC prescaler in ADC Common Control Register
	ADC->CCR |= (1U << 16); // PCLK2 divided by 4 (optional tuning)

	// 5. Set ADC resolution (optional, default is 12-bit)
	ADC1->CR1 &= ~(3U << 24); // 00: 12-bit resolution

	// 6. Configure conversion sequence
	ADC1->SQR3 = 1; // Channel 1 as 1st conversion (PA1 = ADC1_IN1)

	// 7. Set sample time
	ADC1->SMPR2 |= (3U << 3); // 56 cycles for channel 1 (bits 5:3)

	// 8. Enable ADC
	ADC1->CR2 |= (1U << 0); // ADON = 1 => enable ADC
}

uint16_t adc1_read(void)
{
	ADC1->CR2 |= (1U << 30); // Start conversion (SWSTART)
	while (!(ADC1->SR & (1U << 1))); // Wait for EOC
	return (uint16_t)ADC1->DR;
}
