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
#include <vector>
#include <memory>
#include <string>
#include <fstream>

enum class FileMode 
{
	ReadOnly,
	WriteOnly,
	WriteCreateFile,
};

class API File : public UniqueId, public std::enable_shared_from_this<File>
{
public:
	File() = delete;
	File(std::string _path);
	~File() = default;

	/**
	* @brief Write string data to the file
	* @param data The data to write
	*/
	virtual void Write(const std::string& data) {};

	/**
	* @brief Read all the content of the file as a string
	*/
	virtual std::string ReadAll() { return ""; };

	/**
	* @brief Read all the content of the file as a binary (Need to free the pointer after)
	* @param size Output: The size of the binary
	* @return The binary data
	*/
	virtual unsigned char* ReadAllBinary(int& size) { return nullptr; };

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
	const std::string& GetPath() const
	{
		return path;
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
	const std::string& GetFileExtension() const
	{
		return pathExtention;
	}

protected:
	FileMode currentFileMode = FileMode::ReadOnly;
	std::string path = "";
	std::string pathExtention = "";
	std::string name = "";
};