#include "file_system.h"

#include "directory.h"
#include "file.h"
#include "file_psp.h"
#include "file_default.h"

#include <engine/debug/debug.h>

#if defined(__PSP__)
	#include <dirent.h>
	#include <sys/stat.h>
#elif defined(__vita__)
	#include <psp2/io/stat.h>
#endif

#include <filesystem>
#include <string>


FileSystem* FileSystem::fileSystem = nullptr;

#pragma region File


#pragma endregion

#pragma region Directory

void FileSystem::FillDirectory(std::shared_ptr <Directory> directory, bool recursive)
{
	directory->files.clear();
	directory->subdirectories.clear();
	if (!directory->CheckIfExist())
	{
		return;
	}
#if defined(__PSP__)
	DIR* dir = opendir(directory->GetPath().c_str());
	if (dir == NULL)
	{
		return;
	}
	struct dirent* ent;
	while ((ent = readdir(dir)) != NULL)
	{
		std::string found = "";
		found += ent->d_name;
		if (found == "." || found == "..")
			continue;

		std::string fullPath = directory->GetPath() + found;
		struct stat statbuf;
		if (stat(fullPath.c_str(), &statbuf) == -1)
		{
			continue;
		}

		if (S_ISREG(statbuf.st_mode)) // If the entry is a file
		{
			std::shared_ptr<File> newFile = nullptr;
			try
			{
				newFile = FileSystem::MakeFile(fullPath);
				directory->files.push_back(newFile);
			}
			catch (const std::exception&)
			{
			}
		}
		else if (S_ISDIR(statbuf.st_mode)) // If the entry is a folder
		{
			std::shared_ptr <Directory> newDirectory = nullptr;
			try
			{
				newDirectory = std::make_shared<Directory>(fullPath + "\\");
				if (recursive)
					newDirectory->GetAllFiles(true);
				directory->subdirectories.push_back(newDirectory);
			}
			catch (const std::exception&)
			{
			}
		}
	}
	closedir(dir);
#else
	for (const auto& file : std::filesystem::directory_iterator(directory->GetPath()))
	{
		if (file.is_directory())
		{
			std::shared_ptr <Directory> newDirectory = nullptr;
			try
			{
				std::string path = file.path().string();
#if defined(__vita__)
				path = path.substr(4);
#elif defined(_EE)
				path = path.substr(6);
#endif
				newDirectory = std::make_shared<Directory>(path + "\\");
				if (recursive)
					newDirectory->GetAllFiles(true);
				directory->subdirectories.push_back(newDirectory);
			}
			catch (const std::exception&)
			{
			}
		}
		else if (file.is_regular_file())
		{
			std::shared_ptr<File> newFile = nullptr;
			try
			{
				std::string path = file.path().string();
#if defined(__vita__)
				path = path.substr(4);
#elif defined(_EE)
				path = path.substr(6);
#endif
				newFile = FileSystem::MakeFile(path);
				directory->files.push_back(newFile);
			}
			catch (const std::exception&)
			{
			}
		}
	}
#endif
}

bool FileSystem::Rename(const std::string& path, const std::string& newPath)
{
	bool success = true;
	try
	{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
		if (std::filesystem::exists(newPath)) 
		{
			success = false;
		}
		else 
		{
			std::filesystem::rename(path, newPath);
		}
#endif
	}
	catch (const std::exception&)
	{
		success = false;
	}
	return success;
}

std::vector< std::shared_ptr<File>> files;

std::shared_ptr<File> FileSystem::MakeFile(const std::string& path)
{
	std::shared_ptr<File> file;

	size_t fileCount = files.size();
	for (size_t i = 0; i < fileCount; i++)
	{
		if (files[i]->GetPath() == path)
		{
			file = files[i];
			break;
		}
	}

	if (!file)
	{
#if defined(__PSP__)
		file = std::make_shared<FilePSP>(path);
#else
		file = std::make_shared<FileDefault>(path);
#endif
		files.push_back(file);
	}

	return file;
}

#pragma endregion

#pragma region Read/Input

#pragma endregion

void FileSystem::CreateDirectory(const std::string& path)
{
	std::string finalPath = path;
#if defined(__vita__)
	finalPath = PSVITA_BASE_DIR + finalPath;
#endif
	std::filesystem::create_directory(finalPath);
}

void FileSystem::DeleteFile(const std::string& path)
{
	std::string newPath = path;
#if defined(__vita__)
	newPath = PSVITA_BASE_DIR + path;
#endif

#if defined(__PSP__)
	sceIoRemove(newPath.c_str());
#else
	try
	{
		std::filesystem::remove_all(newPath.c_str());
	}
	catch (const std::exception&)
	{

	}
#endif
}

#pragma region Write/Output

#pragma endregion

int FileSystem::InitFileSystem()
{
#if defined(__vita__)
	sceIoMkdir("ux0:/data/xenity_engine", 0777);
#endif

	Debug::Print("-------- File System initiated --------");
	return 0;
}