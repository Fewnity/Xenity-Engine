// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/reflection/reflection.h>
#include <engine/tools/template_utils.h>
#include <string>
#include <vector>

class FileReference;

class FileReferenceFinder
{
public:

	/**
	* @brief Add the file id to the vector
	* @param valuePtr Reference to the file reference
	* @param ids Vector to store the file ids
	*/
	template<typename T>
	std::enable_if_t<std::is_base_of<FileReference, T>::value, bool>
	static GetFileRefId(const std::reference_wrapper<std::shared_ptr<T>>* valuePtr, std::vector<uint64_t>& ids);

	/**
	* @brief Get all files ids from a reflective data, get all files id stored in variables
	* @param usedFilesIds Vector to store the file ids
	* @param reflectiveData Reflective data to get the files ids
	*/
	static void GetUsedFilesInReflectiveData(std::vector<uint64_t>& usedFilesIds, const ReflectiveData& reflectiveData);

private:

	/**
	* @brief Function for non file reference types, return false
	*/
	template<typename T>
	static bool GetFileRefId(const T& var, std::vector <uint64_t>& ids);

	/**
	* @brief Get all files ids from a vector of file references
	* @param valuePtr Reference to the vector of file references
	* @param ids Vector to store the file ids
	*/
	template<typename T>
	std::enable_if_t<std::is_base_of<FileReference, T>::value, bool>
	static GetFileRefId(const std::reference_wrapper<std::vector<std::shared_ptr<T>>>* valuePtr, std::vector <uint64_t>& ids);
};

