// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "project_manager.h"

#include <filesystem>
#include <json.hpp>

#if defined(EDITOR)
#include <editor/editor.h>
#include <editor/file_handler.h>
#include <editor/compiler.h>
#include <editor/file_reference_finder.h>
#endif

#include "code_file.h"

#include <engine/engine.h>
#include <engine/game_interface.h>

#include <engine/reflection/reflection_utils.h>
#include <engine/dynamic_lib/dynamic_lib.h>

#include <engine/asset_management/asset_manager.h>
#include <engine/scene_management/scene_manager.h>
#include <engine/scene_management/scene.h>

#include <engine/file_system/file_system.h>
#include <engine/file_system/file.h>
#include <engine/file_system/directory.h>

#include <engine/graphics/graphics.h>
#include <engine/graphics/skybox.h>
#include <engine/graphics/shader.h>
#include <engine/graphics/material.h>
#include <engine/graphics/ui/font.h>
#include <engine/graphics/texture.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/ui/window.h>

#include <engine/audio/audio_clip.h>

#include <engine/debug/debug.h>

#if !defined(EDITOR) && !defined(_WIN32) && !defined(_WIN64)
#include "game_code/source/game.h"
#endif
#include <engine/engine_settings.h>
#include <engine/tools/string_tag_finder.h>
#include <engine/graphics/icon.h>
#include <engine/assertions/assertions.h>
#include <engine/debug/stack_debug_object.h>

using json = nlohmann::ordered_json;

std::unordered_map<uint64_t, FileInfo> ProjectManager::projectFilesIds;
std::shared_ptr<ProjectDirectory> ProjectManager::projectDirectory = nullptr;
ProjectSettings ProjectManager::projectSettings;
std::string ProjectManager::projectFolderPath = "";
std::string ProjectManager::assetFolderPath = "";
std::string ProjectManager::engineAssetsFolderPath = "";
std::string ProjectManager::publicEngineAssetsFolderPath = "";
bool ProjectManager::projectLoaded = false;
std::shared_ptr<Directory> ProjectManager::projectDirectoryBase = nullptr;
std::shared_ptr<Directory> ProjectManager::publicEngineAssetsDirectoryBase = nullptr;
std::shared_ptr<Directory> ProjectManager::additionalAssetDirectoryBase = nullptr;
Event<> ProjectManager::projectLoadedEvent;
Event<> ProjectManager::projectUnloadedEvent;
FileDataBase ProjectManager::fileDataBase;

std::shared_ptr<ProjectDirectory> ProjectManager::FindProjectDirectory(const ProjectDirectory& directoryToCheck, const std::string& directoryPath)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	const size_t dirCount = directoryToCheck.subdirectories.size();
	for (size_t i = 0; i < dirCount; i++)
	{
		std::shared_ptr<ProjectDirectory> subDir = directoryToCheck.subdirectories[i];
		// Check if the sub directory is the directory to find
		if (subDir->path == directoryPath)
		{
			return subDir;
		}
		else
		{
			// Start recursive to search in the sub directory
			std::shared_ptr<ProjectDirectory> foundSubDir = FindProjectDirectory(*subDir, directoryPath);
			if (foundSubDir)
				return foundSubDir;
		}
	}
	return nullptr;
}

uint64_t ProjectManager::ReadFileId(const File& file)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	uint64_t id = -1;
	std::string metaFilePath = file.GetPath() + META_EXTENSION;

#if defined(_EE)
	metaFilePath = metaFilePath.substr(5);
#endif
	std::shared_ptr<File> metaFile = FileSystem::MakeFile(metaFilePath);

	if (!metaFile->CheckIfExist()) // If there is not meta for this file
	{
	}
	else // Or read meta file
	{
		if (metaFile->Open(FileMode::ReadOnly))
		{
			const std::string jsonString = metaFile->ReadAll();
			metaFile->Close();
			if (!jsonString.empty())
			{
				json data;
				try
				{
					data = json::parse(jsonString);
					id = data["id"];
				}
				catch (const std::exception&)
				{
					Debug::PrintError("[ProjectManager::FindAllProjectFiles] Meta file corrupted! File:" + metaFile->GetPath(), true);
				}
			}
		}
	}

	return id;
}

void ProjectManager::AddFilesToProjectFiles(std::vector<ProjectEngineFile>& projectFilesDestination, Directory& directorySource, bool isEngineAssets)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	std::vector<std::shared_ptr<File>> projectAssetFiles = directorySource.GetAllFiles(true);
	const size_t projectAssetFilesCount = projectAssetFiles.size();
	for (int i = 0; i < projectAssetFilesCount; i++)
	{
		ProjectEngineFile projectEngineFile;
		projectEngineFile.file = projectAssetFiles[i];
		projectEngineFile.isEngineAsset = isEngineAssets;
		projectFilesDestination.push_back(projectEngineFile);
	}
	projectAssetFiles.clear();
}

void ProjectManager::FindAllProjectFiles()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	// Keep in memory the old opened directory path to re-open it later
#if defined(EDITOR)
	std::unordered_map<uint64_t, FileChange> oldProjectFilesIds;
	std::string oldPath = "";
	if (Editor::GetCurrentProjectDirectory())
		oldPath = Editor::GetCurrentProjectDirectory()->path;

	Editor::SetCurrentProjectDirectory(nullptr);

	// Keep in memory all old files path to check later if some files have been moved
	for (const auto& kv : projectFilesIds)
	{
		FileChange fileChange = FileChange();
		fileChange.path = kv.second.path;
		oldProjectFilesIds[kv.first] = fileChange;
	}
#endif
	projectDirectory = std::make_shared<ProjectDirectory>(assetFolderPath, 0);

	projectFilesIds.clear();

	std::vector<CompatibleFile> compatibleFiles;
	// Get all compatible files of the project
	{
#if defined(EDITOR)
		std::vector<ProjectEngineFile> projectFiles;
		AddFilesToProjectFiles(projectFiles, *publicEngineAssetsDirectoryBase, true); // This directory first
		AddFilesToProjectFiles(projectFiles, *projectDirectoryBase, false);
		AddFilesToProjectFiles(projectFiles, *additionalAssetDirectoryBase, false);

		// Get all files supported by the engine
		const size_t fileCount = projectFiles.size();
		for (size_t i = 0; i < fileCount; i++)
		{
			const std::shared_ptr<File> file = projectFiles[i].file;
			const FileType fileType = GetFileType(file->GetFileExtension());

			// If the file is supported, add it to the list
			if (fileType != FileType::File_Other)
			{
				CompatibleFile compatibleFile;
				compatibleFile.file.file = file;
				compatibleFile.file.isEngineAsset = projectFiles[i].isEngineAsset;
				compatibleFile.type = fileType;
				compatibleFiles.push_back(compatibleFile);
			}
		}
		projectFiles.clear();
#else
		// All files in fileDataBase.fileList are compatible
		for (const auto& f : fileDataBase.fileList)
		{
			CompatibleFile compatibleFile;
			compatibleFile.file.file = FileSystem::MakeFile(f->p);
			compatibleFile.file.file->SetUniqueId(f->id);
			compatibleFile.file.isEngineAsset = false; // Not used when not in editor
			compatibleFile.type = f->t;
			compatibleFile.file.filePos = f->po;
			compatibleFile.file.fileSize = f->s;
			compatibleFile.file.metaFilePos = f->mpo;
			compatibleFile.file.metaFileSize = f->ms;

			compatibleFiles.push_back(compatibleFile);
		}
#endif
	}

	{
		// Read meta files and list all files that do not have a meta file for later use
#if defined(EDITOR)
		std::unordered_map<uint64_t, bool> usedIds;
		std::vector<std::shared_ptr<File>> fileWithoutMeta;
		int fileWithoutMetaCount = 0;
		uint64_t biggestId = UniqueId::reservedFileId;
		for (const auto& compatibleFile : compatibleFiles)
		{
			const std::shared_ptr<File>& file = compatibleFile.file.file;
			uint64_t fileId = ReadFileId(*file);
			if (fileId == -1)
			{
				fileWithoutMeta.push_back(file);
				fileWithoutMetaCount++;
			}
			else
			{
				if (!compatibleFile.file.isEngineAsset && (usedIds[fileId] == true || fileId <= UniqueId::reservedFileId))
					//if (usedIds[id] == true)
				{
					Debug::PrintError("[ProjectManager::FindAllProjectFiles] Id already used by another file! Id: " + std::to_string(fileId) + ", File:" + file->GetPath() + ".meta", true);
					fileWithoutMeta.push_back(file);
					fileWithoutMetaCount++;
					continue;
				}

				usedIds[fileId] = true;

				if (fileId > biggestId)
					biggestId = fileId;
				file->SetUniqueId(fileId);
			}
		}
		usedIds.clear();

		// Set new files ids
		UniqueId::lastFileUniqueId = biggestId;
		for (int i = 0; i < fileWithoutMetaCount; i++)
		{
			const uint64_t id = UniqueId::GenerateUniqueId(true);
			fileWithoutMeta[i]->SetUniqueId(id);
		}
#endif
	}

	// Fill projectFilesIds
	for (const auto& kv : compatibleFiles)
	{
		FileInfo fileAndPath = FileInfo();
		fileAndPath.file = kv.file.file;
		fileAndPath.path = kv.file.file->GetPath();
		fileAndPath.type = kv.type;
		fileAndPath.filePos = kv.file.filePos;
		fileAndPath.fileSize = kv.file.fileSize;
		fileAndPath.metaFilePos = kv.file.metaFilePos;
		fileAndPath.metaFileSize = kv.file.metaFileSize;
		projectFilesIds[kv.file.file->GetUniqueId()] = fileAndPath;
	}
	compatibleFiles.clear();

	// Create files references
	for (auto& kv : projectFilesIds)
	{
		std::shared_ptr<FileReference> fileRef;
#if defined(EDITOR)
		fileRef = CreateFileReference(kv.second.path, kv.first);
#else
		fileRef = CreateFileReference(kv.second, kv.first);
#endif
		//fileRef->filePosition = kv.second.filePos;
		//fileRef->fileSize = kv.second.fileSize;
		//fileRef->metaPosition = kv.second.metaFilePos;
		//fileRef->metaSize = kv.second.metaFileSize;

		kv.second.type = fileRef->m_fileType;
	}

#if defined(EDITOR)
	// Check if a file has changed or has been deleted
	for (const auto& kv : projectFilesIds)
	{
		const bool contains = oldProjectFilesIds.contains(kv.first);
		if (contains)
		{
			FileChange& fileChange = oldProjectFilesIds[kv.first];
			fileChange.hasBeenDeleted = false;
			if (fileChange.path != kv.second.path)
			{
				fileChange.hasChanged = true;
			}
		}
	}

	// Update file or delete files references
	for (const auto& kv : oldProjectFilesIds)
	{
		if (kv.second.hasChanged)
		{
			GetFileReferenceById(kv.first)->m_file = projectFilesIds[kv.first].file;
		}
		else if (kv.second.hasBeenDeleted)
		{
			AssetManager::ForceDeleteFileReference(GetFileReferenceById(kv.first));
		}
	}
	oldProjectFilesIds.clear();

	// Get all project directories and open one
	CreateProjectDirectories(*projectDirectoryBase, *projectDirectory);
	std::shared_ptr<ProjectDirectory> lastOpenedDir = FindProjectDirectory(*projectDirectory, oldPath);
	if (lastOpenedDir)
		Editor::SetCurrentProjectDirectory(lastOpenedDir);
	else
		Editor::SetCurrentProjectDirectory(projectDirectory);
#endif
}

void ProjectManager::CreateVisualStudioSettings()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	try
	{
		// Get engine includes folder
		const std::filesystem::path exePath = std::filesystem::canonical("./");
		const std::string includesPath = exePath.generic_string() + "/includes/";

		// Read the empty vscode settings file
		std::shared_ptr<File> emptyVSCodeParamFile = FileSystem::MakeFile("./vscodeSample/c_cpp_properties.json");

		const bool isOpen = emptyVSCodeParamFile->Open(FileMode::ReadOnly);
		if (isOpen)
		{
			std::string vsCodeText = emptyVSCodeParamFile->ReadAll();
			emptyVSCodeParamFile->Close();

			const size_t vsCodeTextSize = vsCodeText.size();

			// Replace tag by the include folder path
			int beg, end;
			for (size_t i = 0; i < vsCodeTextSize; i++)
			{
				if (StringTagFinder::FindTag(vsCodeText, i, vsCodeTextSize, "{ENGINE_SOURCE_PATH}", beg, end))
				{
					vsCodeText.replace(beg, end - beg - 1, includesPath);
					break;
				}
			}

			// Create vscode folder
			FileSystem::s_fileSystem->CreateFolder(assetFolderPath + ".vscode/");

			const std::string filePath = assetFolderPath + ".vscode/c_cpp_properties.json";
			FileSystem::s_fileSystem->Delete(filePath);

			// Create the vscode settings file
			std::shared_ptr<File> vsCodeParamFile = FileSystem::MakeFile(filePath);
			const bool isNewFileOpen = vsCodeParamFile->Open(FileMode::WriteCreateFile);
			if (isNewFileOpen)
			{
				vsCodeParamFile->Write(vsCodeText);
				vsCodeParamFile->Close();
			}
			else
			{
				Debug::PrintError("[ProjectManager::CreateVisualStudioSettings] Failed to create Visual Studio Settings file", true);
			}
		}
		else
		{
			Debug::PrintError("[ProjectManager::CreateVisualStudioSettings] Failed to read Visual Studio Settings sample file", true);
		}
	}
	catch (const std::exception&)
	{
		Debug::PrintError("[ProjectManager::CreateVisualStudioSettings] Fail to create Visual Studio Settings file", true);
	}
}

void ProjectManager::CreateProjectDirectories(Directory& projectDirectoryBase, ProjectDirectory& realProjectDirectory)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	const size_t dirCount = projectDirectoryBase.subdirectories.size();
	for (size_t i = 0; i < dirCount; i++)
	{
		std::shared_ptr<ProjectDirectory> newDir = std::make_shared<ProjectDirectory>(projectDirectoryBase.subdirectories[i]->GetPath(), projectDirectoryBase.subdirectories[i]->GetUniqueId());
		realProjectDirectory.subdirectories.push_back(newDir);
		CreateProjectDirectories(*projectDirectoryBase.subdirectories[i], *newDir);
	}
}

void ProjectManager::RefreshProjectDirectory()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	FindAllProjectFiles();
}

void ProjectManager::FillProjectDirectory(ProjectDirectory& _projectDirectory)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	std::vector<std::shared_ptr<FileReference>>& projFileVector = _projectDirectory.files;
	projFileVector.clear();

	for (const auto& kv : ProjectManager::projectFilesIds)
	{
		// Check if this file is in this folder
		if (_projectDirectory.path == kv.second.file->GetFolderPath())
		{
			projFileVector.push_back(ProjectManager::GetFileReferenceById(kv.first));
		}
	}

	// Sort files by name (only in editor)
#if defined(EDITOR)
	std::sort(projFileVector.begin(), projFileVector.end(),
		[](const std::shared_ptr<FileReference>& a, const std::shared_ptr<FileReference>& b)
		{
			return (a->m_file->GetFileName() + a->m_file->GetFileExtension()) < (b->m_file->GetFileName() + b->m_file->GetFileExtension());
		});
#endif
}

void ProjectManager::Init()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	engineAssetsFolderPath = "./engine_assets/";
	publicEngineAssetsFolderPath = "./public_engine_assets/";

	publicEngineAssetsDirectoryBase = std::make_shared<Directory>(publicEngineAssetsFolderPath);
}

bool ProjectManager::CreateProject(const std::string& name, const std::string& folderPath)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	XASSERT(!name.empty(), "[ProjectManager::CreateProject] name is empty");
	XASSERT(!folderPath.empty(), "[ProjectManager::CreateProject] folderPath is empty");

	FileSystem::s_fileSystem->CreateFolder(folderPath + name + "/");
	FileSystem::s_fileSystem->CreateFolder(folderPath + name + "/temp/");
	FileSystem::s_fileSystem->CreateFolder(folderPath + name + "/additional_assets/");
	FileSystem::s_fileSystem->CreateFolder(folderPath + name + "/assets/");
	FileSystem::s_fileSystem->CreateFolder(folderPath + name + "/assets/Scripts/");
	FileSystem::s_fileSystem->CreateFolder(folderPath + name + "/assets/Scenes/");

	// Create default scene
	std::shared_ptr<Scene> sceneRef = std::dynamic_pointer_cast<Scene>(CreateFileReference(folderPath + name + "/assets/Scenes/MainScene.xen", UniqueId::GenerateUniqueId(true)));
	if (sceneRef->m_file->Open(FileMode::WriteCreateFile))
	{
		const std::string data = AssetManager::GetDefaultFileData(FileType::File_Scene);
		sceneRef->m_file->Write(data);
		sceneRef->m_file->Close();
	}

	// TODO improve this (use copy entry system like in the compiler class)
	try
	{
		std::filesystem::copy_file("engine_assets/empty_default/game.cpp", folderPath + name + "/assets/game.cpp", std::filesystem::copy_options::overwrite_existing);
		std::filesystem::copy_file("engine_assets/empty_default/game.h", folderPath + name + "/assets/game.h", std::filesystem::copy_options::overwrite_existing);
	}
	catch (const std::exception&)
	{
		Debug::PrintError("[ProjectManager::CreateProject] Error when copying default assets into the project.", true);
	}

	projectSettings.projectName = name;
	projectSettings.gameName = name;
	projectSettings.startScene = sceneRef;
	projectFolderPath = folderPath + name + "/";
	SaveProjectSettings();

	return LoadProject(projectFolderPath);
}

FileType ProjectManager::GetFileType(const std::string& _extension)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	FileType fileType = FileType::File_Other;
	std::string extension = _extension;

	// Replace uppercase letters by lowercase letters
	const size_t extLen = extension.size();
	for (size_t i = 1; i < extLen; i++)
	{
		extension[i] = tolower(extension[i]);
	}

	if (extension == ".png" || extension == ".jpg" || extension == ".bmp") // If the file is an image
	{
		fileType = FileType::File_Texture;
	}
	else if (extension == ".wav" || extension == ".mp3") // If the file is a sound/music
	{
		fileType = FileType::File_Audio;
	}
	else if (extension == ".obj") // If the file is a 3D object
	{
		fileType = FileType::File_Mesh;
	}
	else if (extension == ".xen") // If the file is a scene
	{
		fileType = FileType::File_Scene;
	}
	else if (extension == ".cpp") // If the file is a code file/header
	{
		fileType = FileType::File_Code;
	}
	else if (extension == ".h") // If the file is a code file/header
	{
		fileType = FileType::File_Header;
	}
	else if (extension == ".sky") // If the file is a skybox
	{
		fileType = FileType::File_Skybox;
	}
	else if (extension == ".ttf") // If the file is a font
	{
		fileType = FileType::File_Font;
	}
	else if (extension == ".mat") // If the file is a material
	{
		fileType = FileType::File_Material;
	}
	else if (extension == ".shader") // If the file is a shader
	{
		fileType = FileType::File_Shader;
	}
	else if (extension == ".ico") // If the file is an icon
	{
		fileType = FileType::File_Icon;
	}

	return fileType;
}

#if defined(EDITOR)

void ProjectManager::OnProjectCompiled(CompilerParams params, bool result)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	if (params.buildType != BuildType::EditorHotReloading)
		return;

	if (result)
	{
		projectSettings.compiledLibEngineVersion = ENGINE_DLL_VERSION;
	}
	else
	{
		// Set the version to 0 to avoid loading old DLL
		projectSettings.compiledLibEngineVersion = "0";
	}

	bool isDebugMode = false;
#if defined(DEBUG)
	isDebugMode = true;
#endif
	projectSettings.isLibCompiledForDebug = isDebugMode;

	bool is64Bits = false;
#if defined(_WIN64)
	is64Bits = true;
#endif
	projectSettings.isLibCompiledFor64Bits = is64Bits;

	SaveProjectSettings();
}
#endif

bool ProjectManager::LoadProject(const std::string& projectPathToLoad)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

#if defined(EDITOR)
	Compiler::GetOnCompilationEndedEvent().Bind(&ProjectManager::OnProjectCompiled);
#else
	fileDataBase.LoadFromFile(projectPathToLoad + "db.bin");
	fileDataBase.bitFile.Open("data.xenb");
#endif

	Debug::Print("Loading project: " + projectPathToLoad, true);
	projectLoaded = false;

	projectFolderPath = projectPathToLoad;
	assetFolderPath = projectPathToLoad + "assets/";

	projectDirectoryBase = std::make_shared<Directory>(assetFolderPath);

#if defined(EDITOR)
	FileSystem::s_fileSystem->CreateFolder(projectFolderPath + "/temp/");
	FileSystem::s_fileSystem->CreateFolder(projectFolderPath + "/additional_assets/");
#endif

	additionalAssetDirectoryBase = std::make_shared<Directory>(projectFolderPath + "/additional_assets/");

	FindAllProjectFiles();

	LoadProjectSettings();
	projectSettings.engineVersion = ENGINE_VERSION;
#if defined(EDITOR)
	SaveProjectSettings();
#endif

	// Load dynamic library and create game
#if !defined(__LINUX__)
#if defined(_WIN32) || defined(_WIN64)
	bool isDebugMode = false;
	bool is64Bits = false;
#if defined(_WIN64)
	is64Bits = true;
#endif
#if defined(DEBUG)
	isDebugMode = true;
#endif
	const bool isSameVersion = projectSettings.compiledLibEngineVersion == ENGINE_DLL_VERSION;
	const bool isSameDebugMode = projectSettings.isLibCompiledForDebug == isDebugMode;
	const bool isSame64Bits = projectSettings.isLibCompiledFor64Bits == is64Bits;

	if (isSameVersion && isSameDebugMode && isSame64Bits)
	{
#if defined(EDITOR)
		DynamicLibrary::LoadGameLibrary(ProjectManager::GetProjectFolderPath() + "temp/game_editor");
#else
		DynamicLibrary::LoadGameLibrary("game");
#endif // defined(EDITOR)
		Engine::s_game = DynamicLibrary::CreateGame();
	}
	else
	{
		// Maybe automaticaly recompile the project
		Debug::PrintWarning("The project was compiled with another version of the engine, please recompile the game.");
	}
#else
	Engine::s_game = std::make_unique<Game>();
#endif //  defined(_WIN32) || defined(_WIN64)
#endif
	// Fill class registery
	if (Engine::s_game)
		Engine::s_game->Start();

#if defined(EDITOR)
	CreateVisualStudioSettings();

	// Check files to avoid triggerring a compilation
	FileHandler::HasCodeChanged(GetAssetFolderPath());
	FileHandler::HasFileChangedOrAdded(GetAssetFolderPath());

	if (EngineSettings::values.compileWhenOpeningProject)
	{
		Compiler::HotReloadGame();
	}
#endif

	projectLoaded = true;

	// Load start scene
	if (ProjectManager::GetStartScene())
	{
		SceneManager::LoadScene(ProjectManager::GetStartScene());
	}

	projectLoadedEvent.Trigger();

	Debug::Print("Project loaded", true);

	return projectLoaded;
}

void ProjectManager::UnloadProject()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

#if defined(EDITOR)
	Editor::SetCurrentProjectDirectory(nullptr);
	Editor::SetSelectedGameObject(nullptr);
	Editor::SetSelectedFileReference(nullptr);

	SceneManager::SetSceneModified(false);
	SceneManager::SetOpenedScene(nullptr);
	SceneManager::ClearScene();
	SceneManager::CreateEmptyScene();
	Graphics::SetDefaultValues();

	ClassRegistry::Reset();
	ClassRegistry::RegisterEngineComponents();

	projectSettings.startScene.reset();
	projectDirectoryBase.reset();
	additionalAssetDirectoryBase.reset();
	projectDirectoryBase.reset();
	projectDirectory.reset();
	projectFilesIds.clear();
	projectLoaded = false;
	projectSettings.projectName.clear();
	projectSettings.gameName.clear();
	projectFolderPath.clear();

	assetFolderPath.clear();

	Engine::s_game.reset();
	DynamicLibrary::UnloadGameLibrary();
	AssetManager::RemoveAllFileReferences();
	Window::UpdateWindowTitle();

	projectUnloadedEvent.Trigger();
#endif
}

std::vector<uint64_t> ProjectManager::GetAllUsedFileByTheGame()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	std::vector<uint64_t> ids;
#if defined(EDITOR)
	int idCount = 0;
	const std::vector<FileInfo> sceneFiles = GetFilesByType(FileType::File_Scene);
	const size_t sceneCount = sceneFiles.size();

	for (auto& fileIds : projectFilesIds)
	{
		if (fileIds.first <= UniqueId::reservedFileId)
			ids.push_back(fileIds.first);
	}

	//projectFilesIds
	for (size_t i = 0; i < sceneCount; i++)
	{
		ids.push_back(sceneFiles[i].file->GetUniqueId());
		std::shared_ptr<File> jsonFile = sceneFiles[i].file;
		const bool isOpen = jsonFile->Open(FileMode::ReadOnly);
		if (isOpen)
		{
			const std::string jsonString = jsonFile->ReadAll();
			jsonFile->Close();

			try
			{
				json data;
				if (!jsonString.empty())
					data = json::parse(jsonString);

				for (const auto& idKv : data["UsedFiles"]["Values"].items())
				{
					bool idAlreadyInList = false;
					for (int idIndex = 0; idIndex < idCount; idIndex++)
					{
						if (ids[idIndex] == idKv.value())
						{
							idAlreadyInList = true;
							break;
						}
					}
					if (!idAlreadyInList)
					{
						ids.push_back(idKv.value());
						std::shared_ptr<FileReference> fileRef = GetFileReferenceById(idKv.value());
						if (fileRef)
						{
							FileReferenceFinder::GetUsedFilesInReflectiveData(ids, fileRef->GetReflectiveData());
						}
						else
						{
							Debug::PrintError("[ProjectManager::GetAllUsedFileByTheGame] File reference not found, please try re-save the scene: " + sceneFiles[i].file->GetFileName(), true);
						}
						idCount++;
					}
				}
			}
			catch (const std::exception&)
			{
				Debug::PrintError("[ProjectManager::GetAllUsedFileByTheGame] Scene file error", true);
				continue;
			}
		}
	}
#endif
	return ids;
}

std::vector<FileInfo> ProjectManager::GetFilesByType(const FileType type)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	std::vector<FileInfo> fileList;
	for (const auto& fileinfo : projectFilesIds)
	{
		if (fileinfo.second.type == type)
		{
			fileList.push_back(fileinfo.second);
		}
	}

	return fileList;
}

FileInfo* ProjectManager::GetFileById(const uint64_t id)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	if (projectFilesIds.contains(id))
	{
		return &projectFilesIds[id];
	}

	return nullptr;
}

std::shared_ptr<FileReference> ProjectManager::GetFileReferenceById(const uint64_t id)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	if (id == -1)
	{
		return nullptr;
	}

	std::shared_ptr<FileReference> fileRef = nullptr;

	// Find if the File Reference is already instanciated
	const int fileRefCount = AssetManager::GetFileReferenceCount();
	for (int i = 0; i < fileRefCount; i++)
	{
		std::shared_ptr<FileReference> tempFileRef = AssetManager::GetFileReference(i);
		if (tempFileRef->m_fileId == id)
		{
			fileRef = std::move(tempFileRef);
			break;
		}
	}

	// If the file is not instanciated, create the File Reference
	if (fileRef == nullptr)
	{
		if (projectFilesIds.contains(id))
		{
#if defined(EDITOR)
			fileRef = CreateFileReference(projectFilesIds[id].path, id);
#else
			fileRef = CreateFileReference(projectFilesIds[id], id);
#endif
			if (fileRef)
			{
				if (fileRef->m_fileType == FileType::File_Skybox)
					fileRef->LoadFileReference();
			}
		}
	}

	return fileRef;
}

std::shared_ptr<FileReference> ProjectManager::GetFileReferenceByFile(File& file)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	const uint64_t fileId = ProjectManager::ReadFileId(file);
	return GetFileReferenceById(fileId);
}

std::shared_ptr<FileReference> ProjectManager::GetFileReferenceByFilePath(const std::string& filePath)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

#if defined(EDITOR)
	std::shared_ptr<File> file = FileSystem::MakeFile(filePath);
	const uint64_t fileId = ProjectManager::ReadFileId(*file);
#else
	uint64_t fileId = -1;
	for(const auto& kv : projectFilesIds)
	{
		if(kv.second.path == filePath)
		{
			fileId = kv.first;
			break;
		}
	}
#endif
	return GetFileReferenceById(fileId);
}

ProjectSettings ProjectManager::GetProjectSettings(const std::string& projectPath)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	ProjectSettings settings;
	std::shared_ptr<File> projectFile = FileSystem::MakeFile(projectPath + PROJECT_SETTINGS_FILE_NAME);
	std::string jsonString = "";

	// Read file
	if (projectFile->Open(FileMode::ReadOnly))
	{
		jsonString = projectFile->ReadAll();
		projectFile->Close();

		if (!jsonString.empty())
		{
			// Parse Json
			json projectData;
			try
			{
				projectData = json::parse(jsonString);
			}
			catch (const std::exception&)
			{
				Debug::PrintError("[ProjectManager::LoadProjectSettings] Corrupted project settings", true);
				return settings;
			}

			// Change settings
			ReflectionUtils::JsonToReflectiveData(projectData, settings.GetReflectiveData());
		}
	}
	else
	{
		XASSERT(false, "[ProjectManager::LoadProjectSettings] Fail to open the project settings file");
		Debug::PrintError("[ProjectManager::LoadProjectSettings] Fail to open the project settings file", true);
	}

	return settings;
}

void ProjectManager::LoadProjectSettings()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	projectSettings = GetProjectSettings(projectFolderPath);
}

void ProjectManager::SaveProjectSettings(const std::string folderPath)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	const std::string path = folderPath + PROJECT_SETTINGS_FILE_NAME;
	FileSystem::s_fileSystem->Delete(path);
	json projectData;

	projectData["Values"] = ReflectionUtils::ReflectiveDataToJson(projectSettings.GetReflectiveData());

	std::shared_ptr<File> projectFile = FileSystem::MakeFile(path);
	if (projectFile->Open(FileMode::WriteCreateFile))
	{
		projectFile->Write(projectData.dump(0));
		projectFile->Close();
	}
	else
	{
		Debug::PrintError("[ProjectManager::SaveProjectSettings] Cannot save project settings: " + path, true);
	}
}

void ProjectManager::SaveProjectSettings()
{
	SaveProjectSettings(projectFolderPath);
}

void ProjectManager::SaveMetaFile(FileReference& fileReference)
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	std::shared_ptr<File> file = fileReference.m_file;

	std::shared_ptr<File> metaFile = FileSystem::MakeFile(file->GetPath() + META_EXTENSION);
	bool exists = metaFile->CheckIfExist();
	if (!file || (!fileReference.m_isMetaDirty && exists))
		return;

	FileSystem::s_fileSystem->Delete(file->GetPath() + META_EXTENSION);
	json metaData;
	metaData["id"] = fileReference.m_fileId;
	metaData["MetaVersion"] = metaVersion;

	metaData["Standalone"]["Values"] = ReflectionUtils::ReflectiveDataToJson(fileReference.GetMetaReflectiveData(AssetPlatform::AP_Standalone));
	metaData["PSP"]["Values"] = ReflectionUtils::ReflectiveDataToJson(fileReference.GetMetaReflectiveData(AssetPlatform::AP_PSP));
	metaData["PSVITA"]["Values"] = ReflectionUtils::ReflectiveDataToJson(fileReference.GetMetaReflectiveData(AssetPlatform::AP_PsVita));

	if (metaFile->Open(FileMode::WriteCreateFile))
	{
		metaFile->Write(metaData.dump(0));
		metaFile->Close();
		fileReference.m_isMetaDirty = false;
#if defined(EDITOR)
		FileHandler::SetLastModifiedFile(file->GetPath() + META_EXTENSION);
		if (!exists)
			FileHandler::AddOneFile();
#endif
	}
	else
	{
		Debug::PrintError("[ProjectManager::SaveMetaFile] Cannot save meta file: " + file->GetPath(), true);
	}
}

std::vector<ProjectListItem> ProjectManager::GetProjectsList()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	std::vector<ProjectListItem> projects;
	std::shared_ptr<File> file = FileSystem::MakeFile(PROJECTS_LIST_FILE);
	const bool isOpen = file->Open(FileMode::ReadOnly);
	if (isOpen)
	{
		const std::string projectFileString = file->ReadAll();
		if (!projectFileString.empty())
		{
			json j;
			try
			{
				j = json::parse(projectFileString);
			}
			catch (const std::exception&)
			{
				Debug::PrintError("[ProjectManager::GetProjectsList] Fail to load projects list: " + file->GetPath(), true);
			}

			const size_t projectCount = j.size();
			for (size_t i = 0; i < projectCount; i++)
			{
				// Get project informations (name and path)
				ProjectListItem projectItem;
				projectItem.path = j[i]["path"];
				const ProjectSettings settings = GetProjectSettings(projectItem.path);
				if (settings.projectName.empty())
				{
					projectItem.name = j[i]["name"];
				}
				else
				{
					projectItem.name = settings.projectName;
				}
				projects.push_back(projectItem);
			}
		}
		file->Close();
	}
	return projects;
}

void ProjectManager::SaveProjectsList(const std::vector<ProjectListItem>& projects)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	const size_t projectSize = projects.size();
	json j;
	for (size_t i = 0; i < projectSize; i++)
	{
		j[i]["name"] = projects[i].name;
		j[i]["path"] = projects[i].path;
	}
	FileSystem::s_fileSystem->Delete(PROJECTS_LIST_FILE);
	std::shared_ptr<File> file = FileSystem::MakeFile(PROJECTS_LIST_FILE);
	if (file->Open(FileMode::WriteCreateFile))
	{
		file->Write(j.dump(3));
		file->Close();
	}
	else
	{
		Debug::PrintError(std::string("[ProjectManager::SaveProjectsList] Cannot save projects list: ") + PROJECTS_LIST_FILE, true);
	}
}

std::shared_ptr<FileReference> ProjectManager::CreateFileReference(const std::string& path, const uint64_t id)
{
	std::shared_ptr<FileReference> fileRef = nullptr;
	std::shared_ptr<File> file = FileSystem::MakeFile(path);

	const FileType type = GetFileType(file->GetFileExtension());
	switch (type)
	{
	case FileType::File_Audio:
		fileRef = AudioClip::MakeAudioClip();
		break;
	case FileType::File_Mesh:
		fileRef = MeshData::MakeMeshData();
		break;
	case FileType::File_Texture:
		fileRef = Texture::MakeTexture();
		break;
	case FileType::File_Scene:
		fileRef = Scene::MakeScene();
		break;
	case FileType::File_Header:
		fileRef = CodeFile::MakeCode(true);
		break;
	case FileType::File_Code:
		fileRef = CodeFile::MakeCode(false);
		break;
	case FileType::File_Skybox:
		fileRef = SkyBox::MakeSkyBox();
		break;
	case FileType::File_Font:
		fileRef = Font::MakeFont();
		break;
	case FileType::File_Material:
		fileRef = Material::MakeMaterial();
		break;
	case FileType::File_Shader:
		fileRef = Shader::MakeShader();
		break;
	case FileType::File_Icon:
		fileRef = Icon::MakeIcon();
		break;

	case FileType::File_Other:
		// Do nothing
		break;
	}

	if (fileRef)
	{
#if defined(EDITOR)
		fileRef->m_fileId = id;
#endif
		fileRef->m_file = file;
		fileRef->m_fileType = type;
		LoadMetaFile(*fileRef);
#if defined(EDITOR)
		SaveMetaFile(*fileRef);
#endif
	}
	return fileRef;
}

std::shared_ptr<FileReference> ProjectManager::CreateFileReference(const FileInfo& fileInfo, const uint64_t id)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	std::shared_ptr<FileReference> fileRef = nullptr;
	//std::shared_ptr<File> file = FileSystem::MakeFile(fileInfo.file);

	switch (fileInfo.type)
	{
	case FileType::File_Audio:
		fileRef = AudioClip::MakeAudioClip();
		break;
	case FileType::File_Mesh:
		fileRef = MeshData::MakeMeshData();
		break;
	case FileType::File_Texture:
		fileRef = Texture::MakeTexture();
		break;
	case FileType::File_Scene:
		fileRef = Scene::MakeScene();
		break;
	case FileType::File_Header:
		fileRef = CodeFile::MakeCode(true);
		break;
	case FileType::File_Code:
		fileRef = CodeFile::MakeCode(false);
		break;
	case FileType::File_Skybox:
		fileRef = SkyBox::MakeSkyBox();
		break;
	case FileType::File_Font:
		fileRef = Font::MakeFont();
		break;
	case FileType::File_Material:
		fileRef = Material::MakeMaterial();
		break;
	case FileType::File_Shader:
		fileRef = Shader::MakeShader();
		break;
	case FileType::File_Icon:
		fileRef = Icon::MakeIcon();
		break;

	case FileType::File_Other:
		// Do nothing
		break;
	}

	if (fileRef)
	{
		fileRef->m_filePosition = fileInfo.filePos;
		fileRef->m_fileSize = fileInfo.fileSize;
		fileRef->m_metaPosition = fileInfo.metaFilePos;
		fileRef->m_metaSize = fileInfo.metaFileSize;

#if defined(EDITOR)
		fileRef->m_fileId = id;
#endif
		fileRef->m_file = fileInfo.file;
		fileRef->m_fileType = fileInfo.type;
		LoadMetaFile(*fileRef);
#if defined(EDITOR)
		SaveMetaFile(*fileRef);
#endif
	}
	return fileRef;
}

void ProjectManager::LoadMetaFile(FileReference& fileReference)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	const std::string path = fileReference.m_file->GetPath() + META_EXTENSION;

#if defined(EDITOR)
	std::shared_ptr<File> metaFile = FileSystem::MakeFile(path);
	if (metaFile->Open(FileMode::ReadOnly))
#else
	if(true)
#endif
	{
		std::string jsonString;
#if defined(EDITOR)
		jsonString = metaFile->ReadAll();
		metaFile->Close();
#else
		unsigned char* binData = ProjectManager::fileDataBase.bitFile.ReadBinary(fileReference.m_metaPosition, fileReference.m_metaSize);
		jsonString = std::string(reinterpret_cast<const char*>(binData), fileReference.m_metaSize);
		free(binData);
#endif

		json metaData;
		try
		{
			metaData = json::parse(jsonString);
		}
		catch (const std::exception&)
		{
			Debug::PrintError("[ProjectManager::LoadMetaFile] Meta file error", true);
			return;
		}

		if(Application::GetAssetPlatform() == AssetPlatform::AP_Standalone || Application::IsInEditor())
			ReflectionUtils::JsonToReflectiveData(metaData["Standalone"], fileReference.GetMetaReflectiveData(AssetPlatform::AP_Standalone));
		if (Application::GetAssetPlatform() == AssetPlatform::AP_PSP || Application::IsInEditor())
			ReflectionUtils::JsonToReflectiveData(metaData["PSP"], fileReference.GetMetaReflectiveData(AssetPlatform::AP_PSP));
		if (Application::GetAssetPlatform() == AssetPlatform::AP_PsVita || Application::IsInEditor())
			ReflectionUtils::JsonToReflectiveData(metaData["PSVITA"], fileReference.GetMetaReflectiveData(AssetPlatform::AP_PsVita));

		fileReference.m_file->SetUniqueId(metaData["id"]);
		fileReference.m_fileId = metaData["id"];
	}
	else
	{
		Debug::PrintError("[ProjectManager::LoadMetaFile] Cannot open the meta file" + path, true);
	}
}

ProjectDirectory::~ProjectDirectory()
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	subdirectories.clear();
}

std::string ProjectDirectory::GetFolderName()
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	if (path.size() == 0)
		return "";

	const size_t textLen = path.size();

	const size_t lastSlashPos = path.find_last_of('/', textLen - 2);

	const std::string fileName = path.substr(lastSlashPos + 1, textLen - lastSlashPos - 2);

	return fileName;
}

ReflectiveData ProjectSettings::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, projectName, "projectName", true);
	Reflective::AddVariable(reflectedVariables, gameName, "gameName", true);
	Reflective::AddVariable(reflectedVariables, companyName, "companyName", true);
	Reflective::AddVariable(reflectedVariables, startScene, "startScene", true);
	Reflective::AddVariable(reflectedVariables, engineVersion, "engineVersion", false);
	Reflective::AddVariable(reflectedVariables, compiledLibEngineVersion, "compiledLibEngineVersion", false);
	Reflective::AddVariable(reflectedVariables, isLibCompiledForDebug, "isLibCompiledForDebug", false);
	Reflective::AddVariable(reflectedVariables, isLibCompiledFor64Bits, "isLibCompiledFor64Bits", false);
	return reflectedVariables;
}
