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
	ProjectDirectory(std::string path)
	{
		this->path = path;
	}
	~ProjectDirectory();

	/**
	* Get folder name
	*/
	std::string GetFolderName();
	std::string path = "";
	std::vector<ProjectDirectory*> subdirectories;
	std::vector<std::shared_ptr<FileReference>> files;
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
	static bool CreateProject(std::string name, std::string folderPath);

	/**
	* Load a project
	* @param projectPathToLoad Project path
	*/
	static bool LoadProject(std::string projectPathToLoad);

	static void UnloadProject();

	/**
	* Get file reference by Id
	* @param id File reference Id
	*/
	static std::shared_ptr<FileReference> GetFileReferenceById(uint64_t id);

	/**
	* Save the meta file of a file reference
	*/
	static void SaveMetaFile(std::shared_ptr<FileReference> fileReference);

	/**
	* Load project settings
	*/
	static void LoadProjectSettings();

	/**
	* Save project settings
	*/
	static void SaveProjectSettigs();

	/**
	* Get reflection of project settings
	*/
	static std::unordered_map<std::string, ReflectionEntry> GetProjetSettingsReflection();

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
	static void SaveProjectsList(std::vector<ProjectListItem> projects);

	static ProjectDirectory* projectDirectory;
	static std::unordered_map<uint64_t, FileChange> oldProjectFilesIds;
	static std::unordered_map<uint64_t, FileAndPath> projectFilesIds;
	static void FillProjectDirectory(ProjectDirectory* realProjectDirectory);
	static void CreateProjectDirectories(Directory* projectDirectoryBase, ProjectDirectory* realProjectDirectory);
	static void RefreshProjectDirectory();
	static void FindAllProjectFiles();

	/**
	* Find and get a project directory from a path and a parent directory
	*/
	static ProjectDirectory* FindProjectDirectory(ProjectDirectory* directoryToCheck, std::string directoryPath);
	static Directory* projectDirectoryBase;
	static FileType GetFileType(std::string extension);

private:
	static std::shared_ptr<FileReference> CreateFilReference(std::string path, int id);
	static void LoadMetaFile(std::shared_ptr<FileReference> fileReference);
	static bool projectLoaded;
	static std::string projectName;
	static std::string gameName;
	static std::shared_ptr<Scene> startScene;
	static std::string projectFolderPath;
	static std::string engineAssetsFolderPath;
	static std::string assetFolderPath;
};

