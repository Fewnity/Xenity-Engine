#include "file_default.h"
#include "file_psp.h"

#include "directory.h"

#include <engine/engine_settings.h>
#include <engine/debug/debug.h>

#include <filesystem>
#include <string>

FileDefault::FileDefault(std::string _path) : File(_path)
{
}

void FileDefault::Close()
{
	if (file.is_open())
	{
		file.close();
	}
}

void FileDefault::Write(const std::string& data)
{
	if (file.is_open())
	{
		file.seekg(0, std::ios_base::end);
		file << data;
		file.flush();
	}
}

std::string FileDefault::ReadAll()
{
	std::string allText = "";
	file.seekg(0, std::ios_base::beg);
	std::string tempText;
	while (getline(file, tempText))
	{
		allText += tempText;
		allText += "\n";
	}
	return allText;
}

unsigned char* FileDefault::ReadAllBinary(int& size)
{
	char* data = nullptr;
	file.seekg(0, std::ios_base::end);
	int pos = file.tellg();
	file.seekg(0, std::ios_base::beg);
	data = new char[pos];
	file.read(data, pos);
	size = pos;
	return (unsigned char*)data;
}

bool FileDefault::CheckIfExist()
{
	bool exists = false;
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
	return exists;
}

bool FileDefault::Open(bool createFileIfNotFound)
{
	bool isOpen = false;

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
	return isOpen;
}
