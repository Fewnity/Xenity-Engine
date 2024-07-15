#include "cooker.h"
#include <string>
#include <engine/asset_management/project_manager.h>
#include <engine/unique_id/unique_id.h>
#include <engine/file_system/file_system.h>
#include <engine/file_system/file.h>
#include <engine/debug/debug.h>
#include <editor/utils/copy_utils.h>

#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_image_resize.h"

namespace fs = std::filesystem;

#define ASSETS_FOLDER "assets/"

void Cooker::CookAssets(const CookSettings& settings)
{
	const std::string projectAssetFolder = ProjectManager::GetProjectFolderPath() + ASSETS_FOLDER;
	const size_t projectFolderPathLen = projectAssetFolder.size();
	const std::vector<uint64_t> ids = ProjectManager::GetAllUsedFileByTheGame();
	const size_t idsCount = ids.size();
	for (size_t i = 0; i < idsCount; i++)
	{
		const FileInfo* fileInfo = ProjectManager::GetFileById(ids[i]);
		if (fileInfo)
		{
			if (fileInfo->file->GetUniqueId() <= UniqueId::reservedFileId)
				continue;

			if (fileInfo->path[0] == '.')
			{
				Debug::PrintError("[Compiler::ExportProjectFiles] Invalid file path (Maybe no meta file or wrong id?): " + fileInfo->path);
				continue;
			}

			const std::string newPath = fileInfo->path.substr(projectFolderPathLen, fileInfo->path.size() - projectFolderPathLen);
			/*AddCopyEntry(false, fileInfo->path, exportPath + ASSETS_FOLDER + newPath);
			AddCopyEntry(false, fileInfo->path + ".meta", exportPath + ASSETS_FOLDER + newPath + ".meta");*/
			std::string folderToCreate = (settings.exportPath + ASSETS_FOLDER + newPath);
			folderToCreate = folderToCreate.substr(0, folderToCreate.find_last_of('/'));
			fs::create_directories(folderToCreate);
			CookAsset(settings , *fileInfo, folderToCreate);
		}
	}

	CopyUtils::ExecuteCopyEntries();
}

void Cooker::CookAsset(const CookSettings& settings, const std::shared_ptr<FileReference>& fileReference, const std::string& exportFolderPath)
{
}

void Cooker::CookAsset(const CookSettings& settings, const FileInfo& fileInfo, const std::string& exportFolderPath)
{
	const std::string exportPath = exportFolderPath + "/" + fileInfo.file->GetFileName() + fileInfo.file->GetFileExtension();
	if (fileInfo.type == FileType::File_Texture) 
	{
		const std::string texturePath = fileInfo.path;
		//const std::string textureExportPath = exportPath;

		int width, height, channels;
		unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &channels, 0);
		if (!data)
		{
			Debug::PrintError("Failed to load texture: " + texturePath);
			return;
		}

		unsigned char* data2 = (unsigned char* )malloc((width /2) * (height / 2) * channels);
		stbir_resize_uint8(data, width, height, 0, data2, width / 2, height / 2, 0, channels);
		stbi_write_png(exportPath.c_str(), width / 2, height / 2, channels, data2, 0);

		stbi_image_free(data);

		/*FileSystem::WriteFile(textureExportPathData, textureData);
		FileSystem::WriteFile(textureExportPathDataMeta, "type: Texture\n");*/
	}
	else 
	{
		CopyUtils::AddCopyEntry(false, fileInfo.path, exportPath);
	}
	CopyUtils::AddCopyEntry(false, fileInfo.path + ".meta", exportPath + ".meta");
}