// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "file_default.h"

#include <string>

#include <engine/engine_settings.h>
#include <engine/debug/debug.h>


FileDefault::FileDefault(std::string _path) : File(_path)
{
}


FileDefault::~FileDefault()
{
	Close();
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
	if (currentFileMode == FileMode::ReadOnly)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Read Only mode");
		return;
	}

	if (file.is_open())
	{
		file.seekg(0, std::ios_base::end);
		file << data;
		file.flush();
	}
}

void FileDefault::Write(const unsigned char* data, size_t size)
{
	if (currentFileMode == FileMode::ReadOnly)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Read Only mode");
		return;
	}

	if (file.is_open())
	{
		file.seekg(0, std::ios_base::end);
		file.write(reinterpret_cast<const char*>(data), size);
		file.flush();
	}
}

std::string FileDefault::ReadAll()
{
	if (currentFileMode == FileMode::WriteOnly || currentFileMode == FileMode::WriteCreateFile)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Write mode");
		return "";
	}

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
	if (currentFileMode == FileMode::WriteOnly || currentFileMode == FileMode::WriteCreateFile)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Write mode");
		return nullptr;
	}

	char* data = nullptr;
	file.seekg(0, std::ios_base::end);
	const std::streampos pos = file.tellg();
	file.seekg(0, std::ios_base::beg);
	if(pos <= 0)
	{
		size = 0;
		return nullptr;
	}

	data = new char[pos];
	if (!data) 
	{
		size = 0;
		return nullptr;
	}

	file.read(data, pos);
	size = pos;
	return (unsigned char*)data;
}

unsigned char* FileDefault::ReadBinary(int offset, int size)
{
	if (currentFileMode == FileMode::WriteOnly || currentFileMode == FileMode::WriteCreateFile)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Write mode");
		return nullptr;
	}

	char* data = nullptr;
	file.seekg(offset, std::ios_base::beg);
	data = new char[size];
	file.read(data, size);

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
		std::ios_base::openmode params = std::fstream::in;

		file.open(path, params);

		if (file.is_open())
		{
			exists = true;
		}
		file.close();
	}
	return exists;
}

bool FileDefault::Open(FileMode fileMode)
{
	currentFileMode = fileMode;

	bool isOpen = false;
	std::ios_base::openmode params = std::fstream::binary;
	if (fileMode == FileMode::WriteOnly || fileMode == FileMode::WriteCreateFile)
		params |= std::fstream::app;
	else
		params |= std::fstream::in;

	file.open(path, params);
	if (!file.is_open())
	{
		if (fileMode == FileMode::WriteCreateFile)
		{
			// Try to create the file
			params |= std::fstream::trunc;
			file.open(path, params);
			if (!file.is_open())
			{
				Debug::PrintError("[File::Open] Fail while creating and opening and creating file: " + path, true);
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
