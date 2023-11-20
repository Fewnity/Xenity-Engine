#pragma once

#include "../../engine/reflection/reflection.h"
#include <memory>

#include "file_type.h"

class File;

class FileReference : public std::enable_shared_from_this<FileReference>
{
public:
	FileReference() = default;

	/**
	* Load the file
	*/
	virtual void LoadFileReference()
	{
		//isLoaded = true;
		//AssetManager::AddFileReference(this);
	}

	/**
	* Unload the file
	*/
	virtual void UnloadFileReference()
	{
		//isLoaded = false;
		//AssetManager::RemoveFileReference(this);
	}

	virtual void OnLoadFileReferenceFinished()
	{

	}

	void OnReflectionUpdated() 
	{

	}

	/**
	* Get reflection of the file meta
	*/
	virtual ReflectiveData GetMetaReflection()
	{
		return ReflectiveData();
	}

	uint64_t fileId = -1;
	std::shared_ptr<File> file = nullptr;
	FileType fileType = File_Other;
	bool isLoaded = false;
	bool isLoading = false;
};

