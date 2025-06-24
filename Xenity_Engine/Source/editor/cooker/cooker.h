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
	[[nodiscard]] static bool CookAssets(const CookSettings& settings);

private:
	static FileDataBase s_fileDataBase;
	
	static void CookAsset(const CookSettings settings, const FileInfo fileInfo, const std::string exportFolderPath, const std::string partialFilePath);

	static void CookMesh(const CookSettings& settings, const FileInfo& fileInfo, const std::string& exportPath);
	static void CookShader(const CookSettings& settings, const FileInfo& fileInfo, const std::string& exportPath);
	static void CookTexture(const CookSettings& settings, const FileInfo& fileInfo, const std::string& exportPath);
};

