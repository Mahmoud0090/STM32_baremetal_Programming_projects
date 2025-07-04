#ifndef LCD_H
#define LCD_H

#include <stdint.h>

void lcd_init(void);
void lcd_clear(void);
void lcd_send_string(const char* str);
void lcd_set_cursor(uint8_t row, uint8_t col);

#endif
