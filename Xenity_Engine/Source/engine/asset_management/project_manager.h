// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal] Classes not visible to users
 */

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <set>

#include <engine/reflection/reflection.h>
#include <engine/event_system/event_system.h>
#include <engine/asset_management/project_list_item.h>
#include <engine/file_system/data_base/file_data_base.h>
#include <engine/file_system/file_system.h>
#include <engine/project_management/project_errors.h>

class FileReference;
class File;
class Scene;
class Directory;
struct CompilerParams;

enum ProjectState 
{
	NotLoaded,
	Loading,
	Loaded,
};

class ProjectDirectory
{
public:
	ProjectDirectory() = delete;
	/**
	* @param _path Path of the Directory
	* @param _uniqueId Unique Id of the Directory
	*/
	ProjectDirectory(const std::string& _path, uint64_t _uniqueId)
	{
		path = FileSystem::ConvertWindowsPathToBasicPath(_path);
		uniqueId = _uniqueId;
	}
	~ProjectDirectory();

	/**
	* Get folder name
	*/
	[[nodiscard]] std::string GetFolderName();

	std::string path = "";
	std::vector<std::shared_ptr<ProjectDirectory>> subdirectories;
	std::vector<std::shared_ptr<FileReference>> files;
	uint64_t uniqueId = 0;
};

struct FileAndId
{
	std::shared_ptr<File> file;
	uint64_t id = 0;
};

struct FileInfo
{
	FileAndId fileAndId;

	// File position/size in the binary file
	uint64_t filePos = 0;
	uint64_t fileSize = 0;
	uint64_t metaFilePos = 0;
	uint64_t metaFileSize = 0;

	FileType type = FileType::File_Other;
	bool isEngineAsset = false;
};

struct FileChange
{
	std::string path;
	bool hasChanged = false;
	bool hasBeenDeleted = true;
};

class ProjectSettings : public Reflective
{
public:
	std::string gameName = "";
	std::string projectName = "";
	std::string companyName = "";
	std::string engineVersion = "0.0";
	std::string compiledLibEngineVersion = "0";
	std::shared_ptr<Scene> startScene = nullptr;
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
	[[nodiscard]] static bool CreateProject(const std::string& name, const std::string& folderPath);

	/**
	* @brief Load a project
	* @param projectPathToLoad Project path
	* @return True if the project has been loaded
	*/
	[[nodiscard]] static ProjectLoadingErrors LoadProject(const std::string& projectPathToLoad);

	/**
	* @brief Unload a project
	*/
	static void UnloadProject();

	/**
	* @brief Get file reference by Id (does not load the file reference)
	* @param id File reference Id
	* @return File reference (nullptr if not found)
	*/
	[[nodiscard]] static std::shared_ptr<FileReference> GetFileReferenceById(const uint64_t id);

	/**
	* @brief Get file reference by file (does not load the file reference)
	* @param file File
	* @return File reference (nullptr if not found)
	*/
	[[nodiscard]] static std::shared_ptr<FileReference> GetFileReferenceByFile(const File& file);

	/**
	* @brief Get file reference by file path (does not load the file reference)
	* @param filePath File path
	* @return File reference (nullptr if not found)
	*/
	[[nodiscard]] static std::shared_ptr<FileReference> GetFileReferenceByFilePath(const std::string& filePath);

	/**
	* @brief Get file by Id
	* @param id File Id
	* @return File
	*/
	[[nodiscard]] static FileInfo* GetFileById(const uint64_t id);

	/**
	* @brief Save the meta file of a file reference
	* @param fileReference save meta file of this file reference
	*/
	static void SaveMetaFile(FileReference& fileReference);

	/**
	* @brief Save project settings
	*/
	static void SaveProjectSettings();
	static void SaveProjectSettings(const std::string& folderPath);

	/**
	* @brief Get project name
	*/
	[[nodiscard]] static inline std::string GetProjectName()
	{
		return s_projectSettings.projectName;
	}

	/**
	* @brief Get game name
	*/
	[[nodiscard]] static inline std::string GetGameName()
	{
		return s_projectSettings.gameName;
	}

	/**
	* @brief Get game start scene
	*/
	[[nodiscard]] static inline std::shared_ptr<Scene> GetStartScene()
	{
		return s_projectSettings.startScene;
	}

	/**
	* @brief Get project folder path
	*/
	[[nodiscard]] static inline const std::string& GetProjectFolderPath()
	{
		return s_projectFolderPath;
	}

	/**
	* @brief Get asset folder path
	*/
	[[nodiscard]] static inline const std::string& GetAssetFolderPath()
	{
		return s_assetFolderPath;
	}

	/**
	* @brief Get engine asset folder path
	*/
	[[nodiscard]] static inline const std::string& GetEngineAssetFolderPath()
	{
		return s_engineAssetsFolderPath;
	}

	/**
	* @brief Get engine asset folder path
	*/
	[[nodiscard]] static inline const std::string& GetPublicEngineAssetFolderPath()
	{
		return s_publicEngineAssetsFolderPath;
	}

	/**
	* @brief Get if the project is loaded
	*/
	[[nodiscard]] static inline bool IsProjectLoaded()
	{
		return s_projectLoaded;
	}

	[[nodiscard]] static inline ProjectState GetProjectState()
	{
		return s_projectState;
	}

	static inline void SetProjectState(ProjectState projectState)
	{
		s_projectState = projectState;
	}

	/**
	* @brief Get project settings
	* @param path Project path
	*/
	[[nodiscard]] static ProjectSettings GetProjectSettings(const std::string& projectPath);

	/**
	* @brief Get opened projects list
	*/
	[[nodiscard]] static std::vector<ProjectListItem> GetProjectsList();

	/**
	* @brief Save opened projects list
	* @param projects Projects list
	*/
	static void SaveProjectsList(const std::vector<ProjectListItem>& projects);

	/**
	* @brief Fill project directory with all files (files sorted in editor mode)
	* @param _projectDirectory ProjectDirectory to fill
	*/
	static void FillProjectDirectory(ProjectDirectory& _projectDirectory);

	/**
	* @brief Refresh project directory
	*/
	static void RefreshProjectDirectory();

	/**
	* @brief Get a number between 0 and 1 that represents the loading progress of the project
	*/
	static float GetLoadingProgress()
	{
		if(s_totalFilesCount == 0)
			return 0.0f;

		return (float)s_loadedFilesCount  / (float)s_totalFilesCount;
	}

	/**
	* @brief Find and get a project directory from a path and a parent directory
	* @param directoryToCheck ProjectDirectory to use to find the ProjectDirectory from the path
	* @param directoryPath Path of the directory to find
	*/
	[[nodiscard]] static std::shared_ptr <ProjectDirectory> FindProjectDirectory(const ProjectDirectory& directoryToCheck, const std::string& directoryPath);

	/**
	* @brief Get file type from extension
	* @param extension File extension
	* @return File type
	*/
	[[nodiscard]] static FileType GetFileType(const std::string& extension);

	/**
	* @brief Get all files by type
	* @param type File type
	* @return Files
	*/
	[[nodiscard]] static std::vector<FileInfo> GetFilesByType(const FileType type);

	/**
	* @brief Get all used files by the game
	*/
	[[nodiscard]] static std::set<uint64_t> GetAllUsedFileByTheGame();

	/**
	* @brief Get project directory
	*/
	[[nodiscard]] static std::shared_ptr<ProjectDirectory> GetProjectDirectory()
	{
		return s_projectDirectory;
	}

	/**
	* Get the event that is called when a project is loaded
	*/
	[[nodiscard]] static inline Event<>& GetProjectLoadedEvent()
	{
		return s_projectLoadedEvent;
	}

	/**
	* Get the event that is called when a project is unloaded
	*/
	[[nodiscard]] static inline Event<>& GetProjectUnloadedEvent()
	{
		return s_projectUnloadedEvent;
	}

	static ProjectSettings s_projectSettings;
	static std::shared_ptr <Directory> s_projectDirectoryBase;
	static std::shared_ptr <Directory> s_publicEngineAssetsDirectoryBase;
	static std::shared_ptr <Directory> s_additionalAssetDirectoryBase;
	static FileDataBase s_fileDataBase;

private:
	/**
	* @brief Load project settings
	*/
	static void LoadProjectSettings();

	/**
	* @brief Create project directories from projectDirectoryBase
	* @param projectDirectoryBase From
	* @param realProjectDirectory To
	*/
	static void CreateProjectDirectories(Directory& projectDirectoryBase, ProjectDirectory& realProjectDirectory);

	[[nodiscard]] static uint64_t ReadFileId(const File& file);

	/**
	* @brief Create a file reference pointer and load the meta file (if editor mode, create a meta file too)
	* @param path File path
	* @param id File Id
	* @return File reference
	*/
	[[nodiscard]] static std::shared_ptr<FileReference> CreateFileReference(const std::string& path, const uint64_t id, const FileType type);
	[[nodiscard]] static std::shared_ptr<FileReference> CreateFileReference(const FileInfo& fileInfo, const uint64_t id);

	/**
	* @brief Add all files of a directory to a list of project file list
	* @param projectFilesDestination Destination of the files to add
	* @param directorySource Source directory
	* @param isEngineAssets Are the assets to add engine assets?
	*/
	static void AddFilesToProjectFiles(std::vector<FileInfo>& projectFilesDestination, Directory& directorySource, bool isEngineAssets);

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
	static void LoadMetaFile(FileReference& fileReference);

	static void CreateGame();

	[[nodiscard]] static std::vector<FileInfo> GetCompatibleFiles();

	static void CheckAndGenerateFileIds(std::vector<FileInfo>& compatibleFiles);

	static std::shared_ptr<ProjectDirectory> s_projectDirectory;
	static std::unordered_map<uint64_t, FileInfo> s_projectFilesIds;
	static bool s_projectLoaded;
	static ProjectState s_projectState;
	static std::string s_projectFolderPath;
	static std::string s_engineAssetsFolderPath;
	static std::string s_publicEngineAssetsFolderPath;
	static std::string s_assetFolderPath;
	static constexpr int s_metaVersion = 1;

	static Event<> s_projectLoadedEvent;
	static Event<> s_projectUnloadedEvent;
	static int s_loadedFilesCount;
	static int s_totalFilesCount;
};

