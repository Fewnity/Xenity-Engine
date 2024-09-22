// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "file.h"

#include <engine/assertions/assertions.h>
#include "file_system.h"

File::File(const std::string& _path) : UniqueId(true)
{

	XASSERT(!_path.empty(), "[File::File] _path is nullptr");

	if (_path.empty())
	{
		return;
	}

#if defined(_EE)
	//_path = "mass:" + _path;
	this->path = "mass:" + _path;
#else
	this->path = _path;
	//_path = "host0:" + _path;
#endif
	path = FileSystem::ConvertWindowsPathToBasicPath(path);
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

std::string File::GetFolderPath() const
{
	if (path.size() == 0)
		return "";

	int lastSlashPos = (int)path.find_last_of('\\');
	if (lastSlashPos == -1)
	{
		lastSlashPos = (int)path.find_last_of('/');
		if (lastSlashPos == -1)
		lastSlashPos = 0;
	}


#if defined(_EE)
	const std::string fileName = path.substr(5, lastSlashPos + 1);
	// std::string fileName = path.substr(6, lastSlashPos + 1);
#else
	const std::string fileName = path.substr(0, lastSlashPos + 1);
#endif

	return fileName;
}

const std::string& File::GetFileName() const
{
	return name;
}