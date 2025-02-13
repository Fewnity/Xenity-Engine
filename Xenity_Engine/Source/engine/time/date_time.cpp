#include "date_time.h"

#if defined(__PSP__)
#include <psprtc.h>
#else
#include <ctime>
#endif

DateTime DateTime::GetNow()
{
    DateTime dateTime;
#if defined(__PSP__)
	ScePspDateTime time;
	sceRtcGetCurrentClockLocalTime(&time);
	dateTime.second = time.second;
	dateTime.minute = time.minute;
	dateTime.hour = time.hour;
	dateTime.day = time.day;
	dateTime.month = time.month;
	dateTime.year = time.year;
#else
    std::time_t t = std::time(0);   // get time now
    std::tm* now = localtime(&t);

	dateTime.second = now->tm_sec;
	dateTime.minute = now->tm_min;
	dateTime.hour = now->tm_hour;
	dateTime.day = now->tm_mday;
	dateTime.month = now->tm_mon + 1;
	dateTime.year = now->tm_year + 1900;
#endif
    return dateTime;
}

std::string DateTime::ToString() const
{
	return std::to_string(hour) + ":" + std::to_string(minute) + ":" + std::to_string(second) + " " + std::to_string(day) + "/" + std::to_string(month) + "/" + std::to_string(year);
}
