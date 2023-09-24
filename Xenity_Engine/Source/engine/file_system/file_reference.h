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
	File_Skybox,
	File_Font,
	File_Material,
	File_Shader,
};

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

	/**
	* Get reflection of the file meta
	*/
	virtual std::unordered_map<std::string, ReflectionEntry> GetMetaReflection()
	{
		return std::unordered_map<std::string, ReflectionEntry>();
	}

	uint64_t fileId = -1;
	std::shared_ptr<File> file = nullptr;
	FileType fileType = File_Other;
	bool isLoaded = false;
};

