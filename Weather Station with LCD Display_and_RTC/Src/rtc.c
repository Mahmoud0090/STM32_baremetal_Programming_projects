#include "rtc.h"

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

	time->seconds = bcd_to_decimal(i2c_read_ack());
	time->minutes = bcd_to_decimal(i2c_read_ack());
	time->hours = bcd_to_decimal(i2c_read_nack());

	i2c_stop();
}
