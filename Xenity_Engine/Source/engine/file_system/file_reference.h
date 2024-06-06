// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

#include "file_type.h"
#include <engine/api.h>
#include <engine/reflection/reflection.h>

#include <memory>

class File;

class API FileReference : public std::enable_shared_from_this<FileReference>
{
public:
	FileReference() = default;

	/**
	* @brief Load the file
	*/
	virtual void LoadFileReference()
	{
		//isLoaded = true;
		//AssetManager::AddFileReference(this);
	}

	/**
	* @brief Unload the file
	*/
	virtual void UnloadFileReference()
	{
		//isLoaded = false;
		//AssetManager::RemoveFileReference(this);
	}

	virtual void OnLoadFileReferenceFinished()
	{

	}

	virtual void OnReflectionUpdated()
	{

	}

	virtual ReflectiveData GetReflectiveData()
	{
		return ReflectiveData();
	}

	/**
	* @brief Get reflection of the file meta
	*/
	virtual ReflectiveData GetMetaReflectiveData()
	{
		return ReflectiveData();
	}

	uint64_t fileId = -1;

	std::shared_ptr<File> file = nullptr;
	FileType fileType = FileType::File_Other;
	bool isMetaDirty = false;

	void SetIsLoading(bool _isLoading)
	{
		isLoading = _isLoading;
	}

	bool GetIsLoading() const
	{
		return isLoading;
	}

	bool GetIsLoaded() const
	{
		return isLoaded;
	}

protected:
	bool isLoaded = false;
	bool isLoading = false;
};

