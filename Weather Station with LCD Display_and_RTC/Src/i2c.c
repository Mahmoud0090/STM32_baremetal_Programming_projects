#include "i2c.h"
#include "delay.h"  // You must provide delay_us()

#define I2C_GPIO GPIOB
#define SDA_PIN 7
#define SCL_PIN 6

static void i2c_delay(void) {
    delay_us(5); // ~100 kHz I2C
}

static void sda_output(void) {
    I2C_GPIO->MODER &= ~(3U << (2 * SDA_PIN));
    I2C_GPIO->MODER |=  (1U << (2 * SDA_PIN)); // Output
}

static void sda_input(void) {
    I2C_GPIO->MODER &= ~(3U << (2 * SDA_PIN)); // Input
}

static void scl_high(void) {
    I2C_GPIO->BSRR = (1U << SCL_PIN);
}

static void scl_low(void) {
    I2C_GPIO->BSRR = (1U << (SCL_PIN + 16));
}

static void sda_high(void) {
    I2C_GPIO->BSRR = (1U << SDA_PIN);
}

static void sda_low(void) {
    I2C_GPIO->BSRR = (1U << (SDA_PIN + 16));
}

void i2c_init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    I2C_GPIO->MODER &= ~((3U << (2 * SDA_PIN)) | (3U << (2 * SCL_PIN)));
    I2C_GPIO->MODER |=  ((1U << (2 * SDA_PIN)) | (1U << (2 * SCL_PIN))); // Output

    I2C_GPIO->OTYPER |= (1U << SDA_PIN) | (1U << SCL_PIN);  // Open-drain
    I2C_GPIO->PUPDR &= ~((3U << (2 * SDA_PIN)) | (3U << (2 * SCL_PIN))); // No pull
    I2C_GPIO->PUPDR |=  ((1U << (2 * SDA_PIN)) | (1U << (2 * SCL_PIN))); // Pull-up

    sda_high();
    scl_high();
}

void i2c_start(void) {
    sda_high(); scl_high(); i2c_delay();
    sda_low();  i2c_delay();
    scl_low();  i2c_delay();
}

void i2c_stop(void) {
    scl_low();  sda_low(); i2c_delay();
    scl_high(); i2c_delay();
    sda_high(); i2c_delay();
}

uint8_t i2c_write(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        if (data & 0x80) sda_high(); else sda_low();
        scl_high(); i2c_delay();
        scl_low();  i2c_delay();
        data <<= 1;
    }

    // ACK bit
    sda_input(); scl_high(); i2c_delay();
    uint8_t ack = !(I2C_GPIO->IDR & (1U << SDA_PIN));
    scl_low(); sda_output(); i2c_delay();
    return ack;
}
