#include "rtc.h"
#include "i2c.h"
#include "utils.h"

#define RTC_ADDR 0x68<<1 // DS3231 or DS1307

void rtc_init(void)
{
	// Optional: init control/status registers
}


void rtc_get_time(RTC_Time* time)
{
	i2c_start();
	i2c_write(RTC_ADDR);
	i2c_write(0x00); // seconds register
	i2c_restart();
	i2c_write(RTC_ADDR | 0x01);

	uint8_t raw_sec  = i2c_read_ack() & 0x7F; // Mask bit 7
	uint8_t raw_min  = i2c_read_ack() & 0x7F;
	uint8_t raw_hour = i2c_read_nack() & 0x3F; // Mask 6 & 7 (12/24-hour flags)

	i2c_stop();

	time->seconds = bcd_to_decimal(raw_sec);
	time->minutes = bcd_to_decimal(raw_min);
	time->hours   = bcd_to_decimal(raw_hour);
}

void rtc_set_time(RTC_Time* time)
{
    i2c_start();
    i2c_write(RTC_ADDR);
    i2c_write(0x00); // Start at seconds register

    i2c_write(decimal_to_bcd(time->seconds) & 0x7F); // Ensure CH bit is 0
    i2c_write(decimal_to_bcd(time->minutes) & 0x7F);
    i2c_write(decimal_to_bcd(time->hours) & 0x3F);   // 24-hour format

    i2c_stop();
}
