#include "spi.h"
#include "max7219.h"
#include "joystick.h"

/*Hardware connections
 Joystick Pins:
 -VCC  3.3V
 -GND
 -VRx PA0 (ADC CH0)  X-axis analog output
 -VRy PA1 (ADC CH1)  Y-axis analog output
 -SW Optional (e.g., PA2 with pull-up)  Button (click)
 ------------------------------------------------------------------------
 8x8 LED Matrix:
 -VCC
 -GND
 -DIN   PA7 (SPI1\_MOSI) | Data in
 -CLK   PA5 (SPI1\_SCK)  | Clock
 -CS    PA4 (SPI1\_NSS)  | Chip select (load/enable)
 * */

int main(void) {
    spi1_init();
    max7219_init();
    joystick_init();

    uint8_t x = 4, y = 4;

    while (1) {
        joystick_get_position(&x, &y);
        max7219_draw_dot(x, y);
        for (volatile int i = 0; i < 100000; i++); // crude delay

        // to check all dots light up
//        for (uint8_t y = 0; y < 8; y++)
//        {
//            for (uint8_t x = 0; x < 8; x++)
//            {
//            	max7219_clear();
//                max7219_send(8 - y, 1 << x);
//                for (volatile int i = 0; i < 100000; i++); // crude delay
//            }
//        }
    }
}
