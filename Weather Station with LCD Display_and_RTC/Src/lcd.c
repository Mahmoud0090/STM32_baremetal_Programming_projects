#include "lcd.h"
#include "i2c.h"
#include "delay.h"

#define LCD_ADDR  0x4E  // Change if your PCF8574 address is different

#define LCD_RS 0x01
#define LCD_EN 0x04
#define LCD_BL 0x08

static void lcd_send_cmd(uint8_t cmd);
static void lcd_send_data(uint8_t data);
static void lcd_write_nibble(uint8_t nibble);
static void lcd_toggle(uint8_t data);

void lcd_init(void) {
    i2c_init();
    delay_ms(50);

    lcd_write_nibble(0x30);
    delay_ms(5);
    lcd_write_nibble(0x30);
    delay_ms(5);
    lcd_write_nibble(0x30);
    delay_ms(1);
    lcd_write_nibble(0x20);  // 4-bit mode

    lcd_send_cmd(0x28);  // 2 lines, 5x8
    lcd_send_cmd(0x0C);  // Display on, cursor off
    lcd_send_cmd(0x06);  // Increment
    lcd_clear();
}

void lcd_clear(void) {
    lcd_send_cmd(0x01);
    delay_ms(2);  // Required
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    lcd_send_cmd(0x80 | (col + row_offsets[row % 2]));
}

void lcd_send_string(const char* str) {
    while (*str) {
        lcd_send_data(*str++);
    }
}

static void lcd_send_cmd(uint8_t cmd) {
    lcd_write_nibble((cmd & 0xF0));
    lcd_write_nibble((cmd << 4));
}

static void lcd_send_data(uint8_t data) {
    lcd_write_nibble((data & 0xF0) | LCD_RS);
    lcd_write_nibble((data << 4)  | LCD_RS);
}

static void lcd_write_nibble(uint8_t nibble) {
    lcd_toggle(nibble | LCD_BL);  // Backlight always on
}

static void lcd_toggle(uint8_t data) {
    i2c_start();
    i2c_write(LCD_ADDR);
    i2c_write(data | LCD_EN);  // EN=1
    delay_us(1);
    i2c_write(data & ~LCD_EN); // EN=0
    i2c_stop();
    delay_us(50);  // Wait for command
}
