#include "file_handler.h"
#include <Windows.h>
#include <filesystem>
#include <iostream>
#include <engine/debug/debug.h>

using namespace std::chrono;
uint64_t FileHandler::lastModifiedCodeFileTime;
uint64_t FileHandler::lastModifiedFileTime;
uint32_t FileHandler::lastFileCount;

bool FileHandler::HasCodeChanged(const std::string& path)
{
	bool changed = false;
	for (const auto& file : std::filesystem::directory_iterator(path))
	{
		// Check is file
		if (!file.is_regular_file()) continue;

		// Check extension
		std::string ext = file.path().extension().string();
		if (ext != ".h" && ext != ".cpp") continue;

		std::filesystem::file_time_type time = std::filesystem::last_write_time(file);

		// Check last date
		auto duration = time.time_since_epoch();
		uint64_t durationCount = duration.count();
		if (durationCount > lastModifiedCodeFileTime)
		{
			lastModifiedCodeFileTime = durationCount;
			changed = true;
		}
	}
	return changed;
}

bool FileHandler::HasFileChangedOrAdded(const std::string& path)
{
	bool changed = false;
	for (const auto& file : std::filesystem::directory_iterator(path))
	{
		// Check is file
		if (!file.is_regular_file())
		{
			bool temp = HasFileChangedOrAdded(file.path().string());
			if (temp) 
			{
				changed = true;
			}
		}
		else 
		{
			std::string ext = file.path().extension().string();

			if (ext != ".meta")
				continue;
		}

		// Check last date
		std::filesystem::file_time_type time = std::filesystem::last_write_time(file);
		auto duration = time.time_since_epoch();
		uint64_t durationCount = duration.count();
		if (durationCount > lastModifiedFileTime)
		{
			lastModifiedFileTime = durationCount;
			changed = true;
		}
	}
	return changed;
}
