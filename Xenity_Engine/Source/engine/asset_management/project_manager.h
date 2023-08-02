#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <unordered_map>
#include <vector>
#include <string>

class FileReference;
class File;

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
	static void LoadProject();
	static std::unordered_map<uint64_t, FileReference*> projectFilesRef;
	static FileReference* GetFileReferenceById(uint64_t id);
	static void SaveMetaFile(FileReference* fileReference);
	static void LoadProjectSettings();
	static void SaveProjectSettigs();
	static std::string GetProjectName() {
		return projectName;
	}
	static std::string GetGameName() {
		return gameName;
	}

private:
	static void LoadMetaFile(FileReference* fileReference);
	static std::string projectName;
	static std::string gameName;
	struct PairFile 
	{
		File* file;
		File* meta;
	};
};

