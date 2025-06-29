#include "lcd.h"
#include "i2c.h"
#include "delay.h"

#define LCD_ADDR (0x27 << 1) // Common PCF8574 backpack address

// LCD control bits
#define LCD_BL  (1<<3)
#define LCD_EN  (1<<2)
#define LCD_RW  (1<<1)
#define LCD_RS  (1<<0)

static void lcd_write_nibble(uint8_t nibble);
static void lcd_send_cmd(uint8_t cmd);
static void lcd_send_data(uint8_t data);
static void lcd_toggle_enable(uint8_t data);

void lcd_init(void) {
    delay_ms(50);

    // Initialize 4-bit mode
    lcd_write_nibble(0x03);
    delay_ms(5);
    lcd_write_nibble(0x03);
    delay_ms(1);
    lcd_write_nibble(0x03);
    delay_ms(1);
    lcd_write_nibble(0x02);

    // Configure function, display etc.
    lcd_send_cmd(0x28); // 4-bit, 2 lines, 5×8
    lcd_send_cmd(0x0C); // Display on, cursor off
    lcd_send_cmd(0x06); // Entry mode
    lcd_clear();
}

void lcd_clear(void) {
    lcd_send_cmd(0x01);
    delay_ms(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t addr = (row == 0 ? 0x80 : 0xC0) + col;
    lcd_send_cmd(addr);
}

void lcd_send_string(const char* str) {
    while (*str) {
        lcd_send_data((uint8_t)*str++);
    }
}

static void lcd_write_nibble(uint8_t nibble) {
    uint8_t data = (nibble << 4) & 0xF0;
    i2c_start();
    i2c_write(LCD_ADDR);
    lcd_toggle_enable(data | LCD_BL);
    i2c_stop();
}

static void lcd_send_cmd(uint8_t cmd) {
    uint8_t high = cmd >> 4, low = cmd & 0x0F;
    i2c_start();
    i2c_write(LCD_ADDR);
    lcd_toggle_enable((high << 4) | LCD_BL);
    lcd_toggle_enable((low << 4)  | LCD_BL);
    i2c_stop();
}

static void lcd_send_data(uint8_t data) {
    uint8_t high = data >> 4, low = data & 0x0F;
    i2c_start();
    i2c_write(LCD_ADDR);
    lcd_toggle_enable((high << 4) | LCD_RS | LCD_BL);
    lcd_toggle_enable((low << 4)  | LCD_RS | LCD_BL);
    i2c_stop();
}

static void lcd_toggle_enable(uint8_t data) {
    i2c_write(data | LCD_EN);
    i2c_write(data & ~LCD_EN);
}


