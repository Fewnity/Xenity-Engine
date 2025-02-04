#pragma once

#include <string>

#include <engine/platform.h>

class CompilerCache
{
public:
	/**
	* @brief Clear the cache of each platform
	*/
	static void ClearCache();

	/**
	* @brief Update the platform cache from the docker compilation folder
	*/
	static void UpdateCache(const std::string& source, Platform platform);

	static bool CheckIfCacheExists(Platform platform);

	static std::string GetCachePath(Platform platform);
};

