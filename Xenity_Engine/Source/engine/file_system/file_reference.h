#pragma once

/**
 * [Internal]
 */

#include "file_type.h"

#include <engine/reflection/reflection.h>

#include <memory>

class File;

class FileReference : public std::enable_shared_from_this<FileReference>
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
	bool isLoaded = false;
	bool isLoading = false;
};

