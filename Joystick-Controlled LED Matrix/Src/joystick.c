#include "adc.h"
#include "joystick.h"
//#include "max7219.h"
#include <stdint.h>


void joystick_init(void) {
    adc_init();  // Just initialize ADC
}

void joystick_get_position(uint8_t* x_pos, uint8_t* y_pos) {
    uint32_t x_adc = 0, y_adc = 0;
    for (int i = 0; i < 10; i++) {
        x_adc += adc_read(0);
        y_adc += adc_read(1);
    }
    x_adc /= 10;
    y_adc /= 10;

    uint8_t x = (x_adc * 8) / 4096;
    uint8_t y = (y_adc * 8) / 4096;

    if (x > 7) x = 7;
    if (y > 7) y = 7;

    *x_pos = x;
    *y_pos = y;
}




