// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

#include <mutex>
#include <memory>
#include <vector>

class FileReference;

class AsyncFileLoading
{
public:

	/**
	* @brief Finish file loading when files are loaded with a thread
	*/
	static void FinishThreadedFileLoading();

	/**
	* @brief Add a file loading with a thread
	* @param file File to load
	*/
	static void AddFile(const std::shared_ptr<FileReference>& file);

private:
	static std::vector<std::shared_ptr<FileReference>> threadLoadedFiles;
	static std::mutex threadLoadingMutex;
};

