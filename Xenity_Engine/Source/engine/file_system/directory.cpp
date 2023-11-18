#include "directory.h"
#include "file_system.h"
#include <filesystem>

#if defined(__PSP__)
#include <dirent.h>
#include <sys/stat.h>
#elif defined(__vita__)
#include <psp2/io/stat.h>
#endif

Directory::Directory(std::string _path) : UniqueId(true)
{
#if defined(__vita__)
	_path = PSVITA_BASE_DIR + _path;
#endif
	this->path = _path;
}

Directory::~Directory()
{
	subdirectories.clear();
	files.clear();
}

void AddDirectoryFiles(std::vector<std::shared_ptr<File>>& vector, std::shared_ptr <Directory> directory)
{
	int fileCount = (int)directory->files.size();
	for (int i = 0; i < fileCount; i++)
	{
		vector.push_back(directory->files[i]);
	}

	int directoryCount = (int)directory->subdirectories.size();
	for (int i = 0; i < directoryCount; i++)
	{
		AddDirectoryFiles(vector, directory->subdirectories[i]);
	}
}

std::vector<std::shared_ptr<File>> Directory::GetAllFiles(bool recursive)
{
	FileSystem::fileSystem->FillDirectory(shared_from_this(), recursive);
	std::vector<std::shared_ptr<File>> vector;
	AddDirectoryFiles(vector, shared_from_this());
	return vector;
}

bool Directory::CheckIfExist()
{
	bool exists = false;
#if defined(__PSP__)
	DIR* dir = opendir(this->path.c_str());
	if (dir == NULL)
	{
		exists = false;
	}
	else {
		closedir(dir);
		exists = true;
	}
#else
	exists = std::filesystem::exists(this->path);
#endif
	return exists;
}