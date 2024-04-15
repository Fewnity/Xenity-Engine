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

struct ProjectListItem
{
	std::string name;
	std::string path;
};

struct FileAndPath
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
	static std::shared_ptr<FileReference> GetFileReferenceById(const uint64_t id);
	static FileAndPath* GetFileById(const uint64_t id);

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
	* Get project name
	*/
	static std::string GetProjectName()
	{
		return projectSettings.projectName;
	}

	/**
	* Get game name
	*/
	static std::string GetGameName()
	{
		return projectSettings.gameName;
	}

	/**
	* Get game start scene
	*/
	static std::shared_ptr<Scene> GetStartScene()
	{
		return projectSettings.startScene;
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

	static ProjectSettings GetProjectSettings(const std::string& path);

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
	static FileType GetFileType(const std::string& extension);
	static std::vector<FileAndPath> GetFilesByType(const FileType type);
	static std::vector<uint64_t> GetAllUsedFileByTheGame();

	static std::shared_ptr<ProjectDirectory> GetProjectDirectory() 
	{
		return projectDirectory;
	}

	static ProjectSettings projectSettings;
	static std::shared_ptr <Directory> projectDirectoryBase;
	static std::shared_ptr <Directory> additionalAssetDirectoryBase;
private:
#if defined(EDITOR)
	static void OnProjectCompiled(CompilerParams params, bool result);
#endif
	static void FindAllProjectFiles();
	static void CreateVisualStudioSettings();

	static std::shared_ptr<FileReference> CreateFileReference(const std::string& path, const uint64_t id);
	static void LoadMetaFile(const std::shared_ptr<FileReference>& fileReference);

	static std::shared_ptr<ProjectDirectory> projectDirectory;
	static std::unordered_map<uint64_t, FileChange> oldProjectFilesIds;
	static std::unordered_map<uint64_t, FileAndPath> projectFilesIds;
	static bool projectLoaded;
	static std::string projectFolderPath;
	static std::string engineAssetsFolderPath;
	static std::string assetFolderPath;
};

