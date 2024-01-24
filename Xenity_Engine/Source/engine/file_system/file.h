#pragma once
#include <engine/api.h>

#if defined(__PSP__)
	#include <pspkernel.h>
#endif

#include <engine/unique_id/unique_id.h>
#include <vector>
#include <memory>
#include <string>
#include <fstream>

enum class FileMode 
{
	ReadOnly,
	WriteOnly,
	WriteCreateFile,
};

class API File : public UniqueId, public std::enable_shared_from_this<File>
{
public:
	File() = delete;
	File(std::string _path);
	~File();

	/**
	* Write data into the file
	*/
	virtual void Write(const std::string& data) {};

	/**
	* Read all the file
	*/
	virtual std::string ReadAll() { return ""; };

	/**
	* Read all the file in binary mode (Need to free the pointer after)
	* @param sizeOut return read data size
	*/
	virtual unsigned char* ReadAllBinary(int& sizeOut) { return nullptr; };

	/**
	* Check if the file exists
	*/
	virtual bool CheckIfExist() { return false; };

	/**
	* Open the file
	* @param createFileIfNotFound If true, create the file if not found
	*/
	virtual bool Open(FileMode fileMode) { return false; };

	/**
	* Close file
	*/
	virtual void Close() {};

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

protected:
	FileMode currentFileMode = FileMode::ReadOnly;
	std::string path = "";
	std::string pathExtention = "";
	std::string name = "";
};