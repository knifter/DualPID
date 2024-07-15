#include "rtc.h"

#include <BM8563.h>

#include "tools-log.h"

BM8563 bmrtc;

struct tm today;
const char* rtc_month_str[] = {"Jan", "Feb", "Mar", "Apr", "Jun", "Jul", "Aug", "Sep", "Okt", "Nov", "Dec", "OVRFLW"};
const char* rtc_wday_str[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun", "OVRFLW"};

bool _available = false;
time_t _lastread = 0;

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

	bool ok = bmrtc.readDateTime(&today);

	return ok;
};

bool rtc_write()
{
	return bmrtc.writeDateTime(&today);
};
