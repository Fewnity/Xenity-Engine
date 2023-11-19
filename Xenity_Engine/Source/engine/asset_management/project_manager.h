#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#define PROJECT_SETTINGS_FILE_NAME "project_settings.json"

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include "../reflection/reflection.h"
#include "../file_system/file_reference.h"

class FileReference;
class File;
class Scene;
class Directory;

#define META_EXTENSION ".meta"
#define PROJECTS_LIST_FILE "projects.json"

class API ProjectDirectory
{
public:
	ProjectDirectory() = delete;
	ProjectDirectory(const std::string& path, uint64_t uniqueId)
	{
		this->path = path;
		this->uniqueId = uniqueId;
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

class ProjectListItem
{
public:
	std::string name;
	std::string path;
};

class FileAndPath
{
public:
	std::string path;
	std::shared_ptr<File> file;
};

class FileChange
{
public:
	bool hasChanged = false;
	bool hasBeenDeleted = true;
	std::string path;
};

class API ProjectManager
{
public:

	/**
	* Create a project
	* @param name Name of the project
	* @param folderPath Project folder parent
	*/
	static bool CreateProject(const std::string& name, const std::string& folderPath);

	/**
	* Load a project
	* @param projectPathToLoad Project path
	*/
	static bool LoadProject(const std::string& projectPathToLoad);

	/**
	* Unload a project
	*/
	static void UnloadProject();

	/**
	* Get file reference by Id
	* @param id File reference Id
	*/
	static std::shared_ptr<FileReference> GetFileReferenceById(uint64_t id);

	/**
	* Save the meta file of a file reference
	*/
	static void SaveMetaFile(const std::shared_ptr<FileReference>& fileReference);

	/**
	* Load project settings
	*/
	static void LoadProjectSettings();

	/**
	* Save project settings
	*/
	static void SaveProjectSettings();

	/**
	* Get reflection of project settings
	*/
	static ReflectiveData GetProjetSettingsReflection();

	/**
	* Get project name
	*/
	static std::string GetProjectName()
	{
		return projectName;
	}

	/**
	* Get game name
	*/
	static std::string GetGameName()
	{
		return gameName;
	}

	/**
	* Get game start scene
	*/
	static std::shared_ptr<Scene> GetStartScene()
	{
		return startScene;
	}

	/**
	* Get project folder path
	*/
	static std::string GetProjectFolderPath()
	{
		return projectFolderPath;
	}

	/**
	* Get asset folder path
	*/
	static std::string GetAssetFolderPath()
	{
		return assetFolderPath;
	}

	/**
	* Get engine asset folder path
	*/
	static std::string GetEngineAssetFolderPath()
	{
		return engineAssetsFolderPath;
	}

	/**
	* Get if the project is loaded
	*/
	static bool GetIsProjectLoaded()
	{
		return projectLoaded;
	}

	/**
	* Get opened projects list
	*/
	static std::vector<ProjectListItem> GetProjectsList();

	/**
	* Save opened projects list
	*/
	static void SaveProjectsList(const std::vector<ProjectListItem>& projects);

	static void FillProjectDirectory(std::shared_ptr <ProjectDirectory> realProjectDirectory);
	static void CreateProjectDirectories(std::shared_ptr <Directory> projectDirectoryBase, std::shared_ptr <ProjectDirectory> realProjectDirectory);
	static void RefreshProjectDirectory();

	/**
	* Find and get a project directory from a path and a parent directory
	*/
	static std::shared_ptr <ProjectDirectory> FindProjectDirectory(std::shared_ptr <ProjectDirectory> directoryToCheck, const std::string& directoryPath);
	static FileType GetFileType(std::string extension);

	static std::shared_ptr<ProjectDirectory> GetProjectDirectory() 
	{
		return projectDirectory;
	}

private:
	static void FindAllProjectFiles();
	static std::shared_ptr<FileReference> CreateFilReference(const std::string& path, uint64_t id);
	static void LoadMetaFile(const std::shared_ptr<FileReference>& fileReference);

	static std::shared_ptr<ProjectDirectory> projectDirectory;
	static std::shared_ptr <Directory> projectDirectoryBase;
	static std::unordered_map<uint64_t, FileChange> oldProjectFilesIds;
	static std::unordered_map<uint64_t, FileAndPath> projectFilesIds;
	static bool projectLoaded;
	static std::string projectName;
	static std::string gameName;
	static std::shared_ptr<Scene> startScene;
	static std::string projectFolderPath;
	static std::string engineAssetsFolderPath;
	static std::string assetFolderPath;
};

