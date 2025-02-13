#pragma once

#include <cstdint>
#include <string>

#include <engine/api.h>

class API DateTime
{
public:
	static DateTime GetNow();
	std::string ToString() const;
	uint32_t second = 0;
	uint32_t minute = 0;
	uint32_t hour = 0;
	uint32_t day = 0;
	uint32_t month = 0;
	uint32_t year = 0;
};

