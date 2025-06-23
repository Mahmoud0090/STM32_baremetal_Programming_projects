#include "stm32f4xx.h"
#include <stdio.h>


#define ADXL345_ADDR 	0x53


void i2c1_gpio_init(void);
void i2c1_init(void);
uint8_t i2c1_read_register(uint8_t slave_addr, uint8_t reg_addr);
void i2c1_write_register(uint8_t slave_addr, uint8_t reg_addr, uint8_t data);
void read_accel_xyz(int16_t* x, int16_t* y, int16_t* z);

void uart2_init(void);
void uart2_write(char c);
void uart2_print(char* s);

int main(void)
{
	i2c1_gpio_init();
	i2c1_init();
	uart2_init();

	// Enable measurement mode
	i2c1_write_register(ADXL345_ADDR, 0x2D, 0x08);

	uint8_t device_id = i2c1_read_register(ADXL345_ADDR, 0x00);
	if (device_id == 0xE5)
		uart2_print("ADXL345 detected!\r\n");
	else
		uart2_print("Device ID mismatch!\r\n");

	char buffer[64];
	int16_t x, y, z;

	while (1)
	{
		read_accel_xyz(&x, &y, &z);

		sprintf(buffer, "X: %d  Y: %d  Z: %d\r\n", x, y, z);
		uart2_print(buffer);

		for (volatile int i = 0; i < 100000; i++); // Delay
	}
}

//i2c1 SCL PB6
//i2c1 SDA PB7
//AF4
void i2c1_gpio_init(void)
{
	//enable GPIOB clock
	RCC->AHB1ENR |= (1U<<1);

	//alternative mode
	GPIOB->MODER &= ~(3U<<(2*6));
	GPIOB->MODER &= ~(3U<<(2*7));
	GPIOB->MODER |= ((2U<<(2*6)) | (2U<<(2*7)));
	//AF4 configuration for PB6 and PB7
	GPIOB->AFR[0] &= ~(0xF<<(4*6));
	GPIOB->AFR[0] &= ~(0xF<<(4*7));
	GPIOB->AFR[0] |= ((4U<<(4*6)) | (4U<<(4*7)));

	//enable internal pullup resistor
	GPIOB->PUPDR &= ~(3U<<(2*6));
	GPIOB->PUPDR &= ~(3U<<(2*7));
	GPIOB->PUPDR |= ((1U<<(2*6)) | (1U<<(2*7)));

	//open drain mode
	GPIOB->OTYPER |= ((1U<<6) | (1U<<7));
}

void i2c1_init(void)
{
	//enable i2c1 clock
	RCC->APB1ENR |= (1U<<21);

	//resetting the I2C1
	I2C1->CR1 |= (1U<<15);
	I2C1->CR1 &= ~(1<<15);

	//assigning 16 Mhz clock of PCLK to CCR register
	I2C1->CR2 = 16;

	//configure the i2c1 clock to be 100Khz
	I2C1->CCR = 80;

	//configure rise time
	I2C1->TRISE = 17;

	//enable the peripheral
	I2C1->CR1 |= (1U<<0);
}

uint8_t i2c1_read_register(uint8_t slave_addr , uint8_t reg_addr)
{
	uint8_t received;

	//enable start bit
	I2C1->CR1 |= (1U<<8);
	while(!(I2C1->SR1 & (1U<<0)));

	I2C1->DR = (slave_addr<<1); //write LSB is 0
	while(!(I2C1->SR1 & (1<<1))); //wait to check for address
	(void)I2C1->SR1;
	(void)I2C1->SR2;

	I2C1->DR = reg_addr;
	while(!(I2C1->SR1 & (1U<<7))); //TxE bit Data register empty (transmitters)

	//reinitialize start bit again
	I2C1->CR1 |= (1U<<8);
	while(!(I2C1->SR1 & (1U<<0)));

	I2C1->DR = ((slave_addr<<1) | 1); //LSB is 1 for read
	while(!(I2C1->SR1 & (1U<<1)));
	(void)I2C1->SR1;
	(void)I2C1->SR2;

	//disable acknowledge
	I2C1->CR1 &= ~(1U<<10);

	while(!(I2C1->SR1 & (1U<<6))); // wait for RXNE

	//generate stop
	I2C1->CR1 |= (1U<<9);
	received = I2C1->DR;

	return received;
}

void i2c1_write_register(uint8_t slave_addr , uint8_t reg_addr, uint8_t data)
{
	I2C1->CR1 |= (1U<<8);
	while(!(I2C1->SR1 & (1U<<0)));

	I2C1->DR = (slave_addr<<1);
	while(!(I2C1->SR1 & (1U<<1)));
	(void)I2C1->SR1;
	(void)I2C1->SR2;

	I2C1->DR = reg_addr;
	while(!(I2C1->SR1 & (1U<<7)));

	I2C1->DR = data;
	while(!(I2C1->SR1 & (1U<<2))); //BTF (byte transfer finished)

	I2C1->CR1 |= (1U<<9); // STOP
}

void read_accel_xyz(int16_t* x , int16_t*y , int16_t* z)
{
	uint8_t data[6];
	for(int i = 0 ; i<6 ; i++)
		data[i] = i2c1_read_register(ADXL345_ADDR , 0x32 + i);

	*x = ((int16_t)data[1] << 8) | data[0];
	*y = ((int16_t)data[3] << 8) | data[2];
	*z = ((int16_t)data[5] << 8) | data[4];
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
