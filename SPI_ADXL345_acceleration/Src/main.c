#include "stm32f4xx.h"
#include <stdio.h>

#define CS_LOW()	(GPIOA->ODR &= ~(1U<<4))
#define CS_HIGH()	(GPIOA->ODR |= (1U<<4))


#define ADXL345_DEVID_REG   0x00
#define ADXL345_POWER_CTL   0x2D
#define ADXL345_DATAX0      0x32

void spi1_gpio_init(void);
void spi1_init(void);
uint8_t spi1_transfer(uint8_t data);
void adxl345_write(uint8_t reg, uint8_t value);
uint8_t adxl345_read_register(uint8_t reg);
void adxl345_read_xyz(int16_t* x , int16_t* y , int16_t* z);
void uart2_init(void);
void uart2_write(char c);
void uart2_print(const char* s);


int main()
{
	int16_t x, y, z;
	char buffer[64];

	spi1_gpio_init();
	spi1_init();
	uart2_init();

	// Put ADXL345 in measurement mode
	adxl345_write(ADXL345_POWER_CTL, 0x08);

	// Read and verify device ID
	uint8_t devid = adxl345_read_register(ADXL345_DEVID_REG);
	if (devid == 0xE5)
		uart2_print("ADXL345 detected via SPI!\r\n");
	else
		uart2_print("Device ID mismatch!\r\n");

	while(1)
	{
		adxl345_read_xyz(&x, &y, &z);

		sprintf(buffer, "X: %d  Y: %d  Z: %d\r\n", x, y, z);
		uart2_print(buffer);

		for (volatile int i = 0; i < 100000; i++); // Delay
	}

}

/*PA4 - CS
 * PA5 - SCLK
 * PA6 MISO
 * PA7 MOSI
 * alternative AF5*/
void spi1_gpio_init(void)
{
	//enable GPIOA clock
	RCC->AHB1ENR |= (1U<<0);

	//alternative mode
	GPIOA->MODER &= ~((3U<<(2*4)) | (3U<<(2*5)) | (3U<<(2*6)) | (3U<<(2*7)));
	GPIOA->MODER |= ((2U<<(2*5)) | (2U<<(2*6)) | (2U<<(2*7))); //alternative mode
	GPIOA->MODER |= (1U<<(2*4)); // as output

	GPIOA->AFR[0] &= ~((0xF<<(4*5)) | (0xF<<(4*6)) | (0xF<<(4*7)));
	GPIOA->AFR[0] |= ((5U<<(4*5)) | (5U<<(4*6)) | (5U<<(4*7)));

	CS_HIGH();
}

void spi1_init(void)
{
	//enable SPI1 clock
	RCC->APB2ENR |= (1U<<12);

	SPI1->CR1 = 0;
	SPI1->CR1 |= (1U<<2); //master mode
	SPI1->CR1 |= (1U<<1); //CPOL = 1 (idle high)
	SPI1->CR1 |= (1U<<0); //CPHA = 1 (data capture on trailing edge)
	SPI1->CR1 |= (7U<<3); // fPCLK/256 clock frequency
	SPI1->CR1 |= (1U<<9); //Software slave management
	SPI1->CR1 |= (1U<<8); //Internal slave select
	SPI1->CR1 |= (1U<<6); //SPI1 peripheral enable
}

uint8_t spi1_transfer(uint8_t data)
{
	while(!(SPI1->SR & (1U<<1))); //wait for TXE
	SPI1->DR = data;

	while(!(SPI1->SR & (1U<<0))); //wait for RXNE

	return (SPI1->DR);
}

void adxl345_write(uint8_t reg, uint8_t value)
{
    CS_LOW();
    spi1_transfer(reg & 0x3F); // Bit 7 = 0 for write
    spi1_transfer(value);
    CS_HIGH();
}

uint8_t adxl345_read_register(uint8_t reg)
{
	uint8_t value;
	CS_LOW();

	spi1_transfer(0x80 | (reg & 0x3F));// Bit 7 = 1 (read), Bit 6 = 0 (single-byte)
									//according to ADXL345 datasheet — SPI Communication Protocol
	value = spi1_transfer(0x00);
	CS_HIGH();

	return value;
}

void adxl345_read_xyz(int16_t* x , int16_t* y , int16_t* z)
{
	uint8_t buffer[6];

	CS_LOW();

    // Read command: Bit 7 = 1 (read), Bit 6 = 1 (multi-byte)
	spi1_transfer(0xC0 | (ADXL345_DATAX0 & 0x3F)); // 0x32 = start register

	for(int i = 0 ; i<6 ; i++)
		buffer[i] = spi1_transfer(0x00); // Dummy write to read

	CS_HIGH();

	*x = (int16_t)((buffer[1] << 8) | buffer[0]);
	*y = (int16_t)((buffer[3] << 8) | buffer[2]);
	*z = (int16_t)((buffer[5] << 8) | buffer[4]);
}

// === UART2 CONFIG + FUNCTIONS ===
// PA2 -> TX
void uart2_init(void)
{
    RCC->AHB1ENR |= (1U<<0);
    RCC->APB1ENR |= (1U<<17);

    GPIOA->MODER &= ~(3U << (2*2));
    GPIOA->MODER |=  (2U << (2*2)); // AF mode for PA2
    GPIOA->AFR[0] &= ~(0xF << (4*2));
    GPIOA->AFR[0] |=  (7U << (4*2)); // AF7 (USART2)

    USART2->BRR = 0x0683; // 9600 @ 16MHz
    USART2->CR1 |= (1U << 3);  // TE
    USART2->CR1 |= (1U << 13); // UE
}

void uart2_write(char c)
{
    while (!(USART2->SR & (1U << 7))); // Wait for TXE
    USART2->DR = c;
}

void uart2_print(const char* s)
{
    while (*s)
        uart2_write(*s++);
}
