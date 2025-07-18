#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>

// Call this once in main before using delays
void delay_init(void);

void delay_ms(uint32_t ms);
void delay_us(uint32_t us);

#endif
