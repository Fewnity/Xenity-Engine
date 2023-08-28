#pragma once

#include "../../engine/reflection/reflection.h"

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

class FileReference
{
public:

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
	virtual std::unordered_map<std::string, Variable> GetMetaReflection() = 0;

	uint64_t fileId = 0;
	File* file = nullptr;
	FileType fileType = File_Other;
	bool isLoaded = false;
};

