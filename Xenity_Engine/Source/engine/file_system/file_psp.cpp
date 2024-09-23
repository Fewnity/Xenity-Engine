// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(__PSP__)

#include "file_psp.h"

#include <dirent.h>
#include <sys/stat.h>
#include <filesystem>
#include <string>
#include <pspkernel.h>

#include <engine/engine_settings.h>
#include <engine/debug/debug.h>
#include "directory.h"
#include "file.h"

FilePSP::FilePSP(const std::string& _path) : File(_path)
{
}

FilePSP::~FilePSP()
{
	Close();
}

void FilePSP::Close()
{
	if (m_fileId >= 0)
	{
		sceIoClose(m_fileId);
		m_fileId = -1;
	}
}

void FilePSP::Write(const std::string& data)
{
	if (m_currentFileMode == FileMode::ReadOnly)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Read Only mode");
		return;
	}

	if (m_fileId >= 0)
	{
		sceIoLseek(m_fileId, 0, SEEK_END);
		const int b = sceIoWrite(m_fileId, data.c_str(), data.size());
	}
}

void FilePSP::Write(const unsigned char* data, size_t size)
{
}

std::string FilePSP::ReadAll()
{
	if (m_currentFileMode == FileMode::WriteOnly || m_currentFileMode == FileMode::WriteCreateFile)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Write mode");
		return "";
	}

	std::string allText = "";
	if (m_fileId >= 0)
	{
		const int pos = sceIoLseek(m_fileId, 0, SEEK_END);
		sceIoLseek(m_fileId, 0, SEEK_SET);
		char* data = (char*)malloc(pos + 1);
		data[pos] = 0;
		sceIoRead(m_fileId, data, pos);
		allText = data;
		free(data);
	}
	return allText;
}

unsigned char* FilePSP::ReadAllBinary(int& size)
{
	if (m_currentFileMode == FileMode::WriteOnly || m_currentFileMode == FileMode::WriteCreateFile)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Write mode");
		return nullptr;
	}

	char* data = nullptr;
	if (m_fileId >= 0)
	{
		SceIoStat file_stat;
		sceIoGetstat(m_path.c_str(), &file_stat);
		sceIoLseek(m_fileId, 0, SEEK_SET);
		data = (char*)malloc(file_stat.st_size + 1);
		sceIoRead(m_fileId, data, file_stat.st_size);
		size = file_stat.st_size;
	}
	return (unsigned char*)data;
}

unsigned char* FilePSP::ReadBinary(int offset, int size)
{
	char* data = nullptr;
	if (m_fileId >= 0)
	{
		sceIoLseek(m_fileId, offset, SEEK_SET);
		data = (char*)malloc(size);
		sceIoRead(m_fileId, data, size);
	}
	return (unsigned char*)data;
}

bool FilePSP::CheckIfExist()
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

bool FilePSP::Open(FileMode fileMode)
{
	m_currentFileMode = fileMode;

	bool isOpen = false;
	int params = 0;
	if (fileMode == FileMode::WriteOnly || fileMode == FileMode::WriteCreateFile)
		params = PSP_O_WRONLY;
	else
		params = PSP_O_RDONLY;

	if (fileMode == FileMode::WriteCreateFile)
		params |= PSP_O_CREAT;
	m_fileId = sceIoOpen(m_path.c_str(), params, 0777);
	if (m_fileId >= 0)
	{
		isOpen = true;
	}

	return isOpen;
}

#endif