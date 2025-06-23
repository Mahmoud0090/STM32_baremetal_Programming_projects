#include "stm32f4xx.h"

void delay_ms(uint32_t ms);
void tim3_pwm_rgb_init(void);
void pwm_set_rgb(uint8_t r, uint8_t g, uint8_t b);

int main(void) {
	tim3_pwm_rgb_init();

	while (1) {
		// Simple color fade loop
		for (uint8_t i = 0; i <= 100; i++) {
			pwm_set_rgb(i, 100 - i, i / 2);  // Fade R up, G down, B mid
			delay_ms(20);
		}
	}
}

void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 4000; i++) {
        __NOP();
    }
}

void tim3_pwm_rgb_init(void) {
    // Enable GPIOB and TIM3
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // PB4 (CH1), PB5 (CH2), PB0 (CH3) to AF2
    GPIOB->MODER &= ~((3 << (4*2)) | (3 << (5*2)) | (3 << (0*2)));
    GPIOB->MODER |=  (2 << (4*2)) | (2 << (5*2)) | (2 << (0*2));  // AF mode

    GPIOB->AFR[0] |= (2 << (4*4)) | (2 << (5*4)) | (2 << (0*4));  // AF2 for TIM3

    // Timer setup: PWM at ~1 kHz
    TIM3->PSC = 160 - 1;     // Prescaler (16MHz / 160 = 100kHz timer clock)
    TIM3->ARR = 100 - 1;     // Auto-reload for 1 kHz frequency

    // CH1 - PB4
    TIM3->CCMR1 |= (6 << 4) | TIM_CCMR1_OC1PE;
    TIM3->CCER |= TIM_CCER_CC1E;

    // CH2 - PB5
    TIM3->CCMR1 |= (6 << 12) | TIM_CCMR1_OC2PE;
    TIM3->CCER |= TIM_CCER_CC2E;

    // CH3 - PB0
    TIM3->CCMR2 |= (6 << 4) | TIM_CCMR2_OC3PE;
    TIM3->CCER |= TIM_CCER_CC3E;

    // Enable preload and start timer
    TIM3->CR1 |= TIM_CR1_ARPE;
    TIM3->EGR |= TIM_EGR_UG;
    TIM3->CR1 |= TIM_CR1_CEN;
}

void pwm_set_rgb(uint8_t r, uint8_t g, uint8_t b) {
    if (r > 100) r = 100;
    if (g > 100) g = 100;
    if (b > 100) b = 100;

    TIM3->CCR1 = r;  // Red
    TIM3->CCR2 = g;  // Green
    TIM3->CCR3 = b;  // Blue
}
