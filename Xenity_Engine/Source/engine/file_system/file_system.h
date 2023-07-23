#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#ifdef __PSP__
#include <pspkernel.h>
#endif

#include "../unique_id/unique_id.h"
#include <string>
#include <fstream>
#include <vector>

class Directory;
class File;

class API FileSystem
{
public:
	FileSystem()
	{
		this->fileSystem = this;
	}
	static FileSystem *fileSystem;

	void InitFileSystem(const std::string exePath);
	std::string GetGamePath();

	void DeleteFile(const std::string path);
	void FillDirectory(Directory *directory);

	std::string texturePath = "";
	std::string shaderPath = "";
	std::string modelsPath = "";
};

class API File : public UniqueId
{
public:
	File() = delete;
	File(std::string path);
	~File();

	void Write(const std::string data);
	std::string ReadAll();
	// Return isOpen
	bool Open(bool createFileIfNotFound);
	void Close();

	std::string GetPath() const
	{
		return path;
	}

private:
	std::string path = "";
#if defined(__PSP__)
	SceUID fileId;
#else
	std::fstream file;
#endif
};

class API Directory : public UniqueId
{
public:
	Directory() = delete;
	Directory(std::string path);
	~Directory();
	std::string path = "";
	std::vector<Directory *> subdirectories;
	std::vector<File *> files;
};