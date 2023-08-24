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
#include "../reflection/reflection.h"

class FileReference;
class File;
class Scene;

class API ProjectDirectory
{
public:
	ProjectDirectory() = delete;
	ProjectDirectory(std::string path)
	{
		this->path = path;
	}

	/**
	* Get folder name
	*/
	std::string GetFolderName();
	std::string path = "";
	std::vector<ProjectDirectory*> subdirectories;
	std::vector<FileReference*> files;
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


	/**
	* Get file reference by Id
	* @param id File reference Id
	*/
	static FileReference* GetFileReferenceById(uint64_t id);

	/**
	* Save the meta file of a file reference
	*/
	static void SaveMetaFile(FileReference* fileReference);

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
	static std::unordered_map<std::string, Variable> GetProjetSettingsReflection();

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
	static Scene* GetStartScene() 
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

	static ProjectDirectory* projectDirectory;
	static std::unordered_map<uint64_t, FileReference*> projectFilesRef;

private:
	static void LoadMetaFile(FileReference* fileReference);
	static bool projectLoaded;
	static std::string projectName;
	static std::string gameName;
	static Scene* startScene;
	static std::string projectFolderPath;
	static std::string engineAssetsFolderPath;
	static std::string assetFolderPath;
};

