#ifndef __RTC_H
#define __RTC_H

#include <BM8563.h>

extern struct tm today;
extern const char* rtc_month_str[];
extern const char* rtc_wday_str[];

bool rtc_begin();
bool rtc_available();
bool rtc_read();
bool rtc_write();
// struct tm* rtc();

#endif // __RTC_H