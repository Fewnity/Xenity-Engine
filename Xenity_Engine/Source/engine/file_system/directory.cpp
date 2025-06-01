// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "directory.h"

#if defined(__PSP__)
#include <filesystem>
#include <dirent.h>
#include <sys/stat.h>
#elif defined(__vita__)
#include <filesystem>
#include <psp2/io/stat.h>
#elif defined(_EE)
#include <filesystem>
#define NEWLIB_PORT_AWARE
#include <fileXio_rpc.h>
#include <fileio.h>

#include <sifrpc.h>
#include <loadfile.h>
#include <sbv_patches.h>
#include <iopcontrol.h>
#include <iopheap.h>
#elif defined(__PS3__)
#else
#include <filesystem>
#endif

#include <engine/assertions/assertions.h>
#include "file_system.h"

Directory::Directory(std::string _path) : UniqueId(true)
{
	_path = FileSystem::ConvertWindowsPathToBasicPath(_path);

#if defined(_EE)
	_path = "mass:" + _path;
	//_path = "host0:" + _path;
	int pathLen = _path.size();
	for (int i = 0; i < pathLen; i++)
	{
		if (_path[i] == '\\')
		{
			_path[i] = '/';
		}
	}
#endif
	m_path = _path;
}

Directory::~Directory()
{
	subdirectories.clear();
	files.clear();
}

void AddDirectoryFiles(std::vector<std::shared_ptr<File>> &vector, const Directory& directory)
{
	const size_t fileCount = directory.files.size();
	for (size_t i = 0; i < fileCount; i++)
	{
		vector.push_back(directory.files[i]);
	}

	const size_t directoryCount = directory.subdirectories.size();
	for (size_t i = 0; i < directoryCount; i++)
	{
		AddDirectoryFiles(vector, *directory.subdirectories[i]);
	}
}

void Directory::FillDirectory(Directory& directory, bool recursive)
{
	directory.files.clear();
	directory.subdirectories.clear();
	if (!directory.CheckIfExist())
	{
		return;
	}
#if defined(__PSP__)
	DIR* dir = opendir(directory.GetPath().c_str());
	if (dir == NULL)
	{
		return;
	}
	struct dirent* ent;
	while ((ent = readdir(dir)) != NULL)
	{
		std::string found = ent->d_name;
		/*if (found == "." || found == "..")
			continue;*/
		if (found[0] == '.')
			continue;

		std::string fullPath = directory.GetPath() + found;
		struct stat statbuf;
		if (stat(fullPath.c_str(), &statbuf) == -1)
		{
			continue;
		}

		if (S_ISREG(statbuf.st_mode)) // If the entry is a file
		{
			std::shared_ptr<File> newFile = FileSystem::MakeFile(fullPath);
			directory.files.push_back(newFile);
		}
		else if (S_ISDIR(statbuf.st_mode)) // If the entry is a folder
		{
			std::shared_ptr<Directory> newDirectory = std::make_shared<Directory>(fullPath + "/");
			if (recursive)
				newDirectory->GetAllFiles(true);
			directory.subdirectories.push_back(newDirectory);
		}
	}
	closedir(dir);
#elif defined(_EE)
	Debug::Print("FillDirectory", true);
	std::string fullPath = directory.GetPath();
	int fd = fileXioDopen(directory.GetPath().c_str());
	if (fd >= 0)
	{
		iox_dirent_t dirent;
		int t = 0;

		std::vector<std::string> newDirs;

		while ((t = fileXioDread(fd, &dirent)) != 0)
		{
			// Debug::Print(std::to_string(dirent.stat.mode) + " " + std::string(dirent.name));
			if (std::string(dirent.name) == "." || std::string(dirent.name) == "..")
				continue;

			if (dirent.stat.mode == 8198 || dirent.stat.mode == 8199)
			{
				std::string path = directory.GetPath().substr(5) + std::string(dirent.name);
				// std::string path = directory.GetPath().substr(6) + std::string(dirent.name);
				//   path = path.substr(6);
				//   Debug::Print("IsFile " + path);
				std::shared_ptr<File> newFile = FileSystem::MakeFile(path);
				directory.files.push_back(newFile);
			}
			else if (dirent.stat.mode == 4103)
			{
				std::string path = directory.GetPath().substr(5) + std::string(dirent.name);
				// std::string path = directory.GetPath().substr(6) + std::string(dirent.name);
				newDirs.push_back(path);
				// Debug::Print("IsFolder " + path);
				//  path = path.substr(6);

				// std::shared_ptr<Directory> newDirectory = std::make_shared<Directory>(path + "\\");
				// if (recursive)
				// 	newdirectory.GetAllFiles(true);
				// directory.subdirectories.push_back(newDirectory);
			}
		}
		fileXioDclose(fd);

		int c = newDirs.size();
		for (int i = 0; i < c; i++)
		{
			std::shared_ptr<Directory> newDirectory = std::make_shared<Directory>(newDirs[i] + "/");
			if (recursive)
				newDirectory->GetAllFiles(true);
			directory.subdirectories.push_back(newDirectory);
		}
	}
#elif defined(__PS3__)
#else
	for (const auto& file : std::filesystem::directory_iterator(directory.GetPath()))
	{
		if (file.is_directory())
		{
			std::shared_ptr<Directory> newDirectory = nullptr;
			try
			{
				std::string path = file.path().string();

#if defined(_EE)
				path = path.substr(5);
				// path = path.substr(6);
#endif
				newDirectory = std::make_shared<Directory>(path + "/");
				if (recursive)
					newDirectory->GetAllFiles(true);
				directory.subdirectories.push_back(newDirectory);
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
#if defined(_EE)
				path = path.substr(5);
				// path = path.substr(6);
#endif
				newFile = FileSystem::MakeFile(path);
				directory.files.push_back(newFile);
			}
			catch (const std::exception&)
			{
			}
		}
	}
#endif
}

std::vector<std::shared_ptr<File>> Directory::GetAllFiles(bool recursive)
{
	FillDirectory(*this, recursive);
	std::vector<std::shared_ptr<File>> vector;
	AddDirectoryFiles(vector, *this);
	return vector;
}

bool Directory::CheckIfExist() const
{
	bool exists = false;
#if defined(__PSP__)
	DIR *dir = opendir(m_path.c_str());
	if (dir == NULL)
	{
		exists = false;
	}
	else
	{
		closedir(dir);
		exists = true;
	}
#elif defined(_EE)
	int fd = fileXioDopen(m_path.c_str());
	if (fd < 0)
	{
		exists = false;
	}
	else
	{
		exists = true;
		fileXioDclose(fd);
	}
#elif defined(__PS3__)
#else
	exists = std::filesystem::exists(m_path);
#endif
	return exists;
}