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

bool FileHandler::HasCodeChangedDirect(const std::string& folderPath, bool isThreaded, std::function<void()> callback)
{
	bool changed = false;
	for (const auto& file : std::filesystem::directory_iterator(folderPath))
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
	if (isThreaded && changed) 
	{
		callback();
	}
	return changed;
}

bool FileHandler::HasFileChangedOrAddedRecursive(const std::string& folderPath)
{
	bool changed = false;
	try
	{
		for (const auto& file : std::filesystem::directory_iterator(folderPath))
		{
			// Check is file
			if (!file.is_regular_file())
			{
				const bool temp = HasFileChangedOrAddedRecursive(file.path().string());
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
	}
	catch (const std::exception&)
	{
		Debug::PrintError("[FileHandler::HasFileChangedOrAddedRecursive] failed to check if files have changed", true);
	}
	return changed;
}


bool FileHandler::HasFileChangedOrAddedDirect(const std::string& folderPath, bool isThreaded, std::function<void()> callback)
{
	tempFileCount = 0;
	bool result = HasFileChangedOrAddedRecursive(folderPath);
	if (tempFileCount != lastFileCount)
	{
		result = true;
	}
	lastFileCount = tempFileCount;

	if (isThreaded && result)
	{
		callback();
	}

	return result;
}

bool FileHandler::HasCodeChanged(const std::string& folderPath)
{
	return FileHandler::HasCodeChangedDirect(folderPath, false, std::function<void()>());
}

bool FileHandler::HasFileChangedOrAdded(const std::string& folderPath)
{
	return FileHandler::HasFileChangedOrAddedDirect(folderPath, false, std::function<void()>());
}

void FileHandler::HasCodeChangedThreaded(const std::string& folderPath, std::function<void()> callback)
{
	std::thread t  = std::thread(FileHandler::HasCodeChangedDirect, folderPath, true, callback);
	t.detach();
}

void FileHandler::HasFileChangedOrAddedThreaded(const std::string& folderPath, std::function<void()> callback)
{
	std::thread t = std::thread(FileHandler::HasFileChangedOrAddedDirect, folderPath, true, callback);
	t.detach();
}

void FileHandler::SetLastModifiedFile(const std::string& file)
{
	const std::filesystem::file_time_type time = std::filesystem::last_write_time(file);
	const auto duration = time.time_since_epoch();
	const uint64_t durationCount = duration.count();
	if (durationCount > lastModifiedFileTime)
	{
		lastModifiedFileTime = durationCount;
	}
}
