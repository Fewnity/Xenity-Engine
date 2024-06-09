// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include <engine/unique_id/unique_id.h>

#include <vector>
#include <memory>
#include <string>

class File;

class API Directory : public UniqueId, public std::enable_shared_from_this<Directory>
{
public:
	Directory() = delete;
	explicit Directory(std::string _path);
	~Directory();

	/**
	* @brief Get all the files of the directory (can be very slow)
	* @param recursive If true, get all the files of the subdirectories
	*/
	std::vector<std::shared_ptr<File>> GetAllFiles(bool recursive);

	/**
	* @brief Check if the directory exists
	*/
	bool CheckIfExist() const;

	std::vector<std::shared_ptr<Directory>> subdirectories;
	std::vector<std::shared_ptr<File>> files;

	/**
	* @brief Get directory path
	*/
	inline const std::string& GetPath() const
	{
		return path;
	}

private:
	std::string path = "";
};