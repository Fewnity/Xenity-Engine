// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "file_system.h"

#include <string>

#if defined(_EE)
#include <filesystem>
#define NEWLIB_PORT_AWARE
#include <fileXio_rpc.h>
#include <fileio.h>

#include <sifrpc.h>
#include <loadfile.h>
#include <sbv_patches.h>
#include <iopcontrol.h>
#include <iopheap.h>
#elif defined(__PSP__) || defined(_EE)
#include <filesystem>
#include <dirent.h>
#include <sys/stat.h>
#elif defined(__vita__)
#include <filesystem>
#include <psp2/io/stat.h>
#elif defined(__PS3__)
#include <lv2/sysfs.h>
#else
#include <filesystem>
#endif

#include <engine/debug/debug.h>
#include <engine/constants.h>
#include <engine/application.h>

#include "directory.h"
#include "file.h"
#include "file_psp.h"
#include "file_ps2.h"
#include "file_ps3.h"
#include "file_default.h"

FileSystem* FileSystem::s_fileSystem = nullptr;

#pragma region File

#pragma endregion

#pragma region Directory

void FileSystem::FillDirectory(const std::shared_ptr<Directory>& directory, bool recursive)
{
	XASSERT(directory != nullptr, "[FileSystem::FillDirectory] directory is nullptr");

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
		std::string found = ent->d_name;
		/*if (found == "." || found == "..")
			continue;*/
		if (found[0] == '.')
			continue;

		std::string fullPath = directory->GetPath() + found;
		struct stat statbuf;
		if (stat(fullPath.c_str(), &statbuf) == -1)
		{
			continue;
		}

		if (S_ISREG(statbuf.st_mode)) // If the entry is a file
		{
			std::shared_ptr<File> newFile = FileSystem::MakeFile(fullPath);
			directory->files.push_back(newFile);
		}
		else if (S_ISDIR(statbuf.st_mode)) // If the entry is a folder
		{
			std::shared_ptr<Directory> newDirectory = std::make_shared<Directory>(fullPath + "/");
			if (recursive)
				newDirectory->GetAllFiles(true);
			directory->subdirectories.push_back(newDirectory);
		}
	}
	closedir(dir);
#elif defined(_EE)
	Debug::Print("FillDirectory", true);
	std::string fullPath = directory->GetPath();
	int fd = fileXioDopen(directory->GetPath().c_str());
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
				std::string path = directory->GetPath().substr(5) + std::string(dirent.name);
				// std::string path = directory->GetPath().substr(6) + std::string(dirent.name);
				//   path = path.substr(6);
				//   Debug::Print("IsFile " + path);
				std::shared_ptr<File> newFile = FileSystem::MakeFile(path);
				directory->files.push_back(newFile);
			}
			else if (dirent.stat.mode == 4103)
			{
				std::string path = directory->GetPath().substr(5) + std::string(dirent.name);
				// std::string path = directory->GetPath().substr(6) + std::string(dirent.name);
				newDirs.push_back(path);
				// Debug::Print("IsFolder " + path);
				//  path = path.substr(6);

				// std::shared_ptr<Directory> newDirectory = std::make_shared<Directory>(path + "\\");
				// if (recursive)
				// 	newDirectory->GetAllFiles(true);
				// directory->subdirectories.push_back(newDirectory);
			}
		}
		fileXioDclose(fd);

		int c = newDirs.size();
		for (int i = 0; i < c; i++)
		{
			std::shared_ptr<Directory> newDirectory = std::make_shared<Directory>(newDirs[i] + "/");
			if (recursive)
				newDirectory->GetAllFiles(true);
			directory->subdirectories.push_back(newDirectory);
		}
	}
#elif defined(__PS3__)
#else
	for (const auto& file : std::filesystem::directory_iterator(directory->GetPath()))
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
#if defined(_EE)
				path = path.substr(5);
				// path = path.substr(6);
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
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
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

int FileSystem::CopyFile(const std::string& path, const std::string& newPath, bool replace)
{
	int result = 0;
#if !defined(__PS3__)
	try
	{
		if (!replace)
		{
			if (std::filesystem::exists(newPath))
			{
				result = -1; // File already exists
			}
		}

		std::filesystem::copy_options option = std::filesystem::copy_options::none;

		if (replace)
			option |= std::filesystem::copy_options::overwrite_existing;

		if (result == 0)
			std::filesystem::copy_file(path, newPath, option);
	}
	catch (const std::exception&)
	{
		result = -2; // Error
	}
#endif
	return result;
}

//std::vector<std::shared_ptr<File>> files;

std::shared_ptr<File> FileSystem::MakeFile(const std::string& path)
{
	XASSERT(!path.empty(), "[FileSystem::MakeFile] path is empty");

	std::shared_ptr<File> file = nullptr;

	/*size_t fileCount = files.size();
	for (size_t i = 0; i < fileCount; i++)
	{
		if (files[i]->GetPath() == path)
		{
			file = files[i];
			break;
		}
	}*/

	/*if (!file)
	{*/
#if defined(__PSP__)
	file = std::make_shared<FilePSP>(path);
#elif defined(__PS3__)
	file = std::make_shared<FilePS3>(path);
#elif defined(_EE)
	file = std::make_shared<FilePS2>(path);
	// file = std::make_shared<FileDefault>(path);
#else
	file = std::make_shared<FileDefault>(path);
#endif
	//files.push_back(file);
//}

	return file;
}

std::string FileSystem::ConvertWindowsPathToBasicPath(const std::string& path)
{
	const size_t pathSize = path.size();
	std::string newPath = path;
	for (size_t i = 0; i < pathSize; i++)
	{
		if (path[i] == '\\')
		{
			newPath[i] = '/';
		}
	}
	return newPath;
}

std::string FileSystem::ConvertBasicPathToWindowsPath(const std::string& path)
{
	const size_t pathSize = path.size();
	std::string newPath = path;
	for (size_t i = 0; i < pathSize; i++)
	{
		if (path[i] == '/')
		{
			newPath[i] = '\\';
		}
	}
	return newPath;
}


#pragma endregion

#pragma region Read/Input

#pragma endregion

bool FileSystem::CreateFolder(const std::string& path)
{
	XASSERT(!path.empty(), "[FileSystem::CreateFolder] path is empty");
	bool result = true;
#if !defined(__PS3__)
	try
	{
		std::filesystem::create_directory(path);
	}
	catch (const std::exception&)
	{
		result = false;
	}
#endif
	return result;
}

void FileSystem::Delete(const std::string& path)
{
	XASSERT(!path.empty(), "[FileSystem::Delete] path is empty");
#if defined(_EE)
	return;
#endif

#if defined(__PSP__)
	sceIoRemove(path.c_str());
#elif defined(__PS3__)
	sysFsRmdir((Application::GetMountingPoint() + std::string(PS3_DATA_FOLDER) + path).c_str()); // Remove if dir
	sysFsUnlink((Application::GetMountingPoint() + std::string(PS3_DATA_FOLDER) + path).c_str()); // Remove if file
#else
	try
	{
		std::filesystem::remove_all(path.c_str());
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
	sceIoMkdir("ux0:/data/xenity_engine/screenshots", 0777);
#endif
#if defined(_EE)
	// 	SifInitRpc(0);
	// 	while (!SifIopReset(NULL, 0))
	// 	{
	// 	}
	// 	while (!SifIopSync())
	// 	{
	// 	}
	// 	SifInitRpc(0);

	// 	// SifInitIopHeap();
	// 	// // SifLoadFileInit();

	// 	// sbv_patch_enable_lmb();
	// 	// sbv_patch_disable_prefix_check();
	// 	// sbv_patch_fileio();

	// 	int ret = SifLoadModule("host0:iomanX.irx", 0, NULL);
	// 	int ret2 = SifLoadModule("host0:fileXio.irx", 0, NULL);

	// 	fileXioInitSkipOverride();
	// 	// fileXioSetBlockMode(0);

	// 	if (ret < 0)
	// 		Debug::PrintError("Failed to load iomanX.irx");
	// 	if (ret2 < 0)
	// 		Debug::PrintError("Failed to load fileXio.irx");

	// 	if (ret >= 0 && ret2 >= 0)
	// 	{
	// 		Debug::Print("-------- PS2 File System initiated --------");
	// 	}
#endif
	Debug::Print("-------- File System initiated --------", true);
	return 0;
}