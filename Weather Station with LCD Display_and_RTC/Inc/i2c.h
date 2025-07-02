#ifndef I2C_H
#define I2C_H

#include "stm32f4xx.h"

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
uint8_t i2c_write(uint8_t data);

#endif
