// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

#include <string>
#include <functional>
#include <thread>

class FileHandler
{
public:

	/**
	* @brief Check if code files have changed
	*/
	static bool HasCodeChanged(const std::string& folderPath);

	/**
	* @brief Check if files have changed or added
	*/
	static bool HasFileChangedOrAdded(const std::string& folderPath);

	/**
	* @brief Check if code files have changed
	*/
	static void HasCodeChangedThreaded(const std::string& folderPath, std::function<void()> callback);

	/**
	* @brief Check if files have changed or added
	*/
	static void HasFileChangedOrAddedThreaded(const std::string& folderPath, std::function<void()> callback);

	static void SetLastModifiedFile(const std::string& file);

	static void RemoveOneFile();
	static void AddOneFile();

private:

	/**
	* @brief Check if files have changed or added recursively
	*/
	static bool HasFileChangedOrAddedRecursive(const std::string& folderPath);

	static uint64_t lastModifiedCodeFileTime;
	static uint64_t lastModifiedFileTime;
	static uint32_t lastFileCount;
	static uint32_t tempFileCount;

	/**
	* @brief Check if code files have changed
	*/
	static bool HasCodeChangedDirect(const std::string& folderPath, bool isThreaded, std::function<void()> callback);

	/**
	* @brief Check if files have changed or added
	*/
	static bool HasFileChangedOrAddedDirect(const std::string& folderPath, bool isThreaded, std::function<void()> callback);
};

