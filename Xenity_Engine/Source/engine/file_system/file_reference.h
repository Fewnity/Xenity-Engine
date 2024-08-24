// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */
#include <memory>

#include <engine/api.h>
#include <engine/reflection/reflection.h>
#include <engine/platform.h>
#include "file_type.h"

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
	virtual ReflectiveData GetMetaReflectiveData(AssetPlatform platform)
	{
		return ReflectiveData();
	}

	uint64_t m_fileId = -1;

	uint64_t m_filePosition = 0;
	uint64_t m_fileSize = 0;
	uint64_t m_metaPosition = 0;
	uint64_t m_metaSize = 0;

	std::shared_ptr<File> m_file = nullptr;
	FileType m_fileType = FileType::File_Other;
	bool m_isMetaDirty = false;

	void SetIsLoading(bool _isLoading)
	{
		m_isLoading = _isLoading;
	}

	bool IsLoading() const
	{
		return m_isLoading;
	}

	bool IsLoaded() const
	{
		return m_isLoaded;
	}

protected:
	bool m_isLoaded = false;
	bool m_isLoading = false;
};

