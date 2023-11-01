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
#elif defined(__vita__)
#include <psp2/io/stat.h>
#define PSVITA_PATH R"(ux0:data\xenity_engine\)"
#define PSVITA_BASE_DIR "ux0:"
#endif

FileSystem* FileSystem::fileSystem = nullptr;

#pragma region File

File::File(std::string _path) : UniqueId(true)
{
#if defined(__vita__)
	_path = PSVITA_BASE_DIR + _path;
#endif
	this->path = _path;
	int pointIndex = path.find_last_of('.');
	pathExtention = path.substr(pointIndex);



	// Remove all folders from path
	int finalPos = 0;
	int lastSlashPos = path.find_last_of('\\');
	int lastSlashPos2 = path.find_last_of('/');

	if (lastSlashPos != -1 || lastSlashPos2 != -1)
	{
		if (lastSlashPos2 > lastSlashPos)
			finalPos = lastSlashPos2 + 1;
		else
			finalPos = lastSlashPos + 1;
	}

	std::string fileName = path.substr(finalPos);

	// Remove file extension from path
	int nextPointPos = fileName.find_first_of('.');
	if (nextPointPos == -1)
		nextPointPos = INT32_MAX;
	name = fileName.substr(0, nextPointPos);

	//return fileName;
}

File::~File()
{
#if defined(__PSP__)
	if (fileId >= 0)
	{
		sceIoClose(fileId);
	}
#else
	Close();
#endif
}

void File::Write(const std::string& data)
{
#if defined(__PSP__)
	fileId = sceIoOpen(path.c_str(), PSP_O_RDWR, 0777);
	if (fileId >= 0)
	{
		sceIoLseek(fileId, 0, SEEK_END);
		int b = sceIoWrite(fileId, data.c_str(), data.size());
		sceIoClose(fileId);
		fileId = -1;
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

unsigned char* File::ReadAllBinary(int& size)
{
	char* data = nullptr;
#if defined(__PSP__)
	fileId = sceIoOpen(path.c_str(), PSP_O_RDONLY, 0);
	if (fileId >= 0)
	{
		SceIoStat file_stat;
		sceIoGetstat(path.c_str(), &file_stat);
		sceIoLseek(fileId, 0, SEEK_SET);
		data = new char[file_stat.st_size + 1];
		sceIoRead(fileId, data, file_stat.st_size);
		size = file_stat.st_size;
	}
#else

	file.seekg(0, std::ios_base::end);
	int pos = file.tellg();
	file.seekg(0, std::ios_base::beg);
	data = new char[pos];
	file.read(data, pos);
	size = pos;
#endif
	return (unsigned char*)data;
}

std::string File::ReadAll()
{
	std::string allText = "";
#if defined(__PSP__)
	fileId = sceIoOpen(path.c_str(), PSP_O_RDONLY, 0);
	if (fileId >= 0)
	{
		int pos = sceIoLseek(fileId, 0, SEEK_END);
		sceIoLseek(fileId, 0, SEEK_SET);
		char* data = new char[pos + 1];
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
		allText += "\n";
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
		sceIoClose(fileId);
		fileId = -1;
	}
#else
	if (file.is_open())
	{
		exists = true;
	}
	else
	{
		std::ios_base::openmode params = std::fstream::in | std::fstream::out;
		file.open(path, params);

		if (file.is_open())
		{
			exists = true;
		}
		file.close();
	}
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
		sceIoClose(fileId);
		fileId = -1;
	}
#else
	std::ios_base::openmode params = std::fstream::binary | std::fstream::in | std::fstream::out;

	file.open(path, params);

	if (!file.is_open())
	{
		if (createFileIfNotFound)
		{
			// Try to create the file
			params = params | std::fstream::trunc;
			file.open(path, params);
			if (!file.is_open())
			{
				Debug::PrintError("[File::Open] Fail while creating and opening and creating file: " + path);
			}
			else
			{
				isOpen = true;
			}
		}
		else
		{
			Debug::Print("[File::Open] Fail while opening file or file not found: " + path);
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
	if (fileId >= 0)
	{
		sceIoClose(fileId);
		fileId = -1;
	}
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
	return name;

	//if (path.size() == 0)
	//	return "";

	//// Remove all folders from path
	//int finalPos = 0;
	//int lastSlashPos = path.find_last_of('\\');
	//int lastSlashPos2 = path.find_last_of('/');

	//if (lastSlashPos != -1 || lastSlashPos2 != -1)
	//{
	//	if (lastSlashPos2 > lastSlashPos)
	//		finalPos = lastSlashPos2 + 1;
	//	else
	//		finalPos = lastSlashPos + 1;
	//}

	//std::string fileName = path.substr(finalPos);

	//// Remove file extension from path
	//int nextPointPos = fileName.find_first_of('.');
	//if (nextPointPos == -1)
	//	nextPointPos = INT32_MAX;
	//fileName = fileName.substr(0, nextPointPos);

	//return fileName;
}

#pragma endregion

#pragma region Directory

Directory::Directory(std::string _path) : UniqueId(true)
{
#if defined(__vita__)
	_path = PSVITA_BASE_DIR + _path;
#endif
	this->path = _path;
}

Directory::~Directory()
{
	int subDirCount = (int)subdirectories.size();
	for (int i = 0; i < subDirCount; i++)
	{
		delete subdirectories[i];
	}
	subdirectories.clear();
	files.clear();
}

void AddDirectoryFiles(std::vector<std::shared_ptr<File>>& vector, Directory* directory)
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

std::vector<std::shared_ptr<File>> Directory::GetAllFiles(bool recursive)
{
	FileSystem::fileSystem->FillDirectory(this, recursive);
	std::vector<std::shared_ptr<File>> vector;
	AddDirectoryFiles(vector, this);
	return vector;
}

bool Directory::CheckIfExist()
{
	bool exists = false;
#if defined(__PSP__)
	DIR* dir = opendir(this->path.c_str());
	if (dir == NULL)
	{
		exists = false;
	}
	else {
		closedir(dir);
		exists = true;
	}
#else
	exists = std::filesystem::exists(this->path);
#endif
	return exists;
}

void FileSystem::FillDirectory(Directory* directory, bool recursive)
{
	int directorySubDirectoryCount = directory->subdirectories.size();
	for (int i = 0; i < directorySubDirectoryCount; i++)
	{
		delete directory->subdirectories[i];
	}
	directory->files.clear();
	directory->subdirectories.clear();
	if (!directory->CheckIfExist())
	{
		return;
	}
#if defined(__PSP__)
	DIR* dir = opendir(directory->GetPath().c_str());
	if (dir == NULL)
	{
		return;
	}
	struct dirent* ent;
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
			continue;
		}

		if (S_ISREG(statbuf.st_mode)) // If the entry is a file
		{
			std::shared_ptr<File> newFile = nullptr;
			try
			{
				newFile = FileSystem::MakeFile(fullPath);
				directory->files.push_back(newFile);
			}
			catch (const std::exception&)
			{
			}
		}
		else if (S_ISDIR(statbuf.st_mode)) // If the entry is a folder
		{
			Directory* newDirectory = nullptr;
			try
			{
				newDirectory = new Directory(fullPath + "\\");
				if (recursive)
					newDirectory->GetAllFiles(true);
				directory->subdirectories.push_back(newDirectory);
			}
			catch (const std::exception&)
			{
				if (newDirectory != nullptr)
					delete newDirectory;
			}
		}
	}
	closedir(dir);
#else
	for (const auto& file : std::filesystem::directory_iterator(directory->GetPath()))
	{
		if (file.is_directory())
		{
			Directory* newDirectory = nullptr;
			try
			{
				std::string path = file.path().string();
#if defined(__vita__)
				path = path.substr(4);
#endif
				newDirectory = new Directory(path + "\\");
				if (recursive)
					newDirectory->GetAllFiles(true);
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
			std::shared_ptr<File> newFile = nullptr;
			try
			{
				std::string path = file.path().string();
#if defined(__vita__)
				path = path.substr(4);
#endif
				newFile = FileSystem::MakeFile(path);
				directory->files.push_back(newFile);
			}
			catch (const std::exception&)
			{
			}
		}
	}
#endif
}

bool FileSystem::Rename(const std::string& path, const std::string& newPath)
{
	bool success = true;
	try
	{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
		std::filesystem::rename(path, newPath);
#endif
	}
	catch (const std::exception&)
	{
		success = false;
	}
	return success;
}

std::vector< std::shared_ptr<File>> files;

std::shared_ptr<File> FileSystem::MakeFile(const std::string& path)
{
	std::shared_ptr<File> file;

	int fileCount = files.size();
	for (int i = 0; i < fileCount; i++)
	{
		if (files[i]->GetPath() == path)
		{
			file = files[i];
			break;
		}
	}

	if (!file)
	{
		file = std::make_shared<File>(path);
		files.push_back(file);
	}

	return file;
}

#pragma endregion

#pragma region Read/Input

#pragma endregion

void FileSystem::CreateDirectory(const std::string& path)
{
	std::string finalPath = path;
#if defined(__vita__)
	finalPath = PSVITA_BASE_DIR + finalPath;
#endif
	std::filesystem::create_directory(finalPath);
}

void FileSystem::DeleteFile(const std::string& path)
{
	std::string newPath = path;
#if defined(__vita__)
	newPath = PSVITA_BASE_DIR + path;
#endif

#if defined(__PSP__)
	sceIoRemove(newPath.c_str());
#else
	try
	{
		std::filesystem::remove_all(newPath.c_str());
	}
	catch (const std::exception&)
	{

	}
#endif
}

#pragma region Write/Output

#pragma endregion

int FileSystem::InitFileSystem()
{
#if defined(__vita__)
	sceIoMkdir("ux0:/data/xenity_engine", 0777);
#endif

	Debug::Print("-------- File System initiated --------");
	return 0;
}

std::string FileSystem::GetGamePath()
{
	return EngineSettings::RootFolder;
}