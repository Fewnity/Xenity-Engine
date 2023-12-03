#include "file_handler.h"
#include <Windows.h>
#include <filesystem>
#include <iostream>
#include <engine/debug/debug.h>

using namespace std::chrono;
uint64_t FileHandler::lastModifiedCodeFileTime;

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

		// Copy file
		std::string fileFolderPath = file.path().string();
		std::string fileName = file.path().filename().string();
		std::filesystem::file_time_type time = std::filesystem::last_write_time(fileFolderPath);
		auto duration = time.time_since_epoch();
		uint64_t c = duration.count();
		if (c > lastModifiedCodeFileTime) 
		{
			lastModifiedCodeFileTime = c;
			changed = true;
		}
	}
	return changed;
}