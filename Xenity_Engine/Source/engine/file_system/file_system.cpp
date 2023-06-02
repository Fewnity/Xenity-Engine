#include "file_system.h"
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "../../../include/stb_image.h"

#include <iostream>
#include <fstream>

#include "../engine_settings.h"
#include "../debug/debug.h"

// #define TEXTURE_PATH R"(Xenity_Engine\Source\images\)"
// #define SHADER_PATH R"(Xenity_Engine\Source\images\)"
// #define MODEL_PATH R"(Xenity_Engine\Source\images\)"

#define TEXTURE_PATH R"(images\)"
#define SHADER_PATH R"(shaders\)"
#define MODEL_PATH R"(models\)"

std::string FileSystem::texturePath;
std::string FileSystem::shaderPath;
std::string FileSystem::modelsPath;

#pragma region File

File::File(const std::string path)
{
	this->path = path;
#ifdef __PSP__
	fileId = sceIoOpen(path.c_str(), PSP_O_RDWR | PSP_O_CREAT, 0777);
#endif
}

void File::Write(const std::string data)
{
#ifdef __PSP__
	fileId = sceIoOpen(path.c_str(), PSP_O_RDWR | PSP_O_CREAT, 0777);
	sceIoLseek(fileId, 0, SEEK_END);
	int b = sceIoWrite(fileId, data.c_str(), data.size());
	sceIoClose(fileId);
#endif
}

void File::Close()
{
#ifdef __PSP__
	sceIoClose(fileId);
#endif
}

#pragma endregion

#pragma region Read/Input

unsigned char *FileSystem::LoadTextureData(const std::string filePath, int &width, int &height, int &nrChannels)
{
	unsigned char *data = stbi_load((texturePath + filePath).c_str(), &width, &height, &nrChannels, 0);
	return data;
}

std::string FileSystem::ReadText(const std::string path)
{
	// Open file
	std::ifstream file;
	// file.open(finalpath + path);
	file.open(path);

	// Print error if the file can't be read
	if (file.fail())
	{
		std::cout << "\033[31mShader read error. Path : \"" << path << "\"\033[0m" << std::endl;
	}

	// Read file
	std::string text = "", line;
	while (getline(file, line))
	{
		text += line + '\n';
	}

	// Close the file
	file.close();

	return text;
}

#pragma endregion

File *FileSystem::OpenFile(const std::string path)
{
	return new File(path);
}

void FileSystem::CloseFile(File *file)
{
	file->Close();
}

void FileSystem::WriteInFile(File *file, const std::string data)
{
	file->Write(data);
}

void FileSystem::DeleteFile(const std::string path)
{
#ifdef __PSP__
	sceIoRemove(path.c_str());
#endif
}

#pragma region Write/Output

#pragma endregion

void FileSystem::InitFileSystem(const std::string exePath)
{
	// EngineSettings::RootFolder = R"(C:\Users\gregory.machefer\Documents\GitHub\Xenity-Engine\)";
	// EngineSettings::RootFolder = R"(C:\Users\elect\Documents\GitHub\Xenity-Engine\)";

	// EngineSettings::RootFolder = exePath;
	// int index = (int)exePath.find(R"(\Xenity-Engine\)");
	// EngineSettings::RootFolder = exePath.substr(0, index + 15);
	EngineSettings::RootFolder = "";

	texturePath = EngineSettings::RootFolder;
	texturePath += TEXTURE_PATH;

	shaderPath = EngineSettings::RootFolder;
	shaderPath += SHADER_PATH;

	modelsPath = EngineSettings::RootFolder;
	modelsPath += MODEL_PATH;

	Debug::Print("-------- File System initiated --------");
	// gamePath += R"(Debug\)"; //TODO remove this
}

std::string FileSystem::GetGamePath()
{
	return EngineSettings::RootFolder;
}