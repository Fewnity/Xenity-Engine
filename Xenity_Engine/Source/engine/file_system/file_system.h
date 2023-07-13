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

#include <string>

class API File
{
public:
	File() = delete;
	File(const std::string path);

	void Write(const std::string data);
	void Close();

	std::string GetPath() const
	{
		return path;
	}

private:
	std::string path;

#ifdef __PSP__
	SceUID fileId;
#endif
};

class API FileSystem
{
public:
	FileSystem()
	{
		this->fileSystem = this;
	}
	static FileSystem *fileSystem;

	unsigned char *LoadTextureData(const std::string filePath, int &width, int &height, int &nrChannels);
	std::string ReadText(const std::string path);
	void InitFileSystem(const std::string exePath);
	std::string GetGamePath();

	File *OpenFile(const std::string path);
	void CloseFile(File *file);
	void WriteInFile(File *file, const std::string data);
	void DeleteFile(const std::string path);

	std::string texturePath = "";
	std::string shaderPath = "";
	std::string modelsPath = "";
};
