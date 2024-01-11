#include "file.h"
#include "file_system.h"

#include <engine/debug/debug.h>

#include <filesystem>

File::File(std::string _path) : UniqueId(true)
{
#if defined(__vita__)
	_path = PSVITA_BASE_DIR + _path;
#elif defined(_EE)
	_path = "mass:" + _path;
	//_path = "host0:" + _path;
#endif
	this->path = _path;
	const size_t pointIndex = path.find_last_of('.');
	pathExtention = path.substr(pointIndex);

	// Remove all folders from path
	int finalPos = 0;
	const int lastSlashPos = (int)path.find_last_of('\\');
	const int lastSlashPos2 = (int)path.find_last_of('/');

	if (lastSlashPos != -1 || lastSlashPos2 != -1)
	{
		if (lastSlashPos2 > lastSlashPos)
			finalPos = lastSlashPos2 + 1;
		else
			finalPos = lastSlashPos + 1;
	}

	const std::string fileName = path.substr(finalPos);

	// Remove file extension from path
	int nextPointPos = (int)fileName.find_first_of('.');
	if (nextPointPos == -1)
		nextPointPos = INT32_MAX;
	name = fileName.substr(0, nextPointPos);
#if defined(_EE)
	const int pathLen = path.size();
	for (int i = 0; i < pathLen; i++)
	{
		if (path[i] == '\\')
		{
			path[i] = '/';
		}
	}
#endif
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
	const std::string fileName = path.substr(4, lastSlashPos + 1);
#elif defined(_EE)
	const std::string fileName = path.substr(5, lastSlashPos + 1);
	// std::string fileName = path.substr(6, lastSlashPos + 1);
#else
	const std::string fileName = path.substr(0, lastSlashPos + 1);
#endif

	return fileName;
}

std::string File::GetFileName() const
{
	return name;
}