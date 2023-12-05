#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#if defined(__PSP__)
#include <pspkernel.h>
#endif

#include <engine/unique_id/unique_id.h>
#include <string>
#include <fstream>
#include <vector>
#include <memory>

#if defined(__vita__)
#define PSVITA_PATH R"(ux0:data\xenity_engine\)"
#define PSVITA_BASE_DIR "ux0:"
#endif

class Directory;
class File;

class API FileSystem
{
public:
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
	void FillDirectory(std::shared_ptr <Directory> directory, bool recursive);

	bool Rename(const std::string& path, const std::string& newPath);

	int CopyFile(const std::string& path, const std::string& newPath, bool replace);

	static std::shared_ptr<File> MakeFile(const std::string& path);
};