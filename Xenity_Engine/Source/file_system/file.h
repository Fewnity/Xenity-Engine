#pragma once

#include <string>

class File
{
public:
	static unsigned char* LoadTextureData(const std::string filePath, int& width, int& height, int& nrChannels);
	static std::string LoadShaderData(const std::string path);
	static void InitFileSystem();
	static std::string GetGamePath();
};

