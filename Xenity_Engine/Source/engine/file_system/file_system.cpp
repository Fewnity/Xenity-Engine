#include "file_system.h"
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "../../../include/stb_image.h"

#include "../engine_settings.h"
#include "../debug/debug.h"
#include <filesystem>
#if defined(__PSP__)
#include <dirent.h>
#include <sys/stat.h>
#endif
#if defined(__vita__)
#include <psp2/io/stat.h>
#endif

#define TEXTURE_PATH R"(images\)"
#define SHADER_PATH R"(shaders\)"
#define MODEL_PATH R"(models\)"

#define PSVITA_PATH R"(ux0:data\xenity_engine\)"
#define PSVITA_BASE_DIR "ux0:"

FileSystem* FileSystem::fileSystem = nullptr;

#pragma region File

File::File(std::string path) : UniqueId(true)
{
#if defined(__vita__)
	// path = PSVITA_PATH + path;
	path = PSVITA_BASE_DIR + path;
#endif

	this->path = path;

	//std::cout << path << std::endl;
	int pointIndex = path.find_last_of(".");
	pathExtention = path.substr(pointIndex);
	//std::cout << pathExtention << std::endl;
}

File::~File()
{
#if defined(__PSP__)
	sceIoClose(fileId);
#else
	Close();
#endif
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
	fileId = sceIoOpen(path.c_str(), PSP_O_RDWR, 0777);
	if (fileId >= 0)
	{
		int pos = sceIoLseek(fileId, 0, SEEK_END);
		sceIoLseek(fileId, 0, SEEK_SET);
		char *data = new char[pos + 1];
		data[pos] = 0;
		sceIoRead(fileId, data, pos);
		allText = data;
		delete[] data;

		sceIoClose(fileId);
		fileId = -1;
	}
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

bool File::CheckIfExist()
{
	bool exists = false;
#if defined(__PSP__)
	int params = PSP_O_RDONLY;
	fileId = sceIoOpen(path.c_str(), params, 0777);
	if (fileId >= 0)
	{
		exists = true;
	}
	sceIoClose(fileId);
#else
	std::ios_base::openmode params = std::fstream::in | std::fstream::out;
	file.open(path, params);

	if (file.is_open())
	{
		exists = true;
	}
	file.close();
#endif
	return exists;
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
	if (file.is_open())
	{
		file.close();
	}
#endif
}

std::string File::GetFolderPath() const
{
	if (path.size() == 0)
		return "";

	int lastSlashPos = path.find_last_of('\\');
	if (lastSlashPos == -1)
		lastSlashPos = 0;
#if defined(__vita__)
	std::string fileName = path.substr(4, lastSlashPos + 1);
#else
	std::string fileName = path.substr(0, lastSlashPos + 1);
#endif

	return fileName;
}

std::string File::GetFileName() const
{
	if (path.size() == 0)
		return "";

	int lastSlashPos = path.find_last_of('\\');
	if (lastSlashPos == -1)
		lastSlashPos = 0;
	std::string fileName = path.substr(lastSlashPos + 1);
	int nextPointPos = fileName.find_first_of('.');
	if (nextPointPos == -1)
		nextPointPos = INT32_MAX;
	fileName = fileName.substr(0, nextPointPos);
	return fileName;
}

#pragma endregion

#pragma region Directory

Directory::Directory(std::string path) : UniqueId(true)
{
#if defined(__vita__)
	path = PSVITA_BASE_DIR + path;
#endif
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

void AddDirectoryFiles(std::vector<File*>& vector, Directory* directory)
{
	int fileCount = (int)directory->files.size();
	for (int i = 0; i < fileCount; i++)
	{
		vector.push_back(directory->files[i]);
	}

	int directoryCount = (int)directory->subdirectories.size();
	for (int i = 0; i < directoryCount; i++)
	{
		AddDirectoryFiles(vector, directory->subdirectories[i]);
	}
}

std::vector<File*> Directory::GetAllFiles()
{
	std::vector<File*> vector;
	AddDirectoryFiles(vector, this);
	return vector;
}

void FileSystem::FillDirectory(Directory* directory)
{
#if defined(__PSP__)
	DIR *dir = opendir(directory->GetPath().c_str());
	if (dir == NULL)
	{
		//Debug::PrintError("Impossible d'ouvrir le dossier");
		return;
	}
	struct dirent *ent;
	while ((ent = readdir(dir)) != NULL)
	{
		std::string found = "";
		found += ent->d_name;
		if (found == "." || found == "..")
			continue;

		std::string fullPath = directory->GetPath() + found;
		struct stat statbuf;
		if (stat(fullPath.c_str(), &statbuf) == -1)
		{
			// Debug::PrintError("Erreur lors de l'appel à stat");
			continue;
		}

		if (S_ISREG(statbuf.st_mode))
		{
			File *newFile = nullptr;
			try
			{
				newFile = new File(fullPath);
				directory->files.push_back(newFile);
			}
			catch (const std::exception &)
			{
				if (newFile != nullptr)
					delete newFile;
			}
		}
		else if (S_ISDIR(statbuf.st_mode))
		{
			Directory *newDirectory = nullptr;
			try
			{
				newDirectory = new Directory(fullPath + "\\");
				directory->subdirectories.push_back(newDirectory);
			}
			catch (const std::exception &)
			{
				if (newDirectory != nullptr)
					delete newDirectory;
			}
		}

		// printf("%s\n", ent->d_name);
	}
	closedir(dir);
#else
	std::string dirPath = directory->GetPath();
#if defined(__vita__)
	dirPath = PSVITA_BASE_DIR + dirPath;
#endif
	for (const auto &file : std::filesystem::directory_iterator(dirPath))
	{

		if (file.is_directory())
		{
			Directory *newDirectory = nullptr;
			try
			{
			std::string path = file.path().string();
#if defined(__vita__)
			path = path.substr(4);
#endif
				newDirectory = new Directory(path + "\\");
				directory->subdirectories.push_back(newDirectory);
			}
			catch (const std::exception &)
			{
				if (newDirectory != nullptr)
					delete newDirectory;
			}
		}
		else if (file.is_regular_file())
		{
			File *newFile = nullptr;
			try
			{
			std::string path = file.path().string();
#if defined(__vita__)
			path = path.substr(4);
#endif
				std::string p = path;
				newFile = new File(p);
				directory->files.push_back(newFile);
			}
			catch (const std::exception &)
			{
				if (newFile != nullptr)
					delete newFile;
			}
		}
	}
#endif
}

#pragma endregion

#pragma region Read/Input

#pragma endregion

void FileSystem::DeleteFile(const std::string path)
{
	std::string newPath = path;
#if defined(__vita__)
	newPath = PSVITA_BASE_DIR + path;
#endif
#if defined(__PSP__)
	sceIoRemove(newPath.c_str());
#else
	remove(newPath.c_str());
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