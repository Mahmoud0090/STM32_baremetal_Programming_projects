#include "stm32f4xx.h"
#include <stdio.h>

/*components required
 * potentiometer connected between 3.3v and GND , and middle leg to
 *PA0
 * 220 ohm resistor connected to PB4 and other side to led and led to GND*/

void delay_ms(uint32_t ms);
void uart2_init(void);
void uart2_write(char c);
void uart2_print(char *str);
void adc1_init(void);
uint16_t adc1_read(void);
void tim3_pwm_init(void);
void pwm_set_duty(uint16_t duty);

int main(void) {
    uart2_init();
    adc1_init();
    tim3_pwm_init();

    while (1) {
        uint16_t adc_val = adc1_read();          // 0 to 4095
        uint16_t duty = (adc_val * 100) / 4095;  // 0 to 100%

        pwm_set_duty(duty);

        // Print duty over UART
        char buffer[50];
        sprintf(buffer, "Duty Cycle: %d%%\r\n", duty);
        uart2_print(buffer);

        delay_ms(20);
    }
}
void delay_ms(uint32_t ms) {
    for(uint32_t i = 0; i < ms * 4000; i++) {
        __NOP();
    }
}
void uart2_init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // PA2 (TX) as AF7
    GPIOA->MODER &= ~(3 << (2*2));
    GPIOA->MODER |= (2 << (2*2));
    GPIOA->AFR[0] |= (7 << (4*2)); // AF7 for USART2

    USART2->BRR = 0x0683;  // 9600 baud @16MHz
    USART2->CR1 |= USART_CR1_TE | USART_CR1_UE;
}

void uart2_write(char c) {
    while (!(USART2->SR & USART_SR_TXE));
    USART2->DR = c;
}

void uart2_print(char *str) {
    while (*str) {
        uart2_write(*str++);
    }
}
void adc1_init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // PA0 -> analog mode
    GPIOA->MODER |= (3 << 0);

    ADC1->SQR3 = 0;           // Channel 0
    ADC1->SMPR2 |= (3 << 0);  // 56 cycles
    ADC1->CR2 |= ADC_CR2_ADON;
}

uint16_t adc1_read(void) {
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while (!(ADC1->SR & ADC_SR_EOC));
    return ADC1->DR;
}
void tim3_pwm_init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // PB4 -> AF2 (TIM3_CH1)
    GPIOB->MODER &= ~(3 << (4*2));
    GPIOB->MODER |= (2 << (4*2));
    GPIOB->AFR[0] |= (2 << (4*4));  // AF2

    TIM3->PSC = 160 - 1;     // 100 kHz base
    TIM3->ARR = 100 - 1;     // 1 kHz PWM
    TIM3->CCR1 = 0;

    TIM3->CCMR1 |= (6 << 4);  // PWM mode 1
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE;
    TIM3->CCER |= TIM_CCER_CC1E;
    TIM3->CR1 |= TIM_CR1_ARPE;
    TIM3->EGR |= TIM_EGR_UG;
    TIM3->CR1 |= TIM_CR1_CEN;
}

void pwm_set_duty(uint16_t duty) {
    if (duty > 100) duty = 100;
    TIM3->CCR1 = duty;
}
