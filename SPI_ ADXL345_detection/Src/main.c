#include "stm32f4xx.h"
#include <stdio.h>

#define CS_HIGH()  (GPIOA->ODR |= (1 << 4))
#define CS_LOW()   (GPIOA->ODR &= ~(1 << 4))

void spi1_gpio_init(void);
void spi1_init(void);
uint8_t spi1_transfer(uint8_t data);
uint8_t adxl345_read_register(uint8_t reg);


void uart2_init(void);
void uart2_write(char c);
void uart2_print(char* s);

char buffer[50];

int main(void)
{
    spi1_gpio_init();
    spi1_init();
    uart2_init(); // from previous code

    uint8_t devid = adxl345_read_register(0x00); // WHO_AM_I
    if (devid == 0xE5)
        uart2_print("ADXL345 (SPI) detected!\r\n");
    else
        uart2_print("Device ID mismatch!\r\n");


    uint8_t c = adxl345_read_register(0x00);
    sprintf(buffer, "Device ID: 0x%02X\r\n", c); // should print 0xE5
    uart2_print(buffer);

    while (1);
}

void spi1_gpio_init(void)
{
	RCC->AHB1ENR |= (1U<<0); //enable GPIOA clock

	// PA5 (SCK), PA6 (MISO), PA7 (MOSI), PA4 (CS)
	GPIOA->MODER &= ~((3U<<(2*4)) | (3U<<(2*5)) | (3U<<(2*6)) | (3U<<(2*7)));
	GPIOA->MODER |= ((2U<<(2*5)) | (2U<<(2*6)) | (2U<<(2*7)));// Alternate function
	GPIOA->MODER |= (1U<<(2*4)); // PA4 as output for CS

	//AF5
	GPIOA->AFR[0] &= ~((0xF<<(4*5)) | (0xF<<(4*6)) | (0xF<<(4*7)));
	GPIOA->AFR[0] |= ((5U<<(4*5)) | (5U<<(4*6)) | (5U<<(4*7))); // AF5 = SPI1

	CS_HIGH(); // Deselect ADXL345
}

void spi1_init(void)
{
	//enable spi1 clock
	RCC->APB2ENR |= (1U<<12);

	SPI1->CR1 = 0;
	SPI1->CR1 |= (1U<<2); //master mode
	SPI1->CR1 |= (7U<<3); //Baud rate: fPCLK/256
	SPI1->CR1 |= (1U<<0); // CPOL = 1 (idle high)
	SPI1->CR1 |= (1U<<1); // CPHA = 1 (second clock transition)
	SPI1->CR1 |= (1U<<9); // Software slave management
	SPI1->CR1 |= (1U<<8); // SSI bit set
	SPI1->CR1 |= (1U<<6); // Enable SPI
}

uint8_t spi1_transfer(uint8_t data)
{
    while (!(SPI1->SR & (1 << 1))); // Wait until TXE
    SPI1->DR = data;
    while (!(SPI1->SR & (1 << 0))); // Wait until RXNE
    return SPI1->DR;
}


uint8_t adxl345_read_register(uint8_t reg)
{
    uint8_t value;
    CS_LOW();
    spi1_transfer(0x80 | (reg & 0x3F)); // Bit 7 = 1 (read), Bit 6 = 0 (single-byte)
    								//according to ADXL345 datasheet — SPI Communication Protocol
    value = spi1_transfer(0x00);
    CS_HIGH();
    return value;
}

void uart2_init(void)
{
	RCC->AHB1ENR |= (1U << 0);  // GPIOA
	RCC->APB1ENR |= (1U<<17);

	GPIOA->MODER &= ~(3U << (2 * 2));
	GPIOA->MODER |= (2U << (2 * 2)); // Alternate function

	GPIOA->AFR[0] &= ~(0xF << (4 * 2));
	GPIOA->AFR[0] |= (7U << (4 * 2)); // AF7

	USART2->BRR = 0x0683; // 9600 baud for 16MHz
	USART2->CR1 |= (1U<<3); //tx enable
	USART2->CR1 |= (1U<<13); //uart enable
}

void uart2_write(char c)
{
	while (!(USART2->SR & (1U<<7)));
	USART2->DR = c;
}

void uart2_print(char* s)
{
	while (*s)
		uart2_write(*s++);
}
