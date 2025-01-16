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

void AddDirectoryFiles(std::vector<std::shared_ptr<File>> &vector, Directory& directory)
{
	//XASSERT(directory != nullptr, "[Directory::AddDirectoryFiles] directory is nullptr");

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

std::vector<std::shared_ptr<File>> Directory::GetAllFiles(bool recursive)
{
	FileSystem::s_fileSystem->FillDirectory(shared_from_this(), recursive);
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