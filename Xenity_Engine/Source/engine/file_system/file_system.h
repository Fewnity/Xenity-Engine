#pragma once

#ifdef __PSP__
#include <pspkernel.h>
#endif

#include <string>

class File
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

class FileSystem
{
public:
	static unsigned char *LoadTextureData(const std::string filePath, int &width, int &height, int &nrChannels);
	static std::string ReadText(const std::string path);
	static void InitFileSystem(const std::string exePath);
	static std::string GetGamePath();

	static File *OpenFile(const std::string path);
	static void CloseFile(File *file);
	static void WriteInFile(File *file, const std::string data);
	static void DeleteFile(const std::string path);

	static std::string texturePath;
	static std::string shaderPath;
	static std::string modelsPath;
};
