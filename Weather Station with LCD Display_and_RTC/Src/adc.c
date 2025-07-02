#include "stm32f4xx.h"
#include "adc.h"

void adc_init(void)
{
    // Enable clocks for GPIOA and ADC1
    RCC->AHB1ENR |= (1U << 0);   // GPIOA
    RCC->APB2ENR |= (1U << 8);   // ADC1

    // Configure PA1 as analog (assume thermistor connected to PA1)
    GPIOA->MODER |= (3U << (2 * 1)); // Analog mode

    // ADC Common config (prescaler /4)
    ADC->CCR &= ~(3U << 16);
    ADC->CCR |=  (1U << 16);    // PCLK2 / 4

    // ADC1 Config
    ADC1->CR2 = 0;               // Clear CR2
    ADC1->SQR3 = 1;              // First conversion on channel 1 (PA1)
    ADC1->SMPR2 |= (3U << 3);    // Sampling time for channel 1 = 56 cycles
    ADC1->CR2 |= (1U << 0);      // Enable ADC1
}

uint16_t adc_read(uint8_t channel)
{
    // Select ADC channel
    ADC1->SQR3 = channel;

    // Start conversion
    ADC1->CR2 |= (1U << 30);

    // Wait until EOC (End Of Conversion)
    while (!(ADC1->SR & (1U << 1)));

    // Return converted value
    return ADC1->DR;
}


