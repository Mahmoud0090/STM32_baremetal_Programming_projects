#include "stm32f4xx.h"
#include "adc.h"

void adc_init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;   // Enable GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;    // Enable ADC1

    // Set PA0 & PA1 to analog mode
    GPIOA->MODER |= (3U << (0 * 2)) | (3U << (1 * 2)); // PA0, PA1 analog
    GPIOA->PUPDR &= ~((3U << (0 * 2)) | (3U << (1 * 2))); // No pull

    ADC1->CR2 = 0;  // Disable for config
    ADC1->SQR1 = 0; // 1 conversion
    ADC1->CR2 |= ADC_CR2_ADON;
}

uint16_t adc_read(uint8_t channel) {
    ADC1->SQR3 = channel; // Select channel
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while (!(ADC1->SR & ADC_SR_EOC));
    return ADC1->DR;
}
