#pragma once

#include <string>

#include <engine/api.h>

class API Profiler
{
public:
	/**
	* @brief Save profiling data to a file at the end of the frame
	* @param name Name of the profiling data file without the extension, if empty, the name will be "profiler" + the date and time
	* @return True if the data can be saved, false if the profiler is disabled
	*/
	static bool SaveProfilingData(const std::string& name = "");
};

