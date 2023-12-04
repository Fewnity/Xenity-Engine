#pragma once

#include <string>

class FileHandler
{
public:
	static bool HasCodeChanged(const std::string& path);
	static bool HasFileChangedOrAdded(const std::string& path);
	static uint64_t lastModifiedCodeFileTime;
	static uint64_t lastModifiedFileTime;
	static uint32_t lastFileCount;
};

