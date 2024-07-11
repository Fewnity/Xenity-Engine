// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#if defined(__PSP__)
#include <pspkernel.h>
#endif

#include <engine/unique_id/unique_id.h>
#include <string>
#include <fstream>
#include <vector>
#include <memory>

#if defined(__vita__)
#define PSVITA_PATH R"(ux0:data\xenity_engine\)"
#define PSVITA_BASE_DIR "ux0:"
#endif

class Directory;
class File;

class API FileSystem
{
public:
	static FileSystem* fileSystem;


	/**
	* @brief Create a directory
	* @param path Directory path
	* @return True if success
	*/
	bool CreateFolder(const std::string& path);

	/**
	* @brief Delete a file
	* @param path File path
	*/
	void Delete(const std::string& path);

	/**
	* @brief Get all files of a directory and fill it
	* @param directory Directory to fill
	*/
	void FillDirectory(std::shared_ptr <Directory> directory, bool recursive);

	bool Rename(const std::string& path, const std::string& newPath);

	int CopyFile(const std::string& path, const std::string& newPath, bool replace);

	/**
	 * @brief [Internal] ?
	 */
	static std::shared_ptr<File> MakeFile(const std::string& path);

	static std::string ConvertWindowsPathToBasicPath(const std::string& path);

	static std::string ConvertBasicPathToWindowsPath(const std::string& path);

private:
	friend class Engine;

	/**
	* @brief [Internal] Init file system
	* @return 0 if success
	*/
	int InitFileSystem();
};