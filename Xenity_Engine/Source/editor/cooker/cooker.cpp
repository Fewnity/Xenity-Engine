#include "cooker.h"

#include <string>
#include <filesystem>
#include <fstream>
#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_image_resize.h>

#include <editor/utils/copy_utils.h>

#include <engine/asset_management/project_manager.h>
#include <engine/unique_id/unique_id.h>
#include <engine/file_system/file_system.h>
#include <engine/file_system/file.h>
#include <engine/graphics/texture.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/debug/debug.h>

namespace fs = std::filesystem;

#define ASSETS_FOLDER "assets/"

FileDataBase Cooker::fileDataBase;
using ordered_json = nlohmann::ordered_json;

void Cooker::CookAssets(const CookSettings& settings)
{
	fileDataBase.Clear();
	fileDataBase.bitFile.Create(settings.exportPath + "data.xenb");

	const std::string projectAssetFolder = ProjectManager::GetProjectFolderPath();
	const size_t projectFolderPathLen = projectAssetFolder.size();
	const std::vector<uint64_t> ids = ProjectManager::GetAllUsedFileByTheGame();
	const size_t idsCount = ids.size();
	for (size_t i = 0; i < idsCount; i++)
	{
		const FileInfo* fileInfo = ProjectManager::GetFileById(ids[i]);
		if (fileInfo)
		{
			std::string newPath;
			if (fileInfo->path[0] == '.')
			{
				newPath = fileInfo->path.substr(2);
			}
			else
			{
				newPath = fileInfo->path.substr(projectFolderPathLen, fileInfo->path.size() - projectFolderPathLen);
			}

			std::string folderToCreate = (settings.exportPath + newPath);
			folderToCreate = folderToCreate.substr(0, folderToCreate.find_last_of('/'));
			fs::create_directories(folderToCreate);

			CookAsset(settings, *fileInfo, folderToCreate, newPath);
		}
	}

	fileDataBase.SaveToFile(settings.exportPath + "db.bin");

}

void Cooker::CookAsset(const CookSettings& settings, const std::shared_ptr<FileReference>& fileReference, const std::string& exportFolderPath)
{
}

void Cooker::CookAsset(const CookSettings& settings, const FileInfo& fileInfo, const std::string& exportFolderPath, const std::string& partialFilePath)
{
	uint64_t cookedFileSize = 0;
	const std::string exportPath = exportFolderPath + "/" + fileInfo.file->GetFileName() + fileInfo.file->GetFileExtension();

	if (fileInfo.type == FileType::File_Texture)
	{
		const std::string texturePath = fileInfo.path;

		int width, height, channels;
		unsigned char* imageData = stbi_load(texturePath.c_str(), &width, &height, &channels, 4);
		if (!imageData)
		{
			Debug::PrintError("Failed to load texture: " + texturePath);
			return;
		}

		std::shared_ptr<FileReference> fileRef = ProjectManager::GetFileReferenceByFile(*fileInfo.file);
		std::shared_ptr<Texture> texture = std::dynamic_pointer_cast<Texture>(fileRef);
		TextureResolutions textureResolution = texture->m_settings[static_cast<int>(settings.platform)]->resolution;

		int newWidth = width;
		int newHeight = height;
		if ((newWidth > height) && newWidth > static_cast<int>(textureResolution))
		{
			newHeight = static_cast<int>(height * (static_cast<float>(textureResolution) / static_cast<float>(width)));
			newWidth = static_cast<int>(textureResolution);
		}
		else if ((newHeight > width) && newHeight > static_cast<int>(textureResolution))
		{
			newWidth = static_cast<int>(width * (static_cast<float>(textureResolution) / static_cast<float>(height)));
			newHeight = static_cast<int>(textureResolution);
		}
		else if ((newWidth == newHeight) && newWidth > static_cast<int>(textureResolution))
		{
			newWidth = static_cast<int>(textureResolution);
			newHeight = static_cast<int>(textureResolution);
		}

		//TODO do not resize if the texture is already at the correct size
		unsigned char* resizedImageData = (unsigned char*)malloc(newWidth * newHeight * 4);
		stbir_resize_uint8(imageData, width, height, 0, resizedImageData, newWidth, newHeight, 0, 4);
		stbi_write_png(exportPath.c_str(), newWidth, newHeight, 4, resizedImageData, 0);

		free(imageData);
		free(resizedImageData);

		cookedFileSize = fs::file_size(exportPath.c_str());
	}
	else if (fileInfo.type == FileType::File_Mesh)
	{
		std::shared_ptr<FileReference> fileRef = ProjectManager::GetFileReferenceByFile(*fileInfo.file);
		std::shared_ptr<MeshData> meshData = std::dynamic_pointer_cast<MeshData>(fileRef);
		meshData->LoadFileReference();

		std::ofstream meshFile = std::ofstream(exportPath, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

		meshFile.write((char*)&meshData->m_vertexDescriptor, sizeof(VertexElements));
		meshFile.write((char*)&meshData->m_subMeshCount, sizeof(uint32_t));

		for (auto subMesh : meshData->m_subMeshes)
		{
			meshFile.write((char*)&subMesh->vertice_count, sizeof(uint32_t));
			meshFile.write((char*)&subMesh->index_count, sizeof(uint32_t));
			meshFile.write((char*)&subMesh->vertexMemSize, sizeof(uint32_t));
			meshFile.write((char*)&subMesh->indexMemSize, sizeof(uint32_t));
			meshFile.write((char*)subMesh->data, subMesh->vertexMemSize);
			meshFile.write((char*)subMesh->indices, subMesh->indexMemSize);
		}
		meshFile.close();

		cookedFileSize = fs::file_size(exportPath.c_str());
	}
	else
	{
		CopyUtils::AddCopyEntry(false, fileInfo.path, exportPath);
		cookedFileSize = fs::file_size(fileInfo.path);
	}
	CopyUtils::AddCopyEntry(false, fileInfo.path + ".meta", exportPath + ".meta");
	uint64_t metaSize = fs::file_size(fileInfo.path + ".meta");

	CopyUtils::ExecuteCopyEntries();

	// Do not include audio in the binary file
	size_t dataOffset = 0;
	if (fileInfo.type != FileType::File_Audio)
	{
		std::shared_ptr<File> cookedFile = FileSystem::MakeFile(exportPath);
		int cookedFileSizeOut;
		cookedFile->Open(FileMode::ReadOnly);
		unsigned char* fileData = cookedFile->ReadAllBinary(cookedFileSizeOut);
		cookedFile->Close();
		dataOffset = fileDataBase.bitFile.AddData(fileData, cookedFileSize);
		delete[] fileData;
		FileSystem::s_fileSystem->Delete(exportPath);
	}

	size_t metaDataOffset = 0;
	std::shared_ptr<File> cookedMetaFile = FileSystem::MakeFile(exportPath + ".meta");
	int cookedMetaFileSizeOut;
	cookedMetaFile->Open(FileMode::ReadOnly);
	unsigned char* metaFileData = cookedMetaFile->ReadAllBinary(cookedMetaFileSizeOut);
	cookedMetaFile->Close();
	metaDataOffset = fileDataBase.bitFile.AddData(metaFileData, cookedMetaFileSizeOut);
	delete[] metaFileData;
	FileSystem::s_fileSystem->Delete(exportPath + ".meta");

	FileDataBaseEntry* fileDataBaseEntry = new FileDataBaseEntry();
	fileDataBaseEntry->p = partialFilePath;
	fileDataBaseEntry->id = fileInfo.file->GetUniqueId();
	fileDataBaseEntry->po = dataOffset;
	fileDataBaseEntry->s = cookedFileSize;
	fileDataBaseEntry->mpo = metaDataOffset;
	fileDataBaseEntry->ms = metaSize;
	fileDataBaseEntry->t = fileInfo.type;

	fileDataBase.AddFile(fileDataBaseEntry);
}