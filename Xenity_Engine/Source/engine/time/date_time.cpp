#include "date_time.h"

#include <ctime>

DateTime DateTime::GetNow()
{
    DateTime dateTime;

    std::time_t t = std::time(0);   // get time now
    std::tm* now = localtime(&t);

	dateTime.second = now->tm_sec;
	dateTime.minute = now->tm_min;
	dateTime.hour = now->tm_hour;
	dateTime.day = now->tm_mday;
	dateTime.month = now->tm_mon+1;
	dateTime.year = now->tm_year + 1900;

    return dateTime;
}

std::string DateTime::ToString()
{
	return std::to_string(second) + "s " + std::to_string(minute) + "m " + std::to_string(hour) + "h " + std::to_string(day) + "/" + std::to_string(month) + "/" + std::to_string(year);
}
