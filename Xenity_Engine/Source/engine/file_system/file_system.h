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
#include <memory>

class Directory;
class File;

class API FileSystem
{
public:
	FileSystem()
	{
		this->fileSystem = this;
	}
	static FileSystem* fileSystem;

	/**
	* Init file system
	*/
	int InitFileSystem();

	/**
	* Get game path
	*/
	std::string GetGamePath();

	/**
	* Create a directory
	* @param path Directory path
	*/
	void CreateDirectory(const std::string& path);

	/**
	* Delete a file
	* @param path File path
	*/
	void DeleteFile(const std::string& path);

	/**
	* Get all files of a directory and fill it
	* @param directory Directory to fill
	*/
	void FillDirectory(Directory* directory, bool recursive);

	bool Rename(const std::string& path, const std::string& newPath);

	static std::shared_ptr<File> MakeFile(const std::string& path);
};

class API File : public UniqueId
{
public:
	File() = delete;
	File(std::string _path);
	~File();

	/**
	* Write data into the file
	*/
	void Write(const std::string& data);

	/**
	* Read all the file
	*/
	std::string ReadAll();

	/**
	* Read all the file in binary mode (Need to free the pointer after
	*/
	unsigned char* ReadAllBinary(int& size);

	/**
	* Check if the file exists
	*/
	bool CheckIfExist();

	/**
	* Open the file
	* @param createFileIfNotFound If true, create the file if not found
	*/
	bool Open(bool createFileIfNotFound);

	/**
	* Close file
	*/
	void Close();

	/**
	* Get file path
	*/
	std::string GetPath() const
	{
		/*#if defined(__vita__)
				return path.substr(4);
		#else
				return path;
		#endif*/
		return path;
	}

	/**
	* Get file's folder path
	*/
	std::string GetFolderPath() const;

	/**
	* Get file name
	*/
	std::string GetFileName() const;

	/**
	* Get file extension (dot included)
	*/
	std::string GetFileExtension() const
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
	Directory(std::string _path);
	~Directory();

	/**
	* Get all the files of the directory !!! (All files need to be deleted with delete()) !!!
	*/
	std::vector<std::shared_ptr<File>> GetAllFiles(bool recursive);

	/**
	* Check if the directory exists
	*/
	bool CheckIfExist();
	std::vector<Directory*> subdirectories;
	std::vector<std::shared_ptr<File>> files;

	/**
	* Get directory path
	*/
	std::string GetPath() const
	{
		return path;
		/*#if defined(__vita__)
				return path.substr(4);
		#else
				return path;
		#endif*/
	}

private:
	std::string path = "";
};