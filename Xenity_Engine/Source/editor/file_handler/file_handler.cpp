// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "file_handler.h"

#include <filesystem>
#include <thread>

#include <engine/debug/debug.h>

using namespace std::chrono;

uint64_t FileHandler::s_lastModifiedCodeFileTime = 0;
uint64_t FileHandler::s_lastModifiedFileTime = 0;
uint32_t FileHandler::s_lastFileCount = 0;
uint32_t FileHandler::s_tempFileCount = 0;

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
		if (durationCount > s_lastModifiedCodeFileTime)
		{
			s_lastModifiedCodeFileTime = durationCount;
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

			s_tempFileCount++;

			// Check last date
			const std::filesystem::file_time_type time = std::filesystem::last_write_time(file);
			const auto duration = time.time_since_epoch();
			const uint64_t durationCount = duration.count();
			if (durationCount > s_lastModifiedFileTime)
			{
				s_lastModifiedFileTime = durationCount;
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
	s_tempFileCount = 0;
	bool result = HasFileChangedOrAddedRecursive(folderPath);
	if (s_tempFileCount != s_lastFileCount)
	{
		result = true;
	}
	s_lastFileCount = s_tempFileCount;

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
	if (durationCount > s_lastModifiedFileTime)
	{
		s_lastModifiedFileTime = durationCount;
	}
}

void FileHandler::RemoveOneFile()
{
	s_lastFileCount--;
}

void FileHandler::AddOneFile()
{
	s_lastFileCount++;
}
