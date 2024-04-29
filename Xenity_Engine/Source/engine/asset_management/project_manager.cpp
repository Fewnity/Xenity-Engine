#include "project_manager.h"

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
#include <json.hpp>

#include <engine/debug/debug.h>
#include <engine/game_elements/gameplay_manager.h>
#if defined(EDITOR)
#include <editor/ui/editor_ui.h>
#include <editor/editor.h>
#include <editor/file_handler.h>
#include <editor/compiler.h>
#include <editor/file_reference_finder.h>
#endif
#if !defined(EDITOR) && !defined(_WIN32) && !defined(_WIN64)
#include "game_code/source/game.h"
#endif
#include <engine/engine_settings.h>
#include <engine/tools/string_tag_finder.h>

using json = nlohmann::json;

const int ProjectManager::metaVersion = 1;

std::unordered_map<uint64_t, FileChange> ProjectManager::oldProjectFilesIds;
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

std::shared_ptr<ProjectDirectory> ProjectManager::FindProjectDirectory(std::shared_ptr<ProjectDirectory> directoryToCheck, const std::string& directoryPath)
{
	if (!directoryToCheck)
		return nullptr;

	const size_t dirCount = directoryToCheck->subdirectories.size();
	for (size_t i = 0; i < dirCount; i++)
	{
		std::shared_ptr<ProjectDirectory> subDir = directoryToCheck->subdirectories[i];
		// Check if the sub directory is the directory to find
		if (subDir->path == directoryPath)
		{
			return subDir;
		}
		else
		{
			// Start recursive to search in the sub directory
			std::shared_ptr<ProjectDirectory> foundSubDir = FindProjectDirectory(subDir, directoryPath);
			if (foundSubDir)
				return foundSubDir;
		}
	}
	return nullptr;
}

uint64_t ProjectManager::ReadFileId(const std::shared_ptr<File>& file)
{
	uint64_t id = -1;
	const std::string metaFilePath = file->GetPath() + META_EXTENSION;
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

void ProjectManager::AddFilesToProjectFiles(std::vector<ProjectEngineFile>& projectFilesDestination, std::shared_ptr<Directory> directorySource, bool isEngineAssets)
{
	std::vector<std::shared_ptr<File>> projectAssetFiles = directorySource->GetAllFiles(true);
	const int projectAssetFilesCount = (int)projectAssetFiles.size();
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
	// Keep in memory the old opened directory path to re-open it later
	std::string oldPath = "";
#if defined(EDITOR)
	if (Editor::GetCurrentProjectDirectory())
		oldPath = Editor::GetCurrentProjectDirectory()->path;

	Editor::SetCurrentProjectDirectory(nullptr);
#endif

	// Keep in memory all old files path to check later if some files have been moved
	for (const auto& kv : projectFilesIds)
	{
		FileChange fileChange = FileChange();
		fileChange.path = kv.second.path;
		oldProjectFilesIds[kv.first] = fileChange;
	}

	projectFilesIds.clear();

	// Get all files of the project
	std::vector<ProjectEngineFile> projectFiles;

	AddFilesToProjectFiles(projectFiles, publicEngineAssetsDirectoryBase, true); // This directory first
	AddFilesToProjectFiles(projectFiles, projectDirectoryBase, false);
	AddFilesToProjectFiles(projectFiles, additionalAssetDirectoryBase, false);

	projectDirectory = std::make_shared<ProjectDirectory>(assetFolderPath, 0);

	std::vector<CompatibleFile> compatibleFiles;

	// Get all files supported by the engine
	const int fileCount = (int)projectFiles.size();
	for (int i = 0; i < fileCount; i++)
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

	// Read meta files and list all files that do not have a meta file for later use
	std::unordered_map<uint64_t, bool> usedIds;
	std::vector<std::shared_ptr<File>> fileWithoutMeta;
	int fileWithoutMetaCount = 0;
	uint64_t biggestId = UniqueId::reservedFileId;

	UniqueId::ResetIds();

	for (auto& compatibleFile : compatibleFiles)
	{
		std::shared_ptr<File> file = compatibleFile.file.file;
		uint64_t fileId = ReadFileId(file);
		if (fileId == -1)
		{
			fileWithoutMeta.push_back(file);
			fileWithoutMetaCount++;
		}
		else
		{
#if defined(EDITOR)
			if (!compatibleFile.file.isEngineAsset && (usedIds[fileId] == true || fileId <= UniqueId::reservedFileId))
				//if (usedIds[id] == true)
			{
				Debug::PrintError("[ProjectManager::FindAllProjectFiles] Id already used by another file! Id: " + std::to_string(fileId) + ", File:" + file->GetPath() + ".meta", true);
				fileWithoutMeta.push_back(file);
				fileWithoutMetaCount++;
				continue;
			}
#endif

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

	// Fill projectFilesIds
	for (const auto& kv : compatibleFiles)
	{
		FileInfo fileAndPath = FileInfo();
		fileAndPath.file = kv.file.file;
		fileAndPath.path = kv.file.file->GetPath();
		projectFilesIds[kv.file.file->GetUniqueId()] = fileAndPath;
	}

	// Create files references
	for (auto& kv : projectFilesIds)
	{
		std::shared_ptr<FileReference> fileRef = CreateFileReference(kv.second.path, kv.first);
		kv.second.type = fileRef->fileType;
	}

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
			GetFileReferenceById(kv.first)->file = projectFilesIds[kv.first].file;
		}
		else if (kv.second.hasBeenDeleted)
		{
			AssetManager::ForceDeleteFileReference(GetFileReferenceById(kv.first));
		}
	}

#if defined(EDITOR)
	// Get all project directories and open one
	CreateProjectDirectories(projectDirectoryBase, projectDirectory);
	std::shared_ptr<ProjectDirectory> lastOpenedDir = FindProjectDirectory(projectDirectory, oldPath);
	if (lastOpenedDir)
		Editor::SetCurrentProjectDirectory(lastOpenedDir);
	else
		Editor::SetCurrentProjectDirectory(projectDirectory);
#endif

	oldProjectFilesIds.clear();
	fileWithoutMeta.clear();
	compatibleFiles.clear();
}

void ProjectManager::CreateVisualStudioSettings()
{
	try
	{
		// Get engine includes folder
		const std::filesystem::path exePath = std::filesystem::canonical("./");
		const std::string includesPath = exePath.generic_string() + "/includes/";

		// Read the empty vscode settings file
		std::shared_ptr<File> emptyVSCodeParamFile = FileSystem::MakeFile(".\\vscodeSample\\c_cpp_properties.json");

		bool isOpen = emptyVSCodeParamFile->Open(FileMode::ReadOnly);
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
			FileSystem::fileSystem->CreateFolder(assetFolderPath + ".vscode\\");

			const std::string filePath = assetFolderPath + ".vscode\\c_cpp_properties.json";
			FileSystem::fileSystem->Delete(filePath);

			// Create the vscode settings file
			std::shared_ptr<File> vsCodeParamFile = FileSystem::MakeFile(filePath);
			const bool isOpen = vsCodeParamFile->Open(FileMode::WriteCreateFile);
			if (isOpen)
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

void ProjectManager::CreateProjectDirectories(std::shared_ptr<Directory> projectDirectoryBase, std::shared_ptr<ProjectDirectory> realProjectDirectory)
{
	const size_t dirCount = projectDirectoryBase->subdirectories.size();
	for (size_t i = 0; i < dirCount; i++)
	{
		std::shared_ptr<ProjectDirectory> newDir = std::make_shared<ProjectDirectory>(projectDirectoryBase->subdirectories[i]->GetPath(), projectDirectoryBase->subdirectories[i]->GetUniqueId());
		realProjectDirectory->subdirectories.push_back(newDir);
		CreateProjectDirectories(projectDirectoryBase->subdirectories[i], newDir);
	}
}

void ProjectManager::RefreshProjectDirectory()
{
	FindAllProjectFiles();
}

void ProjectManager::FillProjectDirectory(std::shared_ptr<ProjectDirectory> realProjectDirectory)
{
	realProjectDirectory->files.clear();

	for (const auto& kv : ProjectManager::projectFilesIds)
	{
		// Check if this file is in this folder
		if (realProjectDirectory->path == kv.second.file->GetFolderPath())
		{
			realProjectDirectory->files.push_back(ProjectManager::GetFileReferenceById(kv.first));
		}
	}
}

void ProjectManager::Init()
{
	engineAssetsFolderPath = ".\\engine_assets\\";
	publicEngineAssetsFolderPath = ".\\public_engine_assets\\";

	publicEngineAssetsDirectoryBase = std::make_shared<Directory>(publicEngineAssetsFolderPath);
}

bool ProjectManager::CreateProject(const std::string& name, const std::string& folderPath)
{
	FileSystem::fileSystem->CreateFolder(folderPath + name + "\\");
	FileSystem::fileSystem->CreateFolder(folderPath + name + "\\temp\\");
	FileSystem::fileSystem->CreateFolder(folderPath + name + "\\additional_assets\\");
	FileSystem::fileSystem->CreateFolder(folderPath + name + "\\assets\\");
	FileSystem::fileSystem->CreateFolder(folderPath + name + "\\assets\\Scripts\\");
	FileSystem::fileSystem->CreateFolder(folderPath + name + "\\assets\\Scenes\\");

	// Create default scene
	std::shared_ptr<Scene> sceneRef = std::dynamic_pointer_cast<Scene>(CreateFileReference(folderPath + name + "\\assets\\Scenes\\MainScene.xen", UniqueId::GenerateUniqueId(true)));
	if (sceneRef->file->Open(FileMode::WriteCreateFile))
	{
		const std::string data = AssetManager::GetDefaultFileData(FileType::File_Scene);
		sceneRef->file->Write(data);
		sceneRef->file->Close();
	}

	// TODO improve this (use copy entry system like in the compiler class)
	try
	{
		std::filesystem::copy_file("engine_assets\\empty_default\\game.cpp", folderPath + name + "\\assets\\game.cpp", std::filesystem::copy_options::overwrite_existing);
		std::filesystem::copy_file("engine_assets\\empty_default\\game.h", folderPath + name + "\\assets\\game.h", std::filesystem::copy_options::overwrite_existing);
	}
	catch (const std::exception&)
	{
		Debug::PrintError("[ProjectManager::CreateProject] Error when copying default assets into the project.", true);
	}

	projectSettings.projectName = name;
	projectSettings.gameName = name;
	projectSettings.startScene = sceneRef;
	projectFolderPath = folderPath + name + "\\";
	SaveProjectSettings();

	return LoadProject(projectFolderPath);
}

FileType ProjectManager::GetFileType(const std::string& _extension)
{
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
	else if (extension == ".mat") // If the file is a font
	{
		fileType = FileType::File_Material;
	}
	else if (extension == ".shader") // If the file is a font
	{
		fileType = FileType::File_Shader;
	}

	return fileType;
}

#if defined(EDITOR)

void ProjectManager::OnProjectCompiled(CompilerParams params, bool result)
{
	if (params.buildType != BuildType::EditorHotReloading)
		return;

	if (result)
	{
		projectSettings.compiledLibEngineVersion = ENGINE_DLL_VERSION;
	}
	else
	{
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
#if defined(EDITOR)
	Compiler::GetOnCompilationEndedEvent().Bind(&ProjectManager::OnProjectCompiled);
#endif
	Debug::Print("Loading project: " + projectPathToLoad, true);
	projectLoaded = false;

	projectFolderPath = projectPathToLoad;
	assetFolderPath = projectPathToLoad + "assets\\";


	projectDirectoryBase = std::make_shared<Directory>(assetFolderPath);
	if (!projectDirectoryBase->CheckIfExist())
	{
		return projectLoaded;
	}

	FileSystem::fileSystem->CreateFolder(projectFolderPath + "\\temp\\");
	FileSystem::fileSystem->CreateFolder(projectFolderPath + "\\additional_assets\\");

	additionalAssetDirectoryBase = std::make_shared<Directory>(projectFolderPath + "\\additional_assets\\");


	FindAllProjectFiles();

	LoadProjectSettings();
	projectSettings.engineVersion = ENGINE_VERSION;
#if defined(EDITOR)
	SaveProjectSettings();
#endif

	// Load dynamic library and create game
#if defined(_WIN32) || defined(_WIN64)
	bool isDebugMode = false;
	bool is64Bits = false;
#if defined(_WIN64)
	is64Bits = true;
#endif
#if defined(DEBUG)
	isDebugMode = true;
#endif
	if (projectSettings.compiledLibEngineVersion == ENGINE_DLL_VERSION && projectSettings.isLibCompiledForDebug == isDebugMode && projectSettings.isLibCompiledFor64Bits == is64Bits)
	{
#if defined(EDITOR)
		DynamicLibrary::LoadGameLibrary(ProjectManager::GetProjectFolderPath() + "temp\\game_editor");
#else
		DynamicLibrary::LoadGameLibrary("game");
#endif // defined(EDITOR)
		Engine::game = DynamicLibrary::CreateGame();
	}
	else
	{
		// Maybe automaticaly recompile the project
		Debug::PrintWarning("The project was compiled with another version of the engine, please recompile the game.");
	}
#else
	Engine::game = std::make_unique<Game>();
#endif //  defined(_WIN32) || defined(_WIN64)

	// Fill class registery
	if (Engine::game)
		Engine::game->Start();

#if defined(EDITOR)
	CreateVisualStudioSettings();

	// Check files to avoid triggerring a compilation
	FileHandler::HasCodeChanged(GetAssetFolderPath());
	FileHandler::HasFileChangedOrAdded(GetAssetFolderPath());

	if (EngineSettings::compileWhenOpeningProject)
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
#if defined(EDITOR)
	Editor::SetCurrentProjectDirectory(nullptr);
#endif
	SceneManager::SetSceneModified(false);
	SceneManager::SetOpenedScene(nullptr);
	SceneManager::CreateEmptyScene();
	Graphics::SetDefaultValues();

	projectSettings.startScene.reset();
	projectDirectoryBase.reset();
	additionalAssetDirectoryBase.reset();
	projectDirectory.reset();
	projectFilesIds.clear();
	oldProjectFilesIds.clear();
	projectLoaded = false;
	projectSettings.projectName.clear();
	projectSettings.gameName.clear();
	Window::UpdateWindowTitle();

	projectUnloadedEvent.Trigger();
}

std::vector<uint64_t> ProjectManager::GetAllUsedFileByTheGame()
{
	std::vector<uint64_t> ids;
#if defined(EDITOR)
	int idCount = 0;
	const std::vector<FileInfo> sceneFiles = GetFilesByType(FileType::File_Scene);
	const int sceneCount = sceneFiles.size();
	for (int i = 0; i < sceneCount; i++)
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
	if (projectFilesIds.contains(id))
	{
		return &projectFilesIds[id];
	}

	return nullptr;
}

std::shared_ptr<FileReference> ProjectManager::GetFileReferenceById(const uint64_t id)
{
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
		if (tempFileRef->fileId == id)
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
			fileRef = CreateFileReference(projectFilesIds[id].path, id);
			if (fileRef)
			{
				if (fileRef->fileType == FileType::File_Skybox)
					fileRef->LoadFileReference();
			}
		}
	}

	return fileRef;
}

std::shared_ptr<FileReference> ProjectManager::GetFileReferenceByFile(std::shared_ptr<File> file)
{
	const uint64_t fileId = ProjectManager::ReadFileId(file);
	return GetFileReferenceById(fileId);
}

std::shared_ptr<FileReference> ProjectManager::GetFileReferenceByFilePath(const std::string filePath)
{
	std::shared_ptr<File> file = FileSystem::MakeFile(filePath);
	const uint64_t fileId = ProjectManager::ReadFileId(file);

	return GetFileReferenceById(fileId);
}

ProjectSettings ProjectManager::GetProjectSettings(const std::string& projectPath)
{
	ProjectSettings settings;
	std::shared_ptr<File> projectFile = FileSystem::MakeFile(projectPath + PROJECT_SETTINGS_FILE_NAME);
	if (projectFile->CheckIfExist())
	{
		std::string jsonString = "";

		// Read file
		if (projectFile->Open(FileMode::ReadOnly))
		{
			jsonString = projectFile->ReadAll();
			projectFile->Close();
		}
		else
		{
			Debug::PrintError("[ProjectManager::LoadProjectSettings] Fail to open the project settings file", true);
		}

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
				Debug::PrintError("[ProjectManager::LoadProjectSettings] Meta file error", true);
				return settings;
			}

			// Change settings
			ReflectionUtils::JsonToReflectiveData(projectData, settings.GetReflectiveData());
		}
	}
	return settings;
}

void ProjectManager::LoadProjectSettings()
{
	projectSettings = GetProjectSettings(projectFolderPath);
}

void ProjectManager::SaveProjectSettings()
{
	const std::string path = projectFolderPath + PROJECT_SETTINGS_FILE_NAME;
	FileSystem::fileSystem->Delete(path);
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

void ProjectManager::SaveMetaFile(const std::shared_ptr<FileReference>& fileReference)
{
	std::shared_ptr<File> file = fileReference->file;
	if (!file)
		return;

	FileSystem::fileSystem->Delete(file->GetPath() + META_EXTENSION);
	json metaData;
	metaData["id"] = fileReference->fileId;
	metaData["MetaVersion"] = metaVersion;
	metaData["Values"] = ReflectionUtils::ReflectiveDataToJson(fileReference->GetMetaReflectiveData());

	std::shared_ptr<File> metaFile = FileSystem::MakeFile(file->GetPath() + META_EXTENSION);
	if (metaFile->Open(FileMode::WriteCreateFile))
	{
		metaFile->Write(metaData.dump(0));
		metaFile->Close();
	}
	else
	{
		Debug::PrintError("[ProjectManager::SaveMetaFile] Cannot save meta file: " + file->GetPath(), true);
	}
}

std::vector<ProjectListItem> ProjectManager::GetProjectsList()
{
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
	const size_t projectSize = projects.size();
	json j;
	for (size_t i = 0; i < projectSize; i++)
	{
		j[i]["name"] = projects[i].name;
		j[i]["path"] = projects[i].path;
	}
	FileSystem::fileSystem->Delete(PROJECTS_LIST_FILE);
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

	case FileType::File_Other:
		// Do nothing
		break;
	}

	if (fileRef)
	{
		fileRef->fileId = id;
		fileRef->file = file;
		fileRef->fileType = type;
		LoadMetaFile(fileRef);
#if defined(EDITOR)
		SaveMetaFile(fileRef);
#endif
	}
	return fileRef;
}

void ProjectManager::LoadMetaFile(const std::shared_ptr<FileReference>& fileReference)
{
	const std::string path = fileReference->file->GetPath() + META_EXTENSION;
	std::shared_ptr<File> metaFile = FileSystem::MakeFile(path);
	if (metaFile->CheckIfExist())
	{
		if (metaFile->Open(FileMode::ReadOnly))
		{
			std::string jsonString = "";
			jsonString = metaFile->ReadAll();
			metaFile->Close();

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
			ReflectionUtils::JsonToReflectiveData(metaData, fileReference->GetMetaReflectiveData());
		}
		else
		{
			Debug::PrintError("[ProjectManager::LoadMetaFile] Cannot open the meta file" + path, true);
		}
	}
}

ProjectDirectory::~ProjectDirectory()
{
	subdirectories.clear();
}

std::string ProjectDirectory::GetFolderName()
{
	if (path.size() == 0)
		return "";

	const size_t textLen = path.size();

	const size_t lastSlashPos = path.find_last_of('\\', textLen - 2);

	const std::string fileName = path.substr(lastSlashPos + 1, textLen - lastSlashPos - 2);

	return fileName;
}

ReflectiveData ProjectSettings::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, projectName, "projectName", true);
	Reflective::AddVariable(reflectedVariables, gameName, "gameName", true);
	Reflective::AddVariable(reflectedVariables, startScene, "startScene", true);
	Reflective::AddVariable(reflectedVariables, engineVersion, "engineVersion", false);
	Reflective::AddVariable(reflectedVariables, compiledLibEngineVersion, "compiledLibEngineVersion", false);
	Reflective::AddVariable(reflectedVariables, isLibCompiledForDebug, "isLibCompiledForDebug", false);
	Reflective::AddVariable(reflectedVariables, isLibCompiledFor64Bits, "isLibCompiledFor64Bits", false);
	return reflectedVariables;
}
