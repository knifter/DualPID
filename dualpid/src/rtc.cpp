#include "rtc.h"

#include <BM8563.h>

#include "tools-log.h"

BM8563 bmrtc;

struct tm today;
const char* rtc_month_str[] = {"Jan", "Feb", "Mar", "Apr", "Jun", "Jul", "Aug", "Sep", "Okt", "Nov", "Dec"};
const char* rtc_wday_str[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

bool _available = false;
time_t _lastread = 0;

void rtc_ext2int();
void rtc_int2ext();

bool rtc_begin()
{
	_available = false;
	_lastread = 0;

	if(!bmrtc.begin())
	{
		DBG("No RTC detected.");
		return false;
	};
	DBG("RTC (BM8563) Detected.");

	return _available = rtc_read();
};

// struct tm* rtc()
// {
// 	if(millis() - _lastread > 1000)
// 		return &today;
// 	_lastread = millis();
// 	rtc_read;
// 	return &today;
// };

bool rtc_available()
{
	return _available;
};

bool rtc_read()
{
	// We'll throttle the reads a bit
	if(millis() - _lastread < 1000)
		return true;
	_lastread = millis();

	// Get time/date from external RTC
	if(!bmrtc.readDateTime(&today))
	{
		ERROR("Read RTC failed.");
		return false;
	};

	// rtc_ext2int();

	return true;
};

void rtc_ext2int()
{
	// Set internal RTC from &today
	struct timeval tv;
	// struct timezone tz;
	tv.tv_usec = 0;
	localtime_r(&(tv.tv_sec), &today);
	settimeofday(&tv, nullptr);
};

void rtc_int2ext()
{
	// Fill &today from internal rtc
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	struct tm *tmp = localtime(&(tv.tv_sec));
	memcpy(&today, &tmp, sizeof(struct tm));

	// updated global 'today' still needs to be written to ext RTC!
};

// Write internal -> external RTC
bool rtc_write()
{
	return bmrtc.writeDateTime(today);
};
