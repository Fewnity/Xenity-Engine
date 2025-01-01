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

/**
* @brief Class implemented by asset classes (Texture, Audio Clip...)
* @brief Used to implement Load/Unload function and to store file infos (type, size...)
*/
class API FileReference : public Reflective, public std::enable_shared_from_this<FileReference>
{
public:
	FileReference() = default;
	FileReference(const FileReference& other) = delete;
	FileReference& operator=(const FileReference&) = delete;

	/**
	* @brief Load the file
	*/
	virtual void LoadFileReference() { }

	/**
	* @brief Unload the file
	*/
	virtual void UnloadFileReference() { }

	/**
	* @brief event function called when a file has finshed to load
	*/
	virtual void OnLoadFileReferenceFinished() { }

	/**
	* @brief Get reflection of the file meta
	*/
	virtual ReflectiveData GetMetaReflectiveData([[maybe_unused]] AssetPlatform platform)
	{
		return ReflectiveData();
	}

	/**
	* @brief Set file status
	*/
	void SetFileStatus(FileStatus _fileStatus)
	{
		m_fileStatus = _fileStatus;
	}

	/**
	* @brief Get file status
	*/
	FileStatus GetFileStatus() const
	{
		return m_fileStatus;
	}

	/**
	* @brief Get file type
	*/
	FileType GetFileType() const
	{
		return m_fileType;
	}

	uint64_t GetFileId() const
	{
		return m_fileId;
	}

protected:
	friend class ProjectManager;
	friend class InspectorMenu;
	friend class EngineAssetManagerMenu;
	friend class EditorUI;
	friend class AssetModifier;
	friend class Compiler;
	friend class ProfilerMenu;
	template<class T>
	friend class SelectAssetMenu;
	friend class SceneMenu;
	friend class FileExplorerMenu;
	friend class Window;
	friend class SceneManager;
	friend class EngineDebugMenu;

	std::shared_ptr<File> m_file = nullptr;
	uint64_t m_filePosition = 0;
	uint64_t m_fileSize = 0;
	uint64_t m_metaPosition = 0;
	uint64_t m_metaSize = 0;

	uint64_t m_fileId = -1;
	FileType m_fileType = FileType::File_Other;
	FileStatus m_fileStatus = FileStatus::FileStatus_Not_Loaded;

	bool m_isMetaDirty = false;
};

