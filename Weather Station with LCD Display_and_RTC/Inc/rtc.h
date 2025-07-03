/*
 * rtc.h
 *
 *  Created on: Jun 28, 2025
 *      Author: User
 */

#ifndef RTC_H_
#define RTC_H_

#include <stdint.h>

typedef struct{
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
}RTC_Time;

void rtc_init(void);
void rtc_get_time(RTC_Time* time);
void rtc_set_time(RTC_Time* time);

#endif /* RTC_H_ */
