#include "file.h"
#include <filesystem>
#include "../debug/debug.h"
#include "file_system.h"

File::File(std::string _path) : UniqueId(true)
{
#if defined(__vita__)
	_path = PSVITA_BASE_DIR + _path;
#elif defined(_EE)
	_path = "host0:" + _path;
#endif
	this->path = _path;
	size_t pointIndex = path.find_last_of('.');
	pathExtention = path.substr(pointIndex);

	// Remove all folders from path
	int finalPos = 0;
	int lastSlashPos = (int)path.find_last_of('\\');
	int lastSlashPos2 = (int)path.find_last_of('/');

	if (lastSlashPos != -1 || lastSlashPos2 != -1)
	{
		if (lastSlashPos2 > lastSlashPos)
			finalPos = lastSlashPos2 + 1;
		else
			finalPos = lastSlashPos + 1;
	}

	std::string fileName = path.substr(finalPos);

	// Remove file extension from path
	int nextPointPos = (int)fileName.find_first_of('.');
	if (nextPointPos == -1)
		nextPointPos = INT32_MAX;
	name = fileName.substr(0, nextPointPos);
}

File::~File()
{
	Close();
}

std::string File::GetFolderPath() const
{
	if (path.size() == 0)
		return "";

	int lastSlashPos = (int)path.find_last_of('\\');
	if (lastSlashPos == -1)
		lastSlashPos = 0;
#if defined(__vita__)
	std::string fileName = path.substr(4, lastSlashPos + 1);
#elif defined(__EE)
	std::string fileName = path.substr(6, lastSlashPos + 1);
#else
	std::string fileName = path.substr(0, lastSlashPos + 1);
#endif

	return fileName;
}

std::string File::GetFileName() const
{
	return name;
}