// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "file_default.h"

#include <string>
#include <sstream>

#include <engine/engine_settings.h>
#include <engine/debug/debug.h>

FileDefault::FileDefault(const std::string& _path) : File(_path)
{
}


FileDefault::~FileDefault()
{
	Close();
}

void FileDefault::Close()
{
	if (m_file.is_open())
	{
		m_file.close();
	}
}

void FileDefault::Write(const std::string& data)
{
	if (m_currentFileMode == FileMode::ReadOnly)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Read Only mode");
		return;
	}

	if (m_file.is_open())
	{
		m_file.seekg(0, std::ios_base::end);
		m_file << data;
		m_file.flush();
	}
}

void FileDefault::Write(const unsigned char* data, size_t size)
{
	if (m_currentFileMode == FileMode::ReadOnly)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Read Only mode");
		return;
	}

	if (m_file.is_open())
	{
		m_file.seekg(0, std::ios_base::end);
		m_file.write(reinterpret_cast<const char*>(data), size);
		m_file.flush();
	}
}

std::string FileDefault::ReadAll()
{
	if (m_currentFileMode == FileMode::WriteOnly || m_currentFileMode == FileMode::WriteCreateFile)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Write mode");
		return "";
	}

	std::stringstream allText;
	m_file.seekg(0, std::ios_base::beg);
	std::string tempText;
	while (getline(m_file, tempText))
	{
		allText << tempText;
		allText << "\n";
	}
	return allText.str();
}

unsigned char* FileDefault::ReadAllBinary(size_t& size)
{
	if (m_currentFileMode == FileMode::WriteOnly || m_currentFileMode == FileMode::WriteCreateFile)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Write mode");
		return nullptr;
	}

	m_file.seekg(0, std::ios_base::end);
	const std::streampos pos = m_file.tellg();
	m_file.seekg(0, std::ios_base::beg);
	if(pos <= 0)
	{
		size = 0;
		return nullptr;
	}

	char* data = static_cast<char*>(malloc(pos));
	if (!data) 
	{
		size = 0;
		return nullptr;
	}

	m_file.read(data, pos);
	size = pos;
	return reinterpret_cast<unsigned char*>(data);
}

unsigned char* FileDefault::ReadBinary(size_t offset, size_t size)
{
	if (m_currentFileMode == FileMode::WriteOnly || m_currentFileMode == FileMode::WriteCreateFile)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Write mode");
		return nullptr;
	}

	m_file.seekg(offset, std::ios_base::beg);
	char* data = static_cast<char*>(malloc(size));
	m_file.read(data, size);

	return (unsigned char*)data;
}

bool FileDefault::CheckIfExist()
{
	bool exists = false;
	if (m_file.is_open())
	{
		exists = true;
	}
	else
	{
		const std::ios_base::openmode params = std::fstream::in;

		m_file.open(m_path, params);

		if (m_file.is_open())
		{
			exists = true;
		}
		m_file.close();
	}
	return exists;
}

bool FileDefault::Open(FileMode fileMode)
{
	m_currentFileMode = fileMode;

	bool isOpen = false;
	std::ios_base::openmode params = std::fstream::binary;
	if (fileMode == FileMode::WriteOnly || fileMode == FileMode::WriteCreateFile)
		params |= std::fstream::app;
	else
		params |= std::fstream::in;

	m_file.open(m_path, params);
	if (!m_file.is_open())
	{
		if (fileMode == FileMode::WriteCreateFile)
		{
			// Try to create the file
			params |= std::fstream::trunc;
			m_file.open(m_path, params);
			if (!m_file.is_open())
			{
				Debug::PrintError("[File::Open] Fail while creating and opening and creating file: " + m_path, true);
			}
			else
			{
				isOpen = true;
			}
		}
		else
		{
			Debug::Print("[File::Open] Fail while opening file or file not found: " + m_path);
		}
	}
	else
	{
		isOpen = true;
	}
	return isOpen;
}
