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
	std::string GetFolderName();
	std::string path = "";
	std::vector<ProjectDirectory*> subdirectories;
	std::vector<FileReference*> files;
};

class API ProjectManager
{
public:

	static ProjectDirectory* projectDirectory;
	static void LoadProject(std::string projectPathToLoad);
	static std::unordered_map<uint64_t, FileReference*> projectFilesRef;
	static FileReference* GetFileReferenceById(uint64_t id);
	static void SaveMetaFile(FileReference* fileReference);
	static void LoadProjectSettings();
	static void SaveProjectSettigs();
	static std::unordered_map<std::string, Variable> GetProjetSettingsReflection();

	static std::string GetProjectName() 
	{
		return projectName;
	}

	static std::string GetGameName() 
	{
		return gameName;
	}

	static Scene* GetStartScene() 
	{
		return startScene;
	}

	static std::string GetProjectFolderPath() 
	{
		return projectFolderPath;
	}

	static std::string GetAssetFolderPath()
	{
		return assetFolderPath;
	}

	static std::string GetEngineAssetFolderPath()
	{
		return engineAssetsFolderPath;
	}

	static bool GetIsProjectLoaded() 
	{
		return projectLoaded;
	}

private:
	static void LoadMetaFile(FileReference* fileReference);
	static bool projectLoaded;
	static std::string projectName;
	static std::string gameName;
	static Scene* startScene;
	static std::string projectFolderPath;
	static std::string engineAssetsFolderPath;
	static std::string assetFolderPath;
	struct PairFile 
	{
		File* file;
		File* meta;
	};
};

