// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(__PS3__)

#include "file_ps3.h"

#include <string>
#include <lv2/sysfs.h>

#include <engine/engine_settings.h>
#include <engine/debug/debug.h>
#include <engine/constants.h>
#include <engine/application.h>

#include "directory.h"
#include "file.h"

static constexpr int FS_SEEK_SET = 0;
static constexpr int FS_SEEK_END = 2;

FilePS3::FilePS3(const std::string& _path) : File(Application::GetGameFolder() + _path)
{
}

FilePS3::~FilePS3()
{
	Close();
}

void FilePS3::Close()
{
	if (m_fileId >= 0)
	{
		sysFsClose(m_fileId);
		m_fileId = -1;
	}
}

void FilePS3::Write(const std::string& data)
{
	if (m_currentFileMode == FileMode::ReadOnly)
	{
		Debug::PrintError("[File::Write] The file is in Read Only mode");
		return;
	}

	if (m_fileId >= 0)
	{
		uint64_t pos;
		sysFsLseek(m_fileId, 0, FS_SEEK_END, &pos);
		u64 written;
		sysFsWrite(m_fileId, data.c_str(), data.size(), &written);
	}
}

void FilePS3::Write(const unsigned char* data, size_t size)
{
}

std::string FilePS3::ReadAll()
{
	if (m_currentFileMode == FileMode::WriteOnly || m_currentFileMode == FileMode::WriteCreateFile)
	{
		Debug::PrintError("[File::ReadAll] The file is in Write mode");
		return "";
	}

	std::string allText = "";
	if (m_fileId >= 0)
	{
		uint64_t size;
		sysFsLseek(m_fileId, 0, FS_SEEK_END, &size);
		uint64_t pos;
		sysFsLseek(m_fileId, 0, FS_SEEK_SET, &pos);

		char* data = new char[size + 1];
		data[size] = 0;
		uint64_t read;
		sysFsRead(m_fileId, data, size, &read);

		allText = data;
		delete[] data;
	}
	return allText;
}

unsigned char* FilePS3::ReadAllBinary(size_t& size)
{
	if (m_currentFileMode == FileMode::WriteOnly || m_currentFileMode == FileMode::WriteCreateFile)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Write mode");
		return nullptr;
	}

	char* data = nullptr;
	if (m_fileId >= 0)
	{
		sysFSStat file_stat;
		sysFsStat(m_path.c_str(), &file_stat);
		uint64_t pos;
		sysFsLseek(m_fileId, 0, FS_SEEK_SET, &pos);
		data = new char[file_stat.size + 1];

		uint64_t read;
		sysFsRead(m_fileId, data, file_stat.size, &read);
		size = file_stat.size;
	}
	return (unsigned char*)data;
}

unsigned char* FilePS3::ReadBinary(size_t offset, size_t size)
{
	char* data = nullptr;
	if (m_fileId >= 0)
	{
		uint64_t pos;
		sysFsLseek(m_fileId, offset, FS_SEEK_SET, &pos);
		data = new char[size];
		uint64_t read;
		sysFsRead(m_fileId, data, size, &read);
	}
	return (unsigned char*)data;
}

bool FilePS3::CheckIfExist()
{
	/*bool exists = false;

	const int params = PSP_O_RDONLY;
	fileId = sceIoOpen(path.c_str(), params, 0777);
	if (fileId >= 0)
	{
		exists = true;
		sceIoClose(fileId);
		fileId = -1;
	}*/
	bool exists = true;
	return exists;
}

bool FilePS3::Open(FileMode fileMode)
{
	m_currentFileMode = fileMode;

	bool isOpen = false;
	int params = 0;
	if (fileMode == FileMode::WriteOnly || fileMode == FileMode::WriteCreateFile)
		params = SYS_O_WRONLY;
	else
		params = SYS_O_RDONLY;
	if (fileMode == FileMode::WriteCreateFile)
		params |= SYS_O_CREAT;

	sysFsOpen(m_path.c_str(), params, &m_fileId, nullptr, 0);
	if (m_fileId >= 0)
	{
		isOpen = true;
	}

	return isOpen;
}

#endif