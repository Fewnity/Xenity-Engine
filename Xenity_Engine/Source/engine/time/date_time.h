#pragma once

#include <cstdint>
#include <string>

class DateTime
{
public:
	static DateTime GetNow();
	std::string ToString();
	uint32_t second;
	uint32_t minute;
	uint32_t hour;
	uint32_t day;
	uint32_t month;
	uint32_t year;
};

