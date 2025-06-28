#include "i2c.h"
#include "stm32f4xx.h"

#define SDA_PIN 7
#define SCL_PIN 6
#define I2C_TIMEOUT	100000

static volatile uint32_t timeout;

void i2c_init(void)
{
	//enable GPIOA and I2C1 clocks
	RCC->AHB1ENR |= (1U<<1);
	RCC->APB1ENR |= (1U<<21);

	//configure mode
	GPIOB->MODER &= ~((3U<<(2*SCL_PIN)) | (3U<<(2*SDA_PIN)));
	GPIOB->MODER |= ((2U<<(2*SCL_PIN)) | (2U<<(2*SDA_PIN))); //AF mode

	GPIOB->AFR[0] &= ~((0xF<<(4*SCL_PIN)) | (0xF<<(4*SDA_PIN)));
	GPIOB->AFR[0] |= (4U<<(4*SCL_PIN) | (4U<<(4*SDA_PIN))); //AF4

	GPIOB->OTYPER |= ((1U<<SCL_PIN) | (1U<<SDA_PIN)); //open drain

	GPIOB->OSPEEDR |= ((3U<<(2*SCL_PIN)) | (3U<<(2*SDA_PIN))); // high speed

	GPIOB->PUPDR |= ~((3U<<(2*SCL_PIN)) | (3U<<(2*SDA_PIN))); //no pu/pd

	I2C1->CR1 = (1U<<15); //SWRST
	I2C1->CR1 = 0;
	I2C1->CR2 = 16; // PCLK1 = 16 MHz
	I2C1->CCR = 80; // Standard mode ~100 kHz
	I2C1->TRISE = 17; // TRISE = max rise time
	I2C1->CR1 |= (1U<<0); // enable I2C peripheral
}

static int wait_flag(uint16_t flag)
{
	timeout = I2C_TIMEOUT;
	while(!(I2C1->SR & flag) && --timeout);
	return timeout != 0;
}

void i2c_start(void)
{
	I2C1->CR1 |= (1U<<8); //start bit
	wait_flag((1U<<0)); //SB bit in SR
}

void i2c_restart(void)
{
	I2C1->CR1 |= (1U<<8); //start bit;
	wait_flag((1U<<0));
}

void i2c_stop(void)
{
    I2C1->CR1 |= (1U<<9); //stop bit
}

void i2c_write(uint8_t data)
{
	I2C1->DR = data;
	wait_flag((1U<<7)); //TXE
	wait_flag((1U<<2)); //BTF (byte transfer finished)
}

uint8_t i2c_read_ack(void)
{
	I2C1->CR1 |= (1U<<10); //ack bit
	wait_flag((1U<<6)); //RXNE
	return I2C1->DR;
}

uint8_t i2c_read_nack(void)
{
	I2C1->CR1 &= ~(1U<<10); //ack bit
	wait_flag((1U<<6)); //RXNE
	return I2C1->DR;
}
