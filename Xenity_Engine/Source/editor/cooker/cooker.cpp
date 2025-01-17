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
#include <engine/graphics/shader.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/debug/debug.h>

namespace fs = std::filesystem;

#define ASSETS_FOLDER "assets/"

FileDataBase Cooker::fileDataBase;
using ordered_json = nlohmann::ordered_json;

void Cooker::CookAssets(const CookSettings& settings)
{
	fileDataBase.Clear();
	fileDataBase.GetBitFile().Create(settings.exportPath + "data.xenb");

	const std::string projectAssetFolder = ProjectManager::GetProjectFolderPath();
	const size_t projectFolderPathLen = projectAssetFolder.size();
	const std::set<uint64_t> ids = ProjectManager::GetAllUsedFileByTheGame();

	// Find file from id and cook it
	for (uint64_t id : ids)
	{
		const FileInfo* fileInfo = ProjectManager::GetFileById(id);
		if (fileInfo)
		{
			std::string newPath;
			if (fileInfo->file->GetPath()[0] == '.')
			{
				newPath = fileInfo->file->GetPath().substr(2);
			}
			else
			{
				newPath = fileInfo->file->GetPath().substr(projectFolderPathLen, fileInfo->file->GetPath().size() - projectFolderPathLen);
			}

			// Create the destination folder for the file
			std::string folderToCreate = (settings.exportPath + newPath);
			folderToCreate = folderToCreate.substr(0, folderToCreate.find_last_of('/'));
			fs::create_directories(folderToCreate);

			CookAsset(settings, *fileInfo, folderToCreate, newPath);
		}
	}
	IntegrityState integrityState = fileDataBase.CheckIntegrity();
	if (integrityState != IntegrityState::Integrity_Ok)
	{
		Debug::PrintError("[Cooker::CookAssets] Data base integrity check failed");
	}
	fileDataBase.SaveToFile(settings.exportPath + "db.xenb");
	fileDataBase.GetBitFile().Close();
}

void Cooker::CookAsset(const CookSettings& settings, const FileInfo& fileInfo, const std::string& exportFolderPath, const std::string& partialFilePath)
{
	const std::string exportPath = exportFolderPath + "/" + fileInfo.file->GetFileName() + fileInfo.file->GetFileExtension();

	if (fileInfo.type != FileType::File_Shader && settings.exportShadersOnly)
	{
		return;
	}

	// We copy the cooked file to the export folder, and then we add the copied file to the binary file
	if (fileInfo.type == FileType::File_Texture) // Cook texture
	{
		const std::string texturePath = fileInfo.file->GetPath();

		int width, height, channels;
		unsigned char* imageData = stbi_load(texturePath.c_str(), &width, &height, &channels, 4);
		if (!imageData)
		{
			Debug::PrintError("[Cooker::CookAsset] Failed to load texture: " + texturePath);
			return;
		}

		const std::shared_ptr<FileReference> fileRef = ProjectManager::GetFileReferenceByFile(*fileInfo.file);
		const std::shared_ptr<Texture> texture = std::dynamic_pointer_cast<Texture>(fileRef);
		TextureResolutions textureResolution = texture->m_settings[settings.assetPlatform]->resolution;

		int newWidth = width;
		int newHeight = height;
		const int textureResolutionInt = static_cast<int>(textureResolution);
		if ((newWidth > height) && newWidth > textureResolutionInt)
		{
			newWidth = textureResolutionInt;
			newHeight = static_cast<int>(height * (static_cast<float>(textureResolution) / static_cast<float>(width)));
		}
		else if ((newHeight > width) && newHeight > textureResolutionInt)
		{
			newWidth = static_cast<int>(width * (static_cast<float>(textureResolution) / static_cast<float>(height)));
			newHeight = textureResolutionInt;
		}
		else if ((newWidth == newHeight) && newWidth > textureResolutionInt)
		{
			newWidth = textureResolutionInt;
			newHeight = textureResolutionInt;
		}

		//TODO do not resize if the texture is already at the correct size
		unsigned char* resizedImageData = (unsigned char*)malloc(newWidth * newHeight * 4);
		stbir_resize_uint8(imageData, width, height, 0, resizedImageData, newWidth, newHeight, 0, 4);
		stbi_write_png(exportPath.c_str(), newWidth, newHeight, 4, resizedImageData, 0);

		free(imageData);
		free(resizedImageData);
	}
	else if (fileInfo.type == FileType::File_Mesh) // Cook mesh
	{
		const std::shared_ptr<FileReference> fileRef = ProjectManager::GetFileReferenceByFile(*fileInfo.file);

		MeshData meshData = MeshData();
		meshData.m_file = fileInfo.file;
		FileReference::LoadOptions loadOptions;
		loadOptions.platform = settings.platform;
		loadOptions.threaded = false;
		meshData.LoadFileReference(loadOptions);
		// REMINDER: NEVER WRITE A SIZE_T TO A FILE, ALWAYS CONVERT IT TO A FIXED SIZE TYPE
		std::ofstream meshFile = std::ofstream(exportPath, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

		// Write mesh data
		meshFile.write((char*)&meshData.m_vertexDescriptor, sizeof(VertexElements));
		meshFile.write((char*)&meshData.m_subMeshCount, sizeof(uint32_t));

		// Write submeshes data
		for (std::unique_ptr<MeshData::SubMesh>& subMesh : meshData.m_subMeshes)
		{
			meshFile.write((char*)&subMesh->vertice_count, sizeof(uint32_t));
			meshFile.write((char*)&subMesh->index_count, sizeof(uint32_t));
			meshFile.write((char*)&subMesh->vertexMemSize, sizeof(uint32_t));
			meshFile.write((char*)&subMesh->indexMemSize, sizeof(uint32_t));
			meshFile.write((char*)subMesh->data, subMesh->vertexMemSize);
			meshFile.write((char*)subMesh->indices, subMesh->indexMemSize);
		}
		meshFile.close();
	}
	else if (fileInfo.type == FileType::File_Shader) // Cook shader
	{
		if (settings.assetPlatform == AssetPlatform::AP_PS3)
		{
			if (settings.exportShadersOnly)
			{
				if (!std::filesystem::exists(settings.exportPath + "shaders_to_compile/"))
				{
					std::filesystem::create_directories(settings.exportPath + "shaders_to_compile/");
				}

				const std::shared_ptr<FileReference> fileRef = ProjectManager::GetFileReferenceByFile(*fileInfo.file);
				const std::shared_ptr<Shader> shader = std::dynamic_pointer_cast<Shader>(fileRef);

				const std::string vertexShaderCode = shader->GetShaderCode(Shader::ShaderType::Vertex_Shader, Platform::P_PS3);
				const std::string fragmentShaderCode = shader->GetShaderCode(Shader::ShaderType::Fragment_Shader, Platform::P_PS3);

				if (vertexShaderCode.empty() || fragmentShaderCode.empty())
				{
					Debug::PrintError("[Cooker::CookAsset] Failed to get shader code for shader: " + partialFilePath);
					return;
				}

				const std::shared_ptr<File> vertexFile = FileSystem::MakeFile(settings.exportPath + "shaders_to_compile/" + std::to_string(fileRef->GetFileId()) + ".vcg");
				vertexFile->Open(FileMode::WriteCreateFile);
				vertexFile->Write(vertexShaderCode);
				vertexFile->Close();

				const std::shared_ptr<File> fragmentFile = FileSystem::MakeFile(settings.exportPath + "shaders_to_compile/" + std::to_string(fileRef->GetFileId()) + ".fcg");
				fragmentFile->Open(FileMode::WriteCreateFile);
				fragmentFile->Write(fragmentShaderCode);
				fragmentFile->Close();

				CopyUtils::AddCopyEntry(false, fileInfo.file->GetPath(), exportPath);
			}
			else
			{
				const std::shared_ptr<FileReference> fileRef = ProjectManager::GetFileReferenceByFile(*fileInfo.file);

				std::string projetPath = ProjectManager::GetProjectFolderPath();
				std::string vertexShaderCodePath = projetPath + ".shaders_build/cooked_assets/shaders_to_compile/" + std::to_string(fileRef->GetFileId()) + ".vco";
				std::string fragmentShaderCodePath = projetPath + ".shaders_build/cooked_assets/shaders_to_compile/" + std::to_string(fileRef->GetFileId()) + ".fco";

				const std::shared_ptr<File> vertexCodeFile = FileSystem::MakeFile(vertexShaderCodePath);
				const std::shared_ptr<File> fragmentCodeFile = FileSystem::MakeFile(fragmentShaderCodePath);

				const bool vOpenResult = vertexCodeFile->Open(FileMode::ReadOnly);
				if (!vOpenResult) 
				{
					Debug::PrintError("[Cooker::CookAsset] Failed to load shader code: " + vertexShaderCodePath);
					return;
				}
				const bool fOpenResult = fragmentCodeFile->Open(FileMode::ReadOnly);
				if (!fOpenResult)
				{
					Debug::PrintError("[Cooker::CookAsset] Failed to load shader code: " + fragmentShaderCodePath);
					vertexCodeFile->Close();
					return;
				}

				size_t vertexBinaryCodeSize = 0;
				unsigned char* vertexCodeBinary = vertexCodeFile->ReadAllBinary(vertexBinaryCodeSize);
				vertexCodeFile->Close();

				size_t fragmentBinaryCodeSize = 0;
				unsigned char* fragmentCodeBinary = fragmentCodeFile->ReadAllBinary(fragmentBinaryCodeSize);
				fragmentCodeFile->Close();

				const uint32_t vertexBinaryCodeSizeFixed = static_cast<uint32_t>(vertexBinaryCodeSize);
				const uint32_t fragmentBinaryCodeSizeFixed = static_cast<uint32_t>(fragmentBinaryCodeSize);

				// REMINDER: NEVER WRITE A SIZE_T TO A FILE, ALWAYS CONVERT IT TO A FIXED SIZE TYPE
				std::ofstream shaderFile = std::ofstream(exportPath, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
				shaderFile.write((char*)&vertexBinaryCodeSizeFixed, sizeof(uint32_t));
				shaderFile.write((char*)vertexCodeBinary, vertexBinaryCodeSizeFixed);
				shaderFile.write((char*)&fragmentBinaryCodeSizeFixed, sizeof(uint32_t));
				shaderFile.write((char*)fragmentCodeBinary, fragmentBinaryCodeSizeFixed);
				shaderFile.close();

				free(vertexCodeBinary);
				free(fragmentCodeBinary);
			}
		}
		else 
		{
			CopyUtils::AddCopyEntry(false, fileInfo.file->GetPath(), exportPath);
		}
	}
	else // If file can't be cooked, just copy it
	{
		CopyUtils::AddCopyEntry(false, fileInfo.file->GetPath(), exportPath);
	}

	// Copy the raw meta file, maybe we should cook it too later
	CopyUtils::AddCopyEntry(false, fileInfo.file->GetPath() + ".meta", exportPath + ".meta");
	const uint64_t metaSize = fs::file_size(fileInfo.file->GetPath() + ".meta");

	CopyUtils::ExecuteCopyEntries();

	const uint64_t cookedFileSize = fs::file_size(exportPath.c_str());

	// Do not include audio in the binary file
	size_t dataOffset = 0;
	if (fileInfo.type != FileType::File_Audio)
	{
		const std::shared_ptr<File> cookedFile = FileSystem::MakeFile(exportPath);
		size_t cookedFileSizeOut;
		cookedFile->Open(FileMode::ReadOnly);
		unsigned char* fileData = cookedFile->ReadAllBinary(cookedFileSizeOut);
		cookedFile->Close();
		dataOffset = fileDataBase.GetBitFile().AddData(fileData, cookedFileSize);
		delete[] fileData;
		FileSystem::s_fileSystem->Delete(exportPath);
	}

	// Add the meta file to the binary file
	size_t metaDataOffset = 0;
	const std::shared_ptr<File> cookedMetaFile = FileSystem::MakeFile(exportPath + ".meta");
	size_t cookedMetaFileSizeOut;
	bool cookedMetaOpen = cookedMetaFile->Open(FileMode::ReadOnly);
	if (cookedMetaOpen)
	{
		unsigned char* metaFileData = cookedMetaFile->ReadAllBinary(cookedMetaFileSizeOut);
		cookedMetaFile->Close();
		metaDataOffset = fileDataBase.GetBitFile().AddData(metaFileData, cookedMetaFileSizeOut);
		delete[] metaFileData;
	}
	else
	{
		Debug::PrintError("[Cooker::CookAsset] Failed to open meta file: " + exportPath + ".meta");
		return;
	}
	FileSystem::s_fileSystem->Delete(exportPath + ".meta");

	FileDataBaseEntry* fileDataBaseEntry = new FileDataBaseEntry();
	fileDataBaseEntry->p = partialFilePath; // Path
	fileDataBaseEntry->id = fileInfo.file->GetUniqueId(); // Unique id
	fileDataBaseEntry->po = dataOffset; // Position in the binary file in byte
	fileDataBaseEntry->s = cookedFileSize; // Size in byte
	fileDataBaseEntry->mpo = metaDataOffset; // Meta position in the binary file in byte
	fileDataBaseEntry->ms = metaSize; // Meta Size in byte
	fileDataBaseEntry->t = fileInfo.type; // Type

	fileDataBase.AddFile(fileDataBaseEntry);
}