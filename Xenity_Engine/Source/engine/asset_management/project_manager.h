// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

#define PROJECT_SETTINGS_FILE_NAME "project_settings.json"

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <engine/reflection/reflection.h>
#include <engine/file_system/file_reference.h>
#include <engine/event_system/event_system.h>

class FileReference;
class File;
class Scene;
class Directory;
class CompilerParams;

#define META_EXTENSION ".meta"
#define PROJECTS_LIST_FILE "projects.json"

class ProjectDirectory
{
public:
	ProjectDirectory() = delete;
	ProjectDirectory(const std::string& _path, uint64_t _uniqueId) : path(_path)
	{
		uniqueId = _uniqueId;
	}
	~ProjectDirectory();

	/**
	* Get folder name
	*/
	std::string GetFolderName();
	std::string path = "";
	std::vector<std::shared_ptr<ProjectDirectory>> subdirectories;
	std::vector<std::shared_ptr<FileReference>> files;
	uint64_t uniqueId = 0;
};

struct ProjectListItem
{
	std::string name;
	std::string path;
};

class ProjectEngineFile 
{
public:
	std::shared_ptr<File> file = nullptr;
	bool isEngineAsset = false;
};

class CompatibleFile
{
public:
	ProjectEngineFile file;
	FileType type;
};

struct FileInfo
{
	std::string path;
	std::shared_ptr<File> file;
	FileType type;
};

struct FileChange
{
	bool hasChanged = false;
	bool hasBeenDeleted = true;
	std::string path;
};

class ProjectSettings : public Reflective
{
public:
	std::string gameName = "";
	std::string projectName = "";
	std::shared_ptr<Scene> startScene = nullptr;
	std::string engineVersion = "0.0";
	std::string compiledLibEngineVersion = "0";
	bool isCompiled = false;
	bool isLibCompiledForDebug = false;
	bool isLibCompiledFor64Bits = false;
	ReflectiveData GetReflectiveData() override;
};

class ProjectManager
{
public:

	/**
	* Init the Project Manager
	*/
	static void Init();

	/**
	* @brief Create a project
	* @param name Name of the project
	* @param folderPath Project folder parent
	* @return True if the project has been created
	*/
	static bool CreateProject(const std::string& name, const std::string& folderPath);

	/**
	* @brief Load a project
	* @param projectPathToLoad Project path
	* @return True if the project has been loaded
	*/
	static bool LoadProject(const std::string& projectPathToLoad);

	/**
	* @brief Unload a project
	*/
	static void UnloadProject();

	/**
	* @brief Get file reference by Id (does not load the file reference)
	* @param id File reference Id
	* @return File reference (nullptr if not found)
	*/
	static std::shared_ptr<FileReference> GetFileReferenceById(const uint64_t id);

	/**
	* @brief Get file reference by file (does not load the file reference)
	* @param file File
	* @return File reference (nullptr if not found)
	*/
	static std::shared_ptr<FileReference> GetFileReferenceByFile(std::shared_ptr<File> file);

	/**
	* @brief Get file reference by file path (does not load the file reference)
	* @param filePath File path
	* @return File reference (nullptr if not found)
	*/
	static std::shared_ptr<FileReference> GetFileReferenceByFilePath(const std::string& filePath);

	/**
	* @brief Get file by Id
	* @param id File Id
	* @return File
	*/
	static FileInfo* GetFileById(const uint64_t id);

	/**
	* @brief Save the meta file of a file reference
	*/
	static void SaveMetaFile(const std::shared_ptr<FileReference>& fileReference);

	/**
	* @brief Load project settings
	*/
	static void LoadProjectSettings();

	/**
	* @brief Save project settings
	*/
	static void SaveProjectSettings();

	/**
	* @brief Get project name
	*/
	static std::string GetProjectName()
	{
		return projectSettings.projectName;
	}

	/**
	* @brief Get game name
	*/
	static std::string GetGameName()
	{
		return projectSettings.gameName;
	}

	/**
	* @brief Get game start scene
	*/
	static std::shared_ptr<Scene> GetStartScene()
	{
		return projectSettings.startScene;
	}

	/**
	* @brief Get project folder path
	*/
	static const std::string& GetProjectFolderPath()
	{
		return projectFolderPath;
	}

	/**
	* @brief Get asset folder path
	*/
	static const std::string& GetAssetFolderPath()
	{
		return assetFolderPath;
	}

	/**
	* @brief Get engine asset folder path
	*/
	static const std::string& GetEngineAssetFolderPath()
	{
		return engineAssetsFolderPath;
	}

	/**
	* @brief Get engine asset folder path
	*/
	static const std::string& GetPublicEngineAssetFolderPath()
	{
		return publicEngineAssetsFolderPath;
	}

	/**
	* @brief Get if the project is loaded
	*/
	static bool GetIsProjectLoaded()
	{
		return projectLoaded;
	}

	/**
	* @brief Get project settings
	* @param path Project path
	*/
	static ProjectSettings GetProjectSettings(const std::string& projectPath);

	/**
	* @brief Get opened projects list
	*/
	static std::vector<ProjectListItem> GetProjectsList();

	/**
	* @brief Save opened projects list
	* @param projects Projects list
	*/
	static void SaveProjectsList(const std::vector<ProjectListItem>& projects);

	/**
	* @brief Fill project directory with all files and directories
	*/
	static void FillProjectDirectory(std::shared_ptr <ProjectDirectory> realProjectDirectory);

	/**
	* @brief Create project directories
	*/
	static void CreateProjectDirectories(std::shared_ptr <Directory> projectDirectoryBase, std::shared_ptr <ProjectDirectory> realProjectDirectory);

	/**
	* @brief Refresh project directory
	*/
	static void RefreshProjectDirectory();

	/**
	* @brief Find and get a project directory from a path and a parent directory
	*/
	static std::shared_ptr <ProjectDirectory> FindProjectDirectory(std::shared_ptr <ProjectDirectory> directoryToCheck, const std::string& directoryPath);

	/**
	* @brief Get file type from extension
	* @param extension File extension
	* @return File type
	*/
	static FileType GetFileType(const std::string& extension);

	/**
	* @brief Get all files by type
	* @param type File type
	* @return Files
	*/
	static std::vector<FileInfo> GetFilesByType(const FileType type);

	/**
	* @brief Get all used files by the game
	*/
	static std::vector<uint64_t> GetAllUsedFileByTheGame();

	/**
	* @brief Get project directory
	*/
	static std::shared_ptr<ProjectDirectory> GetProjectDirectory() 
	{
		return projectDirectory;
	}

	static uint64_t ReadFileId(const std::shared_ptr<File>& file);

	/**
* @brief Create a file reference pointer and load the meta file (if editor mode, create a meta file too)
* @param path File path
* @param id File Id
* @return File reference
*/
	static std::shared_ptr<FileReference> CreateFileReference(const std::string& path, const uint64_t id);

	static Event<>& GetProjectLoadedEvent()
	{
		return projectLoadedEvent;
	}

	static Event<>& GetProjectUnloadedEvent()
	{
		return projectUnloadedEvent;
	}

	static ProjectSettings projectSettings;
	static std::shared_ptr <Directory> projectDirectoryBase;
	static std::shared_ptr <Directory> publicEngineAssetsDirectoryBase;
	static std::shared_ptr <Directory> additionalAssetDirectoryBase;
private:

	static void AddFilesToProjectFiles(std::vector<ProjectEngineFile>& projectFilesDestination, std::shared_ptr<Directory> directorySource, bool isEngineAssets);

#if defined(EDITOR)
	/**
	* @brief Event called when the project is compiled
	* @param params Compiler parameters
	* @param result Compilation result
	*/
	static void OnProjectCompiled(CompilerParams params, bool result);

#endif

	/**
	* @brief Find all project files
	*/
	static void FindAllProjectFiles();

	/**
	* @brief Create Visual Studio Code settings file
	*/
	static void CreateVisualStudioSettings();



	/**
	* @brief Load meta file
	* @param fileReference File reference
	*/
	static void LoadMetaFile(const std::shared_ptr<FileReference>& fileReference);

	static std::shared_ptr<ProjectDirectory> projectDirectory;
	static std::unordered_map<uint64_t, FileChange> oldProjectFilesIds;
	static std::unordered_map<uint64_t, FileInfo> projectFilesIds;
	static bool projectLoaded;
	static std::string projectFolderPath;
	static std::string engineAssetsFolderPath;
	static std::string publicEngineAssetsFolderPath;
	static std::string assetFolderPath;
	static constexpr int metaVersion = 1;

	static Event<> projectLoadedEvent;
	static Event<> projectUnloadedEvent;
};

