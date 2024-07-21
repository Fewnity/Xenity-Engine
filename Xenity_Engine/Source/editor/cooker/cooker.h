#pragma once

#include <memory>
#include <string>
#include <engine/platform.h>
#include <engine/reflection/reflection.h>
#include <engine/file_system/file_type.h>
#include <engine/file_system/data_base/file_data_base.h>

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
	static FileDataBase fileDataBase;

	static void CookAssets(const CookSettings& settings);
	static void CookAsset(const CookSettings& settings, const std::shared_ptr<FileReference>& fileReference, const std::string& exportFolderPath);
	static void CookAsset(const CookSettings& settings, const FileInfo& fileInfo, const std::string& exportFolderPath, const std::string& partialFilePath);
};

