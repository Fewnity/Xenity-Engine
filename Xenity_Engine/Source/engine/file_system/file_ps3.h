// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

#if defined(__PS3__)
#include <engine/api.h>

#include "file.h"

#include <string>
#include <vector>

class Directory;
class File;

class API FilePS3 : public File
{
public:
	FilePS3() = delete;
	explicit FilePS3(const std::string& _path);
	FilePS3(const FilePS3& other) = delete;
	FilePS3& operator=(const FilePS3&) = delete;

	~FilePS3();

	/**
	* @brief Open the file
	* @param fileMode The mode to open the file
	* @return True if the file is opened successfully
	*/
	bool Open(FileMode fileMode) override;

	/**
	* @brief Close the file
	*/
	void Close() override;

	/**
	* @brief Check if the file exists
	* @return True if the file exists
	*/
	bool CheckIfExist() override;

	/**
	* @brief Read all the content of the file as a string
	*/
	std::string ReadAll() override;

	/**
	* @brief Read all the content of the file as a binary (Need to free the pointer after)
	* @param size Output: The size of the binary
	* @return The binary data
	*/
	unsigned char* ReadAllBinary(size_t& size) override;

	/**
	* @brief Read a part of the content of the file as a binary (Need to free the pointer after)
	* @param offset Read offset in byte
	* @param size The size to read in byte
	* @return The binary data
	*/
	unsigned char* ReadBinary(size_t offset, size_t size) override;

	/**
	* @brief Write string data to the file
	* @param data The data to write
	*/
	void Write(const std::string& data) override;

	/**
	* @brief Write binary data to the file
	* @param data The data to write
	* @param size The size of the data in byte
	*/
	void Write(const unsigned char* data, size_t size) override;

protected:
	int32_t m_fileId = -1;
};

#endif