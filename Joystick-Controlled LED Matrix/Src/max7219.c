#include "stm32f4xx.h"
#include "max7219.h"
#include "spi.h"

#define CS_HIGH() (GPIOA->BSRR = GPIO_BSRR_BS4)
#define CS_LOW()  (GPIOA->BSRR = GPIO_BSRR_BR4)

uint8_t led_matrix[8]; // Column-wise display buffer

void max7219_send(uint8_t address, uint8_t data) {
    CS_LOW();
    spi1_write(address);
    spi1_write(data);
    CS_HIGH();
}

void max7219_init(void) {
    max7219_send(0x09, 0x00);
    max7219_send(0x0A, 0x0F);  // Brightness
    max7219_send(0x0B, 0x07);  // Scan limit (0–7)
    max7219_send(0x0C, 0x01);  // Normal mode
    max7219_send(0x0F, 0x00);  // Display test off
    max7219_clear();
}

void max7219_clear(void) {
    for (int i = 0; i < 8; i++) {
        led_matrix[i] = 0;
        max7219_send(i + 1, 0x00);
    }
}

// Draw 1 dot at x/y (x = column, y = row)
//void max7219_draw_dot(uint8_t x, uint8_t y) {
//
//	max7219_clear();
//    if (x > 8 || y > 8) return;
//
//    led_matrix[y] = 1 << x;
//    for (uint8_t row = 0; row < 8; row++) {
//        max7219_send(row + 1, led_matrix[row]);
//    }
//}

void max7219_draw_dot(uint8_t x, uint8_t y) {
    if (x < 8 && y < 8) {
    	max7219_clear();
        max7219_send(8 - y, 1 << x);  // Flip row, normal bit shift
    }
}
