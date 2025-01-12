#pragma once

#include <memory>
#include <string>

#include <engine/platform.h>
#include <engine/file_system/data_base/file_data_base.h>

class FileReference;
struct FileInfo;

struct CookSettings 
{
	Platform platform;
	AssetPlatform assetPlatform;
	std::string exportPath;
	bool exportShadersOnly = false;
};

class Cooker
{
public:
	static FileDataBase fileDataBase;

	static void CookAssets(const CookSettings& settings);
	static void CookAsset(const CookSettings& settings, const FileInfo& fileInfo, const std::string& exportFolderPath, const std::string& partialFilePath);
};

