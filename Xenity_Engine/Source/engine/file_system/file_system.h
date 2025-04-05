// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <string>
#include <memory>
#if defined(__PSP__)
#include <pspkernel.h>
#endif

#include <engine/api.h>
#include <engine/unique_id/unique_id.h>

class Directory;
class File;

enum class CopyFileResult
{
	Success,
	Failed,
	FileAlreadyExists,
};

class API FileSystem
{
public:
	static FileSystem* s_fileSystem;

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
	void FillDirectory(const std::shared_ptr <Directory>& directory, bool recursive);

	bool Rename(const std::string& path, const std::string& newPath);

	CopyFileResult CopyFile(const std::string& path, const std::string& newPath, bool replace);

	/**
	 * @brief [Internal] ?
	 */
	[[nodiscard]] static std::shared_ptr<File> MakeFile(const std::string& path);

	[[nodiscard]] static std::string ConvertWindowsPathToBasicPath(const std::string& path);

	[[nodiscard]] static std::string ConvertBasicPathToWindowsPath(const std::string& path);

private:
	friend class Engine;

	/**
	* @brief [Internal] Init file system
	* @return 0 if success
	*/
	[[nodiscard]] int InitFileSystem();
};