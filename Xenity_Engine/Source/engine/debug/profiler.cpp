#include "profiler.h"

#include <engine/time/date_time.h>
#include <engine/debug/performance.h>

bool Profiler::SaveProfilingData(const std::string& name)
{
	if (!Performance::IsProfilerEnabled()) 
	{
		return false; // If the profiler is not enabled, do nothing
	}

	std::string path = "";
	if(name.empty())
	{
		path = "profiler";
		DateTime now = DateTime::GetNow();
		path += std::to_string(now.hour) + "h " + std::to_string(now.minute) + "m " + std::to_string(now.second) + "s " + std::to_string(now.day) + "d " + std::to_string(now.month) + "m " + std::to_string(now.year) + "y";
	}
	else
	{
		path = name;
	}
	path += ".xenp";
#if defined(__vita__)
	path = PSVITA_DEBUG_LOG_FOLDER + path;
#endif
	Performance::nextProfilerFileName = path;

	return true;
}
