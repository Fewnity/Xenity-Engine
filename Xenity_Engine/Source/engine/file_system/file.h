#pragma once

#include <string>

class File
{
public:
	static unsigned char *LoadTextureData(const std::string filePath, int &width, int &height, int &nrChannels);
	static std::string ReadText(const std::string path);
	static void InitFileSystem(const std::string exePath);
	static std::string GetGamePath();

	static std::string texturePath;
	static std::string shaderPath;
	static std::string modelsPath;
};
