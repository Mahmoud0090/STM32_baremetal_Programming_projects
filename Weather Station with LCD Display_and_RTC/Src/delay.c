#include "stm32f4xx.h"
#include "delay.h"

// Assumes system clock = 16 MHz
#define SYSCLK_HZ 16000000U

void delay_init(void) {
    // SysTick configuration for 1us tick
    SysTick->LOAD = (SYSCLK_HZ / 1000000U) - 1;  // 1us reload
    SysTick->VAL = 0;                            // Clear current value
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
}

void delay_us(uint32_t us) {
    while (us--) {
        // Wait for COUNTFLAG to set (indicates 1us passed)
        while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    }
}

void delay_ms(uint32_t ms) {
    while (ms--) {
        delay_us(1000);
    }
}


