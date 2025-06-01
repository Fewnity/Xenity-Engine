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
	Success, // File copied successfully.
	Failed, // Failed to copy the file.
	FileAlreadyExists, // The file already exists.Enable the replace option.
};

/**
* @brief File system API
* @brief This class provides methods to manipulate files and directories.
*/
class API FileSystem
{
public:

	/**
	* @brief Create a directory
	* @param path Directory path
	* @return True if success
	*/
	static bool CreateFolder(const std::string& path);

	/**
	* @brief Delete a file or a directory
	* @param path File path
	*/
	static void Delete(const std::string& path);

	/**
	* @brief Rename a file or a directory (Not currently supported on PSP and PS3)
	* @param path File path to rename
	* @param newPath New file path (should be in the same directory)
	*/
	static bool Rename(const std::string& path, const std::string& newPath);

	/**
	* @brief Copy a file to a new path
	* @param path File path to copy
	* @param newPath New file path
	* @param replace If true, replace the file if it already exists
	*/
	static CopyFileResult CopyFile(const std::string& path, const std::string& newPath, bool replace);

	/**
	 * @brief Create a File object from a path (Do not create the file on the system)
	 */
	[[nodiscard]] static std::shared_ptr<File> MakeFile(const std::string& path);

	/**
	 * @brief Create a Directory object from a path (Do not create the directory on the system)
	 */
	[[nodiscard]] static std::shared_ptr<Directory> MakeDirectory(const std::string& path);

	/**
	* @brief Converts all backslashes ('\\') to forward slashes ('/') in the given path.
	*/
	[[nodiscard]] static std::string ConvertWindowsPathToBasicPath(const std::string& path);

	/**
	* @brief Converts all forward slashes ('/') to backslashes ('\\') in the given path.
	*/
	[[nodiscard]] static std::string ConvertBasicPathToWindowsPath(const std::string& path);

private:
	friend class Engine;
	friend class Directory;

	/**
	* @brief [Internal] Init file system
	* @return 0 if success
	*/
	[[nodiscard]] static int InitFileSystem();
};