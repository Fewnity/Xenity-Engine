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

	int InitFileSystem();
	std::string GetGamePath();

	void CreateDirectory(std::string path);
	void DeleteFile(const std::string path);
	void FillDirectory(Directory *directory);
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
	bool CheckIfExist();
	bool Open(bool createFileIfNotFound);
	void Close();

	std::string GetPath() const
	{
#if defined(__vita__)
		return path.substr(4);
#else
		return path;
#endif
	}

	std::string GetFolderPath() const;

	std::string GetFileName() const;

	std::string GetFileExtention() const
	{
		return pathExtention;
	}

private:
	std::string path = "";
	std::string pathExtention = "";
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
	std::vector<File *> GetAllFiles();
	std::vector<File *> GetAllDirectory();
	std::vector<Directory *> subdirectories;
	std::vector<File *> files;

	std::string GetPath() const
	{
#if defined(__vita__)
		return path.substr(4);
#else
		return path;
#endif
	}

private:
	std::string path = "";
};