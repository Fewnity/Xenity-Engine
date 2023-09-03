#pragma once

#include "../../engine/reflection/reflection.h"
#include <memory>

class File;

enum FileType
{
	File_Other,
	File_Audio,
	File_Mesh,
	File_Texture,
	File_Scene,
	File_Code,
};

class FileReference : public std::enable_shared_from_this<FileReference>
{
public:
	FileReference(){}
	/**
	* Load the file
	*/
	virtual void LoadFileReference() 
	{
		isLoaded = true;
		//AssetManager::AddFileReference(this);
	}

	/**
	* Unload the file
	*/
	virtual void UnloadFileReference()
	{
		isLoaded = false;
		//AssetManager::RemoveFileReference(this);
	}

	/**
	* Get reflection of the file meta
	*/
	virtual std::unordered_map<std::string, Variable> GetMetaReflection() {
		return std::unordered_map<std::string, Variable>();
	}

	uint64_t fileId = -1;
	File* file = nullptr;
	FileType fileType = File_Other;
	bool isLoaded = false;
};

