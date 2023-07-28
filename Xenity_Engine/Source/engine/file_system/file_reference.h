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
};

class FileReference
{
public:
	int fileId = 0;
	File* file = nullptr;
	FileType fileType = File_Other;
	virtual void LoadFileReference() 
	{
		isLoaded = true;
	}
	virtual void UnloadFileReference()
	{
		isLoaded = false;
	}
	virtual std::unordered_map<std::string, Variable> GetMetaReflection() = 0;
	bool isLoaded = false;
};

