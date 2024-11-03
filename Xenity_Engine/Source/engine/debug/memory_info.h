#pragma once

#include <cstdint>
#include <cstddef>

/**
* @brief Class to get the memory usage of the current process
*/
class MemoryInfo
{
public:
	/**
	* Initialize the memory profiler
	*/
	static void Init();

	/**
	* Get the used memory in bytes
	*/
	static size_t GetUsedMemory();

	/**
	* Get the total memory in bytes
	*/
	static size_t GetTotalMemory();

	/**
	* Get the used video memory in bytes
	*/
	static size_t GetUsedVideoMemory();
	
	/**
	* Get the total video memory in bytes
	*/
	static size_t GetTotalVideoMemory();
};

