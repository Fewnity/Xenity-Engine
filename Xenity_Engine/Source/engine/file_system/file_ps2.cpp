#if defined(_EE)

#include "file_ps2.h"
#include "directory.h"
#include "file.h"

#include <engine/engine_settings.h>
#include <engine/debug/debug.h>

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

FilePS2::FilePS2(std::string _path) : File(_path)
{
}

FilePS2::~FilePS2()
{
	Close();
}

void FilePS2::Close()
{
	if (fileId >= 0)
	{
		fileXioClose(fileId);
		fileId = -1;
	}
}

void FilePS2::Write(const std::string &data)
{
	if (currentFileMode == FileMode::ReadOnly)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Read Only mode");
		return;
	}

	fileId = fileXioOpen(path.c_str(), FIO_O_WRONLY);
	if (fileId >= 0)
	{
		fileXioLseek(fileId, 0, SEEK_END);
		const int b = fileXioWrite(fileId, data.c_str(), data.size());
		fileXioClose(fileId);
		fileId = -1;
	}
}

std::string FilePS2::ReadAll()
{
	if (currentFileMode == FileMode::WriteOnly || currentFileMode == FileMode::WriteCreateFile)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Write mode");
		return "";
	}

	std::string allText = "";
	fileId = fileXioOpen(path.c_str(), FIO_O_RDONLY, 0777);
	if (fileId >= 0)
	{
		const int pos = fileXioLseek(fileId, 0, SEEK_END);
		fileXioLseek(fileId, 0, SEEK_SET);
		char *data = new char[pos + 1];
		data[pos] = 0;
		fileXioRead(fileId, data, pos);
		allText = data;
		delete[] data;

		fileXioClose(fileId);
		fileId = -1;
	}
	return allText;
}

unsigned char *FilePS2::ReadAllBinary(int &size)
{
	if (currentFileMode == FileMode::WriteOnly || currentFileMode == FileMode::WriteCreateFile)
	{
		Debug::PrintError("[File::ReadAllBinary] The file is in Write mode");
		return nullptr;
	}

	char *data = nullptr;
	fileId = fileXioOpen(path.c_str(), FIO_O_RDONLY, 0777);
	if (fileId >= 0)
	{
		iox_stat_t file_stat;
		fileXioGetStat(path.c_str(), &file_stat);
		fileXioLseek(fileId, 0, SEEK_SET);
		data = new char[file_stat.size + 1];
		fileXioRead(fileId, data, file_stat.size);
		size = file_stat.size;

		fileXioClose(fileId);
		fileId = -1;
	}
	return (unsigned char *)data;
}

bool FilePS2::CheckIfExist()
{
	bool exists = false;

	const int params = FIO_O_RDONLY;
	fileId = fileXioOpen(path.c_str(), params, 0777);
	if (fileId >= 0)
	{
		exists = true;
		fileXioClose(fileId);
		fileId = -1;
	}

	return exists;
}

bool FilePS2::Open(FileMode fileMode)
{
	currentFileMode = fileMode;

	bool isOpen = false;
	int params = 0;
	if (fileMode == FileMode::WriteOnly || fileMode == FileMode::WriteCreateFile)
		params = FIO_O_WRONLY;
	else
		params = FIO_O_RDONLY;

	if (fileMode == FileMode::WriteCreateFile)
		params |= FIO_O_CREAT;
	fileId = fileXioOpen(path.c_str(), params, 0777);
	if (fileId >= 0)
	{
		isOpen = true;
		fileXioClose(fileId);
		fileId = -1;
	}

	return isOpen;
}

#endif