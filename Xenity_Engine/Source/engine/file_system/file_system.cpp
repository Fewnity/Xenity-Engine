#include "file_system.h"
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "../../../include/stb_image.h"

#include "../engine_settings.h"
#include "../debug/debug.h"
#include <filesystem>

#if defined(__vita__)
#include <psp2/io/stat.h>
#endif

#define TEXTURE_PATH R"(images\)"
#define SHADER_PATH R"(shaders\)"
#define MODEL_PATH R"(models\)"

#define PSVITA_PATH R"(ux0:data\xenity_engine\)"

FileSystem* FileSystem::fileSystem = nullptr;

#pragma region File

File::File(std::string path)
{
#if defined(__vita__)
	path = PSVITA_PATH + path;
#endif

	this->path = path;
}

void File::Write(const std::string data)
{
#if defined(__PSP__)
	fileId = sceIoOpen(path.c_str(), PSP_O_RDWR, 0777);
	if (fileId >= 0)
	{
		sceIoLseek(fileId, 0, SEEK_END);
		int b = sceIoWrite(fileId, data.c_str(), data.size());
		sceIoClose(fileId);
	}
#else
	if (file.is_open())
	{
		file.seekg(0, std::ios_base::end);
		file << data;
		file.flush();
	}
#endif
}

std::string File::ReadAll()
{
	std::string allText = "";
#if defined(__PSP__)
	int pos = sceIoLseek(fileId, 0, SEEK_END);
	sceIoLseek(fileId, 0, SEEK_SET);
	char* data = new char[pos];
	sceIoRead(fileId, data, pos);
	allText = data;
	delete[] data;
#else
	file.seekg(0, std::ios_base::beg);
	std::string tempText;
	while (getline(file, tempText))
	{
		allText += tempText;
	}
#endif
	return allText;
}

bool File::Open(bool createFileIfNotFound)
{
	bool isOpen = false;
#if defined(__PSP__)
	int params = PSP_O_RDWR;
	if (createFileIfNotFound)
		params = params | PSP_O_CREAT;
	fileId = sceIoOpen(path.c_str(), params, 0777);
	if (fileId >= 0)
	{
		isOpen = true;
	}
#else
	std::ios_base::openmode params = std::fstream::in | std::fstream::out;

	file.open(path, params);

	if (!file.is_open())
	{
		if (createFileIfNotFound)
		{
			params = params | std::fstream::trunc;
			file.open(path, params);
			if (!file.is_open())
			{
				Debug::PrintError("Fail while opening and creating file (file.is_open())");
			}
			else
			{
				isOpen = true;
			}
		}
		else
		{
			Debug::PrintError("Fail while opening file (file.is_open())");
		}
	}
	else
	{
		isOpen = true;
	}
#endif
	return isOpen;
}

void File::Close()
{
#if defined(__PSP__)
	sceIoClose(fileId);
#else
	file.close();
#endif
}

#pragma endregion

#pragma region Directory

Directory::Directory(std::string path)
{
	this->path = path;
	FileSystem::fileSystem->FillDirectory(this);
}

Directory::~Directory()
{
	int subDirCount = (int)subdirectories.size();
	for (int i = 0; i < subDirCount; i++)
	{
		delete subdirectories[i];
	}
	subdirectories.clear();
	int fileount = (int)files.size();
	for (int i = 0; i < fileount; i++)
	{
		delete files[i];
	}
	files.clear();
}

void FileSystem::FillDirectory(Directory* directory)
{
	for (const auto& file : std::filesystem::directory_iterator(directory->path))
	{
		if (file.is_directory())
		{
			Directory* newDirectory = nullptr;
			try
			{
				newDirectory = new Directory(file.path().string());
				directory->subdirectories.push_back(newDirectory);
			}
			catch (const std::exception&)
			{
				if (newDirectory != nullptr)
					delete newDirectory;
			}
		}
		else if (file.is_regular_file())
		{
			File* newFile = nullptr;
			try
			{
				std::string p = file.path().string();
				p = file.path().string();
				newFile = new File(p);
				directory->files.push_back(newFile);
			}
			catch (const std::exception&)
			{
				if (newFile != nullptr)
					delete newFile;
			}
		}
	}
}

#pragma endregion

#pragma region Read/Input

#pragma endregion

void FileSystem::DeleteFile(const std::string path)
{
#if defined(__PSP__)
	sceIoRemove(path.c_str());
#else
	remove(path.c_str());
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
	// EngineSettings::RootFolder = "";

	texturePath = "";
	texturePath += TEXTURE_PATH;

	shaderPath = "";
	shaderPath += SHADER_PATH;

	modelsPath = "";
	modelsPath += MODEL_PATH;

#if defined(__vita__)
	sceIoMkdir("ux0:/data/xenity_engine", 0777);
#endif

	// Debug::Print("-------- File System initiated --------");
	//  gamePath += R"(Debug\)"; //TODO remove this
}

std::string FileSystem::GetGamePath()
{
	return EngineSettings::RootFolder;
}