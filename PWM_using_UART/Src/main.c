#include "stm32f4xx.h"
#include <stdint.h>

/*connection :
 * PB4 to red led , and terminal program (Tera Term, PuTTY, etc.) at 9600 baud.
 * the input using terminal should be between 0-9 for 0 to 90 duty cycle*/

// === Function Declarations ===
void delay_ms(uint32_t ms);
void uart2_init(void);
void uart2_write(char c);
void uart2_print(char *s);
void uart2_print_int(uint8_t val);
char uart2_read(void);
void pwm_init(void);
void set_pwm_duty(uint8_t duty);

// === main ===
int main(void) {
    uart2_init();
    pwm_init();

    uint8_t brightness = 0;

    uart2_print("Ready. Send 0-9 for brightness\n");

    while (1) {
        char c = uart2_read();

        if (c >= '0' && c <= '9') {
            brightness = (c - '0') * 10;
            set_pwm_duty(brightness);

            uart2_print("Duty: ");
            uart2_print_int(brightness);
            uart2_print("\n\r");
        }
    }
}

// === Function Implementations ===

// Delay (very approximate)
void delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 4000; i++) {
        __NOP();
    }
}

// UART2 Initialization
void uart2_init(void) {
    RCC->AHB1ENR  |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR  |= RCC_APB1ENR_USART2EN;

    GPIOA->MODER  &= ~((3 << (2 * 2)) | (3 << (3 * 2)));
    GPIOA->MODER  |=  (2 << (2 * 2)) | (2 << (3 * 2));  // AF mode
    GPIOA->AFR[0] |=  (7 << (2 * 4)) | (7 << (3 * 4));  // AF7 = USART2

    USART2->BRR    = 0x0683;  // 9600 baud @ 16 MHz
    USART2->CR1   |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;  // Enable RX, TX, USART
}

// Send one byte over UART
void uart2_write(char c) {
    while (!(USART2->SR & USART_SR_TXE));
    USART2->DR = c;
}

// Send string over UART
void uart2_print(char *s) {
    while (*s) {
        uart2_write(*s++);
    }
}

// Convert integer to ASCII and print
void uart2_print_int(uint8_t val) {
    char buf[4];
    int i = 0;

    if (val == 0) {
        uart2_write('0');
        return;
    }

    while (val > 0) {
        buf[i++] = (val % 10) + '0';
        val /= 10;
    }

    while (i--) {
        uart2_write(buf[i]);
    }
}

// Receive one byte from UART
char uart2_read(void) {
    while (!(USART2->SR & USART_SR_RXNE));
    return USART2->DR;
}

// PWM Initialization on TIM3_CH1 (PB4)
void pwm_init(void) {
    RCC->APB1ENR  |= RCC_APB1ENR_TIM3EN;
    RCC->AHB1ENR  |= RCC_AHB1ENR_GPIOBEN;

    GPIOB->MODER  &= ~(3 << (4 * 2));
    GPIOB->MODER  |=  (2 << (4 * 2));   // AF mode
    GPIOB->AFR[0] |=  (2 << (4 * 4));   // AF2 = TIM3_CH1

    TIM3->PSC     = 1599;   // Prescaler → 10kHz (16MHz / 1600)
    TIM3->ARR     = 100;    // Period → 0–100 for 1% resolution
    TIM3->CCR1    = 0;      // Start at 0% duty

    TIM3->CCMR1  |= (6 << 4) | TIM_CCMR1_OC1PE;
    TIM3->CCER   |= TIM_CCER_CC1E;
    TIM3->CR1    |= TIM_CR1_ARPE | TIM_CR1_CEN;
}

// Set PWM duty cycle (0–100)
void set_pwm_duty(uint8_t duty) {
    if (duty > 100) duty = 100;
    TIM3->CCR1 = duty;
}
