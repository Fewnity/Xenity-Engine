#include "file_handler.h"
#include <Windows.h>
#include <filesystem>
#include <iostream>
#include <engine/debug/debug.h>

using namespace std::chrono;

uint64_t FileHandler::lastModifiedCodeFileTime = 0;
uint64_t FileHandler::lastModifiedFileTime = 0;
uint32_t FileHandler::lastFileCount = 0;
uint32_t FileHandler::tempFileCount = 0;

bool FileHandler::HasCodeChanged(const std::string& path)
{
	bool changed = false;
	for (const auto& file : std::filesystem::directory_iterator(path))
	{
		// Check is file
		if (!file.is_regular_file()) continue;

		// Check extension
		const std::string ext = file.path().extension().string();
		if (ext != ".h" && ext != ".cpp") continue;

		const std::filesystem::file_time_type time = std::filesystem::last_write_time(file);

		// Check last date
		const auto duration = time.time_since_epoch();
		const uint64_t durationCount = duration.count();
		if (durationCount > lastModifiedCodeFileTime)
		{
			lastModifiedCodeFileTime = durationCount;
			changed = true;
		}
	}
	return changed;
}

bool FileHandler::HasFileChangedOrAddedRecursive(const std::string& path)
{
	bool changed = false;
	for (const auto& file : std::filesystem::directory_iterator(path))
	{
		// Check is file
		if (!file.is_regular_file())
		{
			const bool temp = HasFileChangedOrAdded(file.path().string());
			if (temp)
			{
				changed = true;
			}
		}
		else
		{
			const std::string ext = file.path().extension().string();

			if (ext != ".meta")
				continue;
		}
		
		tempFileCount++;

		// Check last date
		const std::filesystem::file_time_type time = std::filesystem::last_write_time(file);
		const auto duration = time.time_since_epoch();
		const uint64_t durationCount = duration.count();
		if (durationCount > lastModifiedFileTime)
		{
			lastModifiedFileTime = durationCount;
			changed = true;
		}
	}
	return changed;
}


bool FileHandler::HasFileChangedOrAdded(const std::string& path)
{
	tempFileCount = 0;
	bool result = HasFileChangedOrAddedRecursive(path);
	if (tempFileCount != lastFileCount) 
	{
		result = true;
	}
	lastFileCount = tempFileCount;
	return result;
}
