#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <engine/unique_id/unique_id.h>

#include <vector>
#include <memory>
#include <string>

class File;

class API Directory : public UniqueId, public std::enable_shared_from_this<Directory>
{
public:
	Directory() = delete;
	Directory(std::string _path);
	~Directory();

	/**
	* Get all the files of the directory (can be very slow)
	*/
	std::vector<std::shared_ptr<File>> GetAllFiles(bool recursive);

	/**
	* Check if the directory exists
	*/
	bool CheckIfExist();
	std::vector<std::shared_ptr<Directory>> subdirectories;
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