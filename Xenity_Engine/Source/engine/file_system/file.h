// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <memory>
#include <string>
#if defined(__PSP__)
	#include <pspkernel.h>
#endif

#include <engine/api.h>
#include <engine/unique_id/unique_id.h>

enum class FileMode 
{
	ReadOnly,
	WriteOnly,
	WriteCreateFile,
};

/**
* Class to manage a file (Create, open, read, write)
*/
class API File : public UniqueId, public std::enable_shared_from_this<File>
{
public:
	File() = delete;
	explicit File(const std::string& _path);
	~File() = default;

	/**
	* @brief Write string data to the file
	* @param data The data to write
	*/
	virtual void Write(const std::string& data) = 0;

	/**
	* @brief Write binary data to the file
	* @param data The data to write
	* @param size The size of the data in byte
	*/
	virtual void Write(const unsigned char* data, size_t size) = 0;

	/**
	* @brief Read all the content of the file as a string
	*/
	virtual std::string ReadAll() { return ""; };

	/**
	* @brief Read all the content of the file as a binary (Need to free the pointer after)
	* @param size Output: The size of the binary in byte
	* @return The binary data
	*/
	virtual unsigned char* ReadAllBinary(size_t& size) { return nullptr; };

	/**
	* @brief Read a part of the content of the file as a binary (Need to free the pointer after)
	* @param offset Read offset in byte
	* @param size The size to read in byte
	* @return The binary data
	*/
	virtual unsigned char* ReadBinary(size_t offset, size_t size) { return nullptr; };

	/**
	* @brief Check if the file exists
	* @return True if the file exists
	*/
	virtual bool CheckIfExist() { return false; };

	/**
	* @brief Open the file
	* @param fileMode The mode to open the file
	* @return True if the file is opened successfully
	*/
	virtual bool Open(FileMode fileMode) { return false; };

	/**
	* @brief Close file
	*/
	virtual void Close() {};

	/**
	* @brief Get file path
	*/
	inline const std::string& GetPath() const
	{
		return m_path;
	}

	/**
	* @brief Get file's folder path
	*/
	std::string GetFolderPath() const;

	/**
	* @brief Get file name
	*/
	const std::string& GetFileName() const;

	/**
	* @brief Get file extension (dot included)
	*/
	inline const std::string& GetFileExtension() const
	{
		return m_pathExtention;
	}

protected:
	FileMode m_currentFileMode = FileMode::ReadOnly;
	std::string m_path = "";
	std::string m_pathExtention = "";
	std::string m_name = "";
};