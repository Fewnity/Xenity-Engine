#pragma once

#include <cstdint>
#include <string>

#include <engine/api.h>

class API DateTime
{
public:

	/**
	* @brief Get the current date and time
	*/
	static DateTime GetNow();

	/**
	* @brief Get a string representation of the date and time in the format: "hour:minute:second day/month/year"
	*/
	std::string ToString() const;

	uint32_t second = 0;
	uint32_t minute = 0;
	uint32_t hour = 0;
	uint32_t day = 0;
	uint32_t month = 0;
	uint32_t year = 0;
};

