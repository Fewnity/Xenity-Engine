// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(_EE)

#include "file_ps2.h"

#include <dirent.h>
#include <sys/stat.h>
#include <filesystem>
#include <string>
#define NEWLIB_PORT_AWARE
#include <fileXio_rpc.h>
#include <fileio.h>

#include <sifrpc.h>
#include <loadfile.h>
#include <sbv_patches.h>
#include <iopcontrol.h>
#include <iopheap.h>
#include <debug.h>

#include <engine/engine_settings.h>
#include <engine/debug/debug.h>
#include "directory.h"
#include "file.h"


FilePS2::FilePS2(const std::string& _path) : File(_path)
{
}

FilePS2::~FilePS2()
{
	Close();
}

void FilePS2::Close()
{
	if (m_fileId >= 0)
	{
		fileXioClose(m_fileId);
		m_fileId = -1;
	}
}

void FilePS2::Write(const std::string &data)
{
	if (m_currentFileMode == FileMode::ReadOnly)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Read Only mode");
		return;
	}

	m_fileId = fileXioOpen(path.c_str(), FIO_O_WRONLY);
	if (m_fileId >= 0)
	{
		fileXioLseek(m_fileId, 0, SEEK_END);
		const int b = fileXioWrite(m_fileId, data.c_str(), data.size());
		fileXioClose(m_fileId);
		m_fileId = -1;
	}
}

std::string FilePS2::ReadAll()
{
	if (m_currentFileMode == FileMode::WriteOnly || m_currentFileMode == FileMode::WriteCreateFile)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Write mode");
		return "";
	}

	std::string allText = "";
	m_fileId = fileXioOpen(m_path.c_str(), FIO_O_RDONLY, 0777);
	if (m_fileId >= 0)
	{
		const int pos = fileXioLseek(m_fileId, 0, SEEK_END);
		fileXioLseek(m_fileId, 0, SEEK_SET);
		char *data = new char[pos + 1];
		data[pos] = 0;
		fileXioRead(m_fileId, data, pos);
		allText = data;
		delete[] data;

		fileXioClose(m_fileId);
		m_fileId = -1;
	}
	return allText;
}

unsigned char *FilePS2::ReadAllBinary(int &size)
{
	if (m_currentFileMode == FileMode::WriteOnly || m_currentFileMode == FileMode::WriteCreateFile)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Write mode");
		return nullptr;
	}

	char *data = nullptr;
	m_fileId = fileXioOpen(m_path.c_str(), FIO_O_RDONLY, 0777);
	if (m_fileId >= 0)
	{
		iox_stat_t file_stat;
		fileXioGetStat(m_path.c_str(), &file_stat);
		fileXioLseek(m_fileId, 0, SEEK_SET);
		data = new char[file_stat.size + 1];
		fileXioRead(m_fileId, data, file_stat.size);
		size = file_stat.size;

		fileXioClose(m_fileId);
		m_fileId = -1;
	}
	return (unsigned char *)data;
}

bool FilePS2::CheckIfExist()
{
	bool exists = false;

	const int params = FIO_O_RDONLY;
	m_fileId = fileXioOpen(m_path.c_str(), params, 0777);
	if (m_fileId >= 0)
	{
		exists = true;
		fileXioClose(m_fileId);
		m_fileId = -1;
	}

	return exists;
}

bool FilePS2::Open(FileMode fileMode)
{
	m_currentFileMode = fileMode;

	bool isOpen = false;
	int params = 0;
	if (fileMode == FileMode::WriteOnly || fileMode == FileMode::WriteCreateFile)
		params = FIO_O_WRONLY;
	else
		params = FIO_O_RDONLY;

	if (fileMode == FileMode::WriteCreateFile)
		params |= FIO_O_CREAT;
	m_fileId = fileXioOpen(m_path.c_str(), params, 0777);
	if (m_fileId >= 0)
	{
		isOpen = true;
		fileXioClose(m_fileId);
		m_fileId = -1;
	}

	return isOpen;
}

#endif