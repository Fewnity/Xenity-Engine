// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(__PSP__)

#include "file_psp.h"
#include "directory.h"
#include "file.h"

#include <engine/engine_settings.h>
#include <engine/debug/debug.h>

#include <dirent.h>
#include <sys/stat.h>

#include <filesystem>
#include <string>

FilePSP::FilePSP(std::string _path) : File(_path)
{
}

FilePSP::~FilePSP()
{
	Close();
}

void FilePSP::Close()
{
	if (fileId >= 0)
	{
		sceIoClose(fileId);
		fileId = -1;
	}
}

void FilePSP::Write(const std::string& data)
{
	if (currentFileMode == FileMode::ReadOnly)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Read Only mode");
		return;
	}

	fileId = sceIoOpen(path.c_str(), PSP_O_RDWR, 0777);
	if (fileId >= 0)
	{
		sceIoLseek(fileId, 0, SEEK_END);
		const int b = sceIoWrite(fileId, data.c_str(), data.size());
		sceIoClose(fileId);
		fileId = -1;
	}
}

std::string FilePSP::ReadAll()
{
	if (currentFileMode == FileMode::WriteOnly || currentFileMode == FileMode::WriteCreateFile)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Write mode");
		return "";
	}

	std::string allText = "";
	fileId = sceIoOpen(path.c_str(), PSP_O_RDONLY, 0);
	if (fileId >= 0)
	{
		const int pos = sceIoLseek(fileId, 0, SEEK_END);
		sceIoLseek(fileId, 0, SEEK_SET);
		char* data = new char[pos + 1];
		data[pos] = 0;
		sceIoRead(fileId, data, pos);
		allText = data;
		delete[] data;

		sceIoClose(fileId);
		fileId = -1;
	}
	return allText;
}

unsigned char* FilePSP::ReadAllBinary(int& size)
{
	if (currentFileMode == FileMode::WriteOnly || currentFileMode == FileMode::WriteCreateFile)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Write mode");
		return nullptr;
	}

	char* data = nullptr;
	fileId = sceIoOpen(path.c_str(), PSP_O_RDONLY, 0);
	if (fileId >= 0)
	{
		SceIoStat file_stat;
		sceIoGetstat(path.c_str(), &file_stat);
		sceIoLseek(fileId, 0, SEEK_SET);
		data = new char[file_stat.st_size + 1];
		sceIoRead(fileId, data, file_stat.st_size);
		size = file_stat.st_size;

		sceIoClose(fileId);
		fileId = -1;
	}
	return (unsigned char*)data;
}

bool FilePSP::CheckIfExist()
{
	bool exists = false;

	const int params = PSP_O_RDONLY;
	fileId = sceIoOpen(path.c_str(), params, 0777);
	if (fileId >= 0)
	{
		exists = true;
		sceIoClose(fileId);
		fileId = -1;
	}

	return exists;
}

bool FilePSP::Open(FileMode fileMode)
{
	currentFileMode = fileMode;

	bool isOpen = false;
	int params = 0;
	if (fileMode == FileMode::WriteOnly || fileMode == FileMode::WriteCreateFile)
		params = PSP_O_WRONLY;
	else
		params = PSP_O_RDONLY;

	if (fileMode == FileMode::WriteCreateFile)
		params |= PSP_O_CREAT;
	fileId = sceIoOpen(path.c_str(), params, 0777);
	if (fileId >= 0)
	{
		isOpen = true;
		sceIoClose(fileId);
		fileId = -1;
	}

	return isOpen;
}

#endif