#include "stm32f4xx.h"
#include "adc.h"
#include "lcd.h"
#include "delay.h"
#include <stdio.h>
#include <math.h>

// UART helper functions
void uart2_init(void);
void uart2_write(char c);
void uart2_print(const char* s);

// Thermistor parameters
#define R_FIXED   10000.0     // 10k resistor
#define BETA      3950.0
#define R0        10000.0
#define T0        298.15      // 25°C in Kelvin
#define VREF      3.3

char buffer[64];
char lcd_buf[16];

// Convert ADC reading to temperature (Celsius)
float thermistor_get_temp(uint16_t adc_val)
{
    float vout = (adc_val / 4095.0f) * VREF;
    float r_thermistor = (R_FIXED * vout) / (VREF - vout);

    float tempK = 1.0f / (1.0f / T0 + logf(r_thermistor / R0) / BETA);
    float tempC = tempK - 273.15f;

    return tempC;
}

int main(void)
{
	SCB->CPACR |= (0xF << 20); //  Enable FPU

    uint16_t adc_val;
    float temperature;
    delay_init();      // if you use SysTick-based delay

    lcd_init();

    //lcd_send_string("Hello!");

	uart2_init();
	adc_init();       // for thermistor



    while (1)
    {
        adc_val = adc_read(1); // Read from ADC channel 1 (PA1)
        temperature = thermistor_get_temp(adc_val);

        sprintf(buffer, "Temp: %.2f C\r\n", temperature);
        uart2_print(buffer);

        // LCD output
		//lcd_clear();
		lcd_set_cursor(0, 0);
		sprintf(lcd_buf, "Temp: %.2f C", temperature);
		lcd_buf[15] = '\0';  // Ensure null-termination (if buffer is small)
		lcd_send_string(lcd_buf);

		delay_ms(1000); // Update every 1 second; // delay
    }
}

// === UART2 CONFIG ===
void uart2_init(void)
{
    RCC->AHB1ENR |= (1U << 0);     // GPIOA
    RCC->APB1ENR |= (1U << 17);    // USART2

    // PA2 = TX (AF7)
    GPIOA->MODER &= ~(3U << (2 * 2));
    GPIOA->MODER |=  (2U << (2 * 2));
    GPIOA->AFR[0]  &= ~(0xF << (4 * 2));
    GPIOA->AFR[0]  |=  (7U << (4 * 2));

    USART2->BRR = 0x008A;  // 115200 @ 16 MHz
    USART2->CR1 |= (1U << 3);  // TE
    USART2->CR1 |= (1U << 13); // UE
}

void uart2_write(char c)
{
    while (!(USART2->SR & (1 << 7))); // Wait for TXE
    USART2->DR = c;
}

void uart2_print(const char* s)
{
    while (*s)
        uart2_write(*s++);
}
