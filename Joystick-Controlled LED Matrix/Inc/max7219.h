#ifndef MAX7219_H_
#define MAX7219_H_

void max7219_init(void);
void max7219_send(uint8_t address, uint8_t data);
void max7219_clear(void);
void max7219_draw_dot(uint8_t x, uint8_t y);

#endif
