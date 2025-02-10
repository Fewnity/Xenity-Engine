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
#include <engine/graphics/texture/texture.h>
#include <engine/graphics/shader/shader.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/debug/debug.h>
#include <mutex>

namespace fs = std::filesystem;

#define ASSETS_FOLDER "assets/"

FileDataBase Cooker::fileDataBase;
using ordered_json = nlohmann::ordered_json;

std::mutex dataBaseMutex;
std::mutex copyMutex;

void Cooker::CookAssets(const CookSettings& settings)
{
	XASSERT(settings.assetPlatform != AssetPlatform::AP_COUNT, "[Cooker::CookAssets] Asset platform is not set");
	XASSERT(!settings.exportPath.empty(), "[Cooker::CookAssets] Export path is not set");
	XASSERT(settings.platform != Platform::P_COUNT, "[Cooker::CookAssets] Platform is not set");

	// Create a new data base and binary file
	fileDataBase.Clear();
	fileDataBase.GetBitFile().Create(settings.exportPath + "data.xenb");

	const std::string projectAssetFolder = ProjectManager::GetProjectFolderPath();
	const size_t projectFolderPathLen = projectAssetFolder.size();
	const std::set<uint64_t> fileToCookIds = ProjectManager::GetAllUsedFileByTheGame();

	// Create all file references to avoid making them in the threads
	for (uint64_t id : fileToCookIds)
	{
		const FileInfo* fileInfo = ProjectManager::GetFileById(id);
		if (fileInfo)
		{
			const std::shared_ptr<FileReference> fileRef = ProjectManager::GetFileReferenceByFile(*fileInfo->fileAndId.file);
		}
	}

	// Cook all files
	std::vector<std::thread> threads;

	for (uint64_t id : fileToCookIds)
	{
		const FileInfo* fileInfo = ProjectManager::GetFileById(id);
		if (fileInfo)
		{
			std::string newPath;
			if (fileInfo->fileAndId.file->GetPath()[0] == '.')
			{
				newPath = fileInfo->fileAndId.file->GetPath().substr(2);
			}
			else
			{
				newPath = fileInfo->fileAndId.file->GetPath().substr(projectFolderPathLen, fileInfo->fileAndId.file->GetPath().size() - projectFolderPathLen);
			}

			// Create the destination folder for the file
			std::string folderToCreate = (settings.exportPath + newPath);
			folderToCreate = folderToCreate.substr(0, folderToCreate.find_last_of('/'));
			fs::create_directories(folderToCreate);

			threads.emplace_back(&Cooker::CookAsset, settings, *fileInfo, folderToCreate, newPath);
		}
	}

	// Wait for all threads to finish
	for (std::thread& thread : threads)
	{
		thread.join();
	}

	// Check the integrity of the data base
	const IntegrityState integrityState = fileDataBase.CheckIntegrity();
	if (integrityState != IntegrityState::Integrity_Ok)
	{
		Debug::PrintError("[Cooker::CookAssets] Data base integrity check failed");
	}

	// Save the data base file
	fileDataBase.SaveToFile(settings.exportPath + "db.xenb");
	fileDataBase.GetBitFile().Close();
}

//void Cooker::CookAsset(const CookSettings& settings, const FileInfo& fileInfo, const std::string& exportFolderPath, const std::string& partialFilePath)
void Cooker::CookAsset(const CookSettings settings, const FileInfo fileInfo, const std::string exportFolderPath, const std::string partialFilePath)
{
	XASSERT(!partialFilePath.empty(), "[Cooker::CookAssets] partialFilePath is not set");
	XASSERT(!exportFolderPath.empty(), "[Cooker::CookAssets] exportFolderPath is not set");

	const std::string exportPath = exportFolderPath + "/" + fileInfo.fileAndId.file->GetFileName() + fileInfo.fileAndId.file->GetFileExtension();

	if (fileInfo.type != FileType::File_Shader && settings.exportShadersOnly)
	{
		return;
	}

	// We copy the cooked file to the export folder, and then we add the copied file to the binary file
	if (fileInfo.type == FileType::File_Texture) // Cook texture
	{
		CookTexture(settings, fileInfo, exportPath);
	}
	else if (fileInfo.type == FileType::File_Mesh) // Cook mesh
	{
		CookMesh(settings, fileInfo, exportPath);
	}
	else if (fileInfo.type == FileType::File_Shader) // Cook shader
	{
		CookShader(settings, fileInfo, exportPath);
	}
	else // If file can't be cooked, just copy it
	{
		copyMutex.lock();
		CopyUtils::AddCopyEntry(false, fileInfo.fileAndId.file->GetPath(), exportPath);
		copyMutex.unlock();
	}

	copyMutex.lock();
	// Copy the raw meta file, maybe we should cook it too later
	CopyUtils::AddCopyEntry(false, fileInfo.fileAndId.file->GetPath() + ".meta", exportPath + ".meta");
	CopyUtils::ExecuteCopyEntries();
	copyMutex.unlock();

	const uint64_t metaSize = fs::file_size(exportPath + ".meta");
	uint64_t cookedFileSize = 0;

	// Do not include audio in the binary file
	size_t dataOffset = 0;
	unsigned char* fileData = nullptr;
	unsigned char* metaFileData = nullptr;
	if (fileInfo.type != FileType::File_Audio)
	{
		const std::shared_ptr<File> cookedFile = FileSystem::MakeFile(exportPath);
		cookedFile->Open(FileMode::ReadOnly);
		fileData = cookedFile->ReadAllBinary(cookedFileSize);
		cookedFile->Close();
		FileSystem::s_fileSystem->Delete(exportPath);
	}

	// Add the meta file to the binary file
	size_t metaDataOffset = 0;
	const std::shared_ptr<File> cookedMetaFile = FileSystem::MakeFile(exportPath + ".meta");
	size_t cookedMetaFileSizeOut;
	bool cookedMetaOpen = cookedMetaFile->Open(FileMode::ReadOnly);
	if (cookedMetaOpen)
	{
		metaFileData = cookedMetaFile->ReadAllBinary(cookedMetaFileSizeOut);
		cookedMetaFile->Close();
	}
	else
	{
		Debug::PrintError("[Cooker::CookAsset] Failed to open meta file: " + exportPath + ".meta");

		delete[] fileData;
		return;
	}
	FileSystem::s_fileSystem->Delete(exportPath + ".meta");

	XASSERT(metaSize != 0, "[Cooker::CookAssets] Wrong original meta file size");
	XASSERT(cookedMetaFileSizeOut != 0, "[Cooker::CookAssets] Wrong cooked meta file size");
	XASSERT(cookedFileSize != 0, "[Cooker::CookAssets] Wrong original meta file size");

	dataBaseMutex.lock();

	if (fileData)
	{
		dataOffset = fileDataBase.GetBitFile().AddData(fileData, cookedFileSize);
		delete[] fileData;
	}
	if (metaFileData)
	{
		metaDataOffset = fileDataBase.GetBitFile().AddData(metaFileData, cookedMetaFileSizeOut);
		delete[] metaFileData;
	}

	FileDataBaseEntry* fileDataBaseEntry = new FileDataBaseEntry();
	fileDataBaseEntry->p = partialFilePath; // Path
	fileDataBaseEntry->id = fileInfo.fileAndId.id; // Unique id
	fileDataBaseEntry->po = dataOffset; // Position in the binary file in byte
	fileDataBaseEntry->s = cookedFileSize; // Size in byte
	fileDataBaseEntry->mpo = metaDataOffset; // Meta position in the binary file in byte
	fileDataBaseEntry->ms = metaSize; // Meta Size in byte
	fileDataBaseEntry->t = fileInfo.type; // Type

	fileDataBase.AddFile(fileDataBaseEntry);
	dataBaseMutex.unlock();
}

void Cooker::CookMesh(const CookSettings& settings, const FileInfo& fileInfo, const std::string& exportPath)
{
	const std::shared_ptr<FileReference> fileRef = ProjectManager::GetFileReferenceByFile(*fileInfo.fileAndId.file);

	// Load mesh data
	MeshData meshData = MeshData();
	meshData.m_file = fileInfo.fileAndId.file;
	FileReference::LoadOptions loadOptions;
	loadOptions.platform = settings.platform;
	loadOptions.threaded = false;
	meshData.LoadFileReference(loadOptions);

	// REMINDER: NEVER WRITE A SIZE_T TO A FILE, ALWAYS CONVERT IT TO A FIXED SIZE TYPE
	std::ofstream meshFile = std::ofstream(exportPath, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

	// -------------- Write mesh data

	// Write submesh count
	meshFile.write((char*)&meshData.m_subMeshCount, sizeof(uint32_t));

	// Write submeshes data
	// REMINDER: NEVER WRITE A SIZE_T TO A FILE, ALWAYS CONVERT IT TO A FIXED SIZE TYPE
	for (std::unique_ptr<MeshData::SubMesh>& subMesh : meshData.m_subMeshes)
	{
		// Adapt the vertex descriptor for other platforms
		VertexDescriptor vertexDescriptorToWrite;
		bool needConvertVertexData = false;
		if (settings.assetPlatform == AssetPlatform::AP_PSP)
		{
			for (const VertexElementInfo& vertexDescriptor : subMesh->m_vertexDescriptor.m_vertexElementInfos)
			{
				// PSP does not support 4 floats color, convert it to a single 32 bits uint color
				if ((vertexDescriptor.vertexElement & VertexElements::COLOR_4_FLOATS) == VertexElements::COLOR_4_FLOATS)
				{
					vertexDescriptorToWrite.AddVertexDescriptor(VertexElements::COLOR_32_BITS_UINT);
					needConvertVertexData = true;
				}
				else
				{
					vertexDescriptorToWrite.AddVertexDescriptor(vertexDescriptor.vertexElement);
				}
			}
		}
		else
		{
			vertexDescriptorToWrite = subMesh->m_vertexDescriptor;
		}

		// Write vertex descriptor
		uint32_t vertexDescriptorSize = static_cast<uint32_t>(vertexDescriptorToWrite.m_vertexElementInfos.size());
		meshFile.write((char*)&vertexDescriptorSize, sizeof(uint32_t));
		for (const VertexElementInfo& vertexDescriptor : vertexDescriptorToWrite.m_vertexElementInfos)
		{
			meshFile.write((char*)&vertexDescriptor.vertexElement, sizeof(VertexElements));
		}

		const uint32_t newVertexMemSize = vertexDescriptorToWrite.m_vertexSize * subMesh->vertice_count;
		meshFile.write((char*)&subMesh->vertice_count, sizeof(uint32_t));
		meshFile.write((char*)&subMesh->index_count, sizeof(uint32_t));
		meshFile.write((char*)&newVertexMemSize, sizeof(uint32_t));
		meshFile.write((char*)&subMesh->indexMemSize, sizeof(uint32_t));

		const VertexDescriptor& sourceVertexDescriptor = subMesh->m_vertexDescriptor;

		// Adapt the vertex data for other platforms
		if (needConvertVertexData)
		{
			char* newMeshData = new char[newVertexMemSize];

			// Simply copy the data to the new buffer or convert it
			for (uint32_t i = 0; i < subMesh->vertice_count; i++)
			{
				if (subMesh->m_vertexDescriptor.m_uvIndex != -1)
				{
					memcpy(newMeshData + i * vertexDescriptorToWrite.m_vertexSize + vertexDescriptorToWrite.GetUvOffset(),
						(char*)subMesh->data + i * sourceVertexDescriptor.m_vertexSize + sourceVertexDescriptor.GetUvOffset(),
						VertexDescriptor::GetVertexElementSize(sourceVertexDescriptor.GetElementFromIndex(sourceVertexDescriptor.m_uvIndex)));
				}
				if (subMesh->m_vertexDescriptor.m_colorIndex != -1)
				{
					if (vertexDescriptorToWrite.GetElementFromIndex(vertexDescriptorToWrite.m_colorIndex) == VertexElements::COLOR_32_BITS_UINT)
					{
						const float* colorPtr = (float*)((char*)subMesh->data + i * sourceVertexDescriptor.m_vertexSize + sourceVertexDescriptor.GetColorOffset());

						const Color color = Color::CreateFromRGBAFloat(colorPtr[0], colorPtr[1], colorPtr[2], colorPtr[3]);

						*(unsigned int*)(newMeshData + i * vertexDescriptorToWrite.m_vertexSize + vertexDescriptorToWrite.GetColorOffset()) = color.GetUnsignedIntABGR();
					}
					else
					{
						memcpy(newMeshData + i * vertexDescriptorToWrite.m_vertexSize + vertexDescriptorToWrite.GetColorOffset(),
							(char*)subMesh->data + i * sourceVertexDescriptor.m_vertexSize + sourceVertexDescriptor.GetColorOffset(),
							VertexDescriptor::GetVertexElementSize(sourceVertexDescriptor.GetElementFromIndex(sourceVertexDescriptor.m_colorIndex)));
					}
				}
				if (subMesh->m_vertexDescriptor.m_normalIndex != -1)
				{
					memcpy(newMeshData + i * vertexDescriptorToWrite.m_vertexSize + vertexDescriptorToWrite.GetNormalOffset(),
						(char*)subMesh->data + i * sourceVertexDescriptor.m_vertexSize + sourceVertexDescriptor.GetNormalOffset(),
						VertexDescriptor::GetVertexElementSize(sourceVertexDescriptor.GetElementFromIndex(sourceVertexDescriptor.m_normalIndex)));
				}
				if (subMesh->m_vertexDescriptor.m_positionIndex != -1)
				{
					memcpy(newMeshData + i * vertexDescriptorToWrite.m_vertexSize + vertexDescriptorToWrite.GetPositionOffset(),
						(char*)subMesh->data + i * sourceVertexDescriptor.m_vertexSize + sourceVertexDescriptor.GetPositionOffset(),
						VertexDescriptor::GetVertexElementSize(sourceVertexDescriptor.GetElementFromIndex(sourceVertexDescriptor.m_positionIndex)));
				}
			}

			// Write raw data
			meshFile.write((char*)newMeshData, newVertexMemSize);

			delete[] newMeshData;
		}
		else // No need to convert the vertex data
		{
			// Write raw data
			meshFile.write((char*)subMesh->data, newVertexMemSize);
		}
		meshFile.write((char*)subMesh->indices, subMesh->indexMemSize);
	}
	meshFile.close();
}

void Cooker::CookShader(const CookSettings& settings, const FileInfo& fileInfo, const std::string& exportPath)
{
	if (settings.assetPlatform == AssetPlatform::AP_PS3)
	{
		if (settings.exportShadersOnly)
		{
			if (!std::filesystem::exists(settings.exportPath + "shaders_to_compile/"))
			{
				std::filesystem::create_directories(settings.exportPath + "shaders_to_compile/");
			}

			const std::shared_ptr<FileReference> fileRef = ProjectManager::GetFileReferenceByFile(*fileInfo.fileAndId.file);
			const std::shared_ptr<Shader> shader = std::dynamic_pointer_cast<Shader>(fileRef);

			const std::string vertexShaderCode = shader->GetShaderCode(Shader::ShaderType::Vertex_Shader, Platform::P_PS3);
			const std::string fragmentShaderCode = shader->GetShaderCode(Shader::ShaderType::Fragment_Shader, Platform::P_PS3);

			if (vertexShaderCode.empty() || fragmentShaderCode.empty())
			{
				Debug::PrintError("[Cooker::CookAsset] Failed to get shader code for shader: " + fileInfo.fileAndId.file->GetPath());
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
			copyMutex.lock();
			CopyUtils::AddCopyEntry(false, fileInfo.fileAndId.file->GetPath(), exportPath);
			copyMutex.unlock();
		}
		else
		{
			const std::shared_ptr<FileReference> fileRef = ProjectManager::GetFileReferenceByFile(*fileInfo.fileAndId.file);

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
		copyMutex.lock();
		CopyUtils::AddCopyEntry(false, fileInfo.fileAndId.file->GetPath(), exportPath);
		copyMutex.unlock();
	}
}

void Cooker::CookTexture(const CookSettings& settings, const FileInfo& fileInfo, const std::string& exportPath)
{
	const std::string texturePath = fileInfo.fileAndId.file->GetPath();

	const std::shared_ptr<FileReference> fileRef = ProjectManager::GetFileReferenceByFile(*fileInfo.fileAndId.file);
	const std::shared_ptr<Texture> texture = std::dynamic_pointer_cast<Texture>(fileRef);

	TextureResolutions textureResolution = texture->m_settings[settings.assetPlatform]->resolution;
	int width, height, channels;
	unsigned char* imageData = nullptr;

	// If the texture is already loaded, we can get the width and height
	if (texture->GetFileStatus() == FileStatus::FileStatus_Loaded)
	{
		width = texture->m_originalWidth;
		height = texture->m_originalHeight;
	}
	else // Load the texture to get the width and height and the data
	{
		imageData = stbi_load(texturePath.c_str(), &width, &height, &channels, 4);
		if (!imageData)
		{
			Debug::PrintError("[Cooker::CookAsset] Failed to load texture: " + texturePath);
			return;
		}
	}

	// Get the new width and height of the texture
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

	// Resize the texture if needed
	if (width != newWidth || height != newHeight)
	{
		// If the data is not loaded, load it
		if (!imageData)
		{
			imageData = stbi_load(texturePath.c_str(), &width, &height, &channels, 4);
			if (!imageData)
			{
				Debug::PrintError("[Cooker::CookAsset] Failed to load texture: " + texturePath);
				return;
			}
		}
		unsigned char* resizedImageData = (unsigned char*)malloc(newWidth * newHeight * 4);
		stbir_resize_uint8(imageData, width, height, 0, resizedImageData, newWidth, newHeight, 0, 4);

		stbi_write_png(exportPath.c_str(), newWidth, newHeight, 4, resizedImageData, 0);
		free(resizedImageData);
	}
	else // Just copy the texture file
	{
		std::filesystem::copy(texturePath, exportPath, std::filesystem::copy_options::overwrite_existing);
	}

	if (imageData)
	{
		free(imageData);
	}
}
