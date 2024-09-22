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

enum class FileStatus
{
	FileStatus_Not_Loaded,
	FileStatus_Loading,
	FileStatus_Loaded,
	FileStatus_Failed
};

class API FileReference : public std::enable_shared_from_this<FileReference>
{
public:
	FileReference() = default;

	/**
	* @brief Load the file
	*/
	virtual void LoadFileReference() { }

	/**
	* @brief Unload the file
	*/
	virtual void UnloadFileReference() { }

	virtual void OnLoadFileReferenceFinished() { }

	virtual void OnReflectionUpdated() { }

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

	void SetFileStatus(FileStatus _fileStatus)
	{
		m_fileStatus = _fileStatus;
	}

	FileStatus GetFileStatus() const
	{
		return m_fileStatus;
	}

protected:
	FileStatus m_fileStatus = FileStatus::FileStatus_Not_Loaded;
};

