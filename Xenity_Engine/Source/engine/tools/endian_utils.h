#pragma once

#include <stdint.h>
#include <cstddef>

/**
* @brief Utility class for endian conversion.
*/
class EndianUtils
{
public:
	/*
	* @brief Checks if the system is big-endian.
	*/
	static bool IsBigEndian();

	/*
	* @brief Converts an type from little-endian to big-endian or vice versa.
	* @brief Only use simple types like int32_t, uint16_t, etc. Do not use this for complex types like structs or classes.
	*/
	template <typename T>
	static T SwapEndian(T value)
	{
		union
		{
			T value;
			unsigned char u8[sizeof(T)];
		} source, dest;

		source.value = value;

		for (size_t k = 0; k < sizeof(T); k++)
		{
			dest.u8[k] = source.u8[sizeof(T) - k - 1];
		}

		return dest.value;
	}
};

