#include "compiler_cache.h"

#include <filesystem>

#include <engine/reflection/enum_utils.h>
#include <engine/asset_management/project_manager.h>
#include <engine/debug/debug.h>

void CompilerCache::ClearCache()
{
}

void CompilerCache::UpdateCache(const std::string& source, Platform platform)
{
	std::string cachePath = GetCachePath(platform);
	std::filesystem::remove_all(cachePath);
	std::filesystem::create_directories(cachePath);

	std::filesystem::copy(source, cachePath, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
}

std::string CompilerCache::GetCachePath(Platform platform)
{
	const std::string enumString = EnumHelper::EnumAsString(platform);
	const std::string projectCache = ProjectManager::GetProjectFolderPath() + "cache/" + enumString + "/";
	Debug::Print("Cache path: " + projectCache);

	return projectCache;
}
