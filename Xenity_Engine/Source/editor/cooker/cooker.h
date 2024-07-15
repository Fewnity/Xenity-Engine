#pragma once

#include <memory>
#include <string>
#include <engine/platform.h>

class FileReference;
struct FileInfo;

struct CookSettings 
{
	AssetPlatform platform;
	std::string exportPath;
};

class Cooker
{
public:
	static void CookAssets(const CookSettings& settings);
	static void CookAsset(const CookSettings& settings, const std::shared_ptr<FileReference>& fileReference, const std::string& exportFolderPath);
	static void CookAsset(const CookSettings& settings, const FileInfo& fileInfo, const std::string& exportFolderPath);
};

