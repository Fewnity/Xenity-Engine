#pragma once

#include <unordered_map>
#include <vector>
#include <string>

class FileReference;
class File;

class ProjectDirectory 
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

class ProjectManager
{
public:

	static ProjectDirectory* projectDirectory;
	static void LoadProject();
	static std::unordered_map<uint64_t, FileReference*> projectFilesRef;
	static FileReference* GetFileReferenceById(uint64_t id);
	static void SaveMetaFile(FileReference* fileReference);

private:
	static void LoadMetaFile(FileReference* fileReference);
	struct PairFile {
		File* file;
		File* meta;
	};
};

