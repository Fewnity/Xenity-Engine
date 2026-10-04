#pragma once

#include <memory>
#include <string>
#include <set>
#include <vector>

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

	/**
	* @brief [Main thread only] List and create the file references used by the game before cooking in another thread.
	* Creating file references modifies the asset manager lists, it must not be done by the build threads while the editor is running
	*/
	static void PrepareCooking();

	/**
	* @brief Release the data created by PrepareCooking
	*/
	static void ClearPreparedCooking();

private:
	static std::set<uint64_t> s_preparedFileIds;
	static std::vector<std::shared_ptr<FileReference>> s_preparedFileReferences;
	static bool s_isCookingPrepared;

	static FileDataBase s_fileDataBase;
	
	static void CookAsset(const CookSettings settings, const FileInfo fileInfo, const std::string exportFolderPath, const std::string partialFilePath);

	static void CookMesh(const CookSettings& settings, const FileInfo& fileInfo, const std::string& exportPath);
	static void CookShader(const CookSettings& settings, const FileInfo& fileInfo, const std::string& exportPath);
	static void CookTexture(const CookSettings& settings, const FileInfo& fileInfo, const std::string& exportPath);
};

