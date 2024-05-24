// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "file_reference_finder.h"

// List of all file types drawn by the EditorUI or the editor wont compile
#include <engine/file_system/file_reference.h>
#include <engine/graphics/texture.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/audio/audio_clip.h>
#include <engine/scene_management/scene.h>
#include <engine/graphics/material.h>
#include <engine/graphics/shader.h>
#include <engine/graphics/ui/font.h>
#include <engine/graphics/skybox.h>
#include <engine/debug/debug.h>
#include <engine/graphics/icon.h>

template<typename T>
std::enable_if_t<std::is_base_of<FileReference, T>::value, bool>
FileReferenceFinder::GetFileRefId(const std::reference_wrapper<std::shared_ptr<T>>* valuePtr, std::vector<uint64_t>& ids)
{
	if (valuePtr && valuePtr->get())
	{
		ids.push_back(valuePtr->get()->fileId);
		return true;
	}
	else
	{
		return false;
	}
}

template<typename T>
std::enable_if_t<std::is_base_of<FileReference, T>::value, bool>
FileReferenceFinder::GetFileRefId(const std::reference_wrapper<std::vector<std::shared_ptr<T>>>* valuePtr, std::vector <uint64_t>& ids)
{
	if (valuePtr)
	{
		const std::vector <std::shared_ptr<T>>& getVal = valuePtr->get();
		const size_t vectorSize = getVal.size();
		for (size_t vIndex = 0; vIndex < vectorSize; vIndex++)
		{
			if (getVal.at(vIndex))
			{
				ids.push_back(getVal.at(vIndex)->fileId);
			}
		}
		return true;
	}
	else 
	{
		return false;
	}
}

template<typename T>
bool FileReferenceFinder::GetFileRefId(const T& var, std::vector <uint64_t>& ids)
{
	return false;
}

void FileReferenceFinder::GetUsedFilesInReflectiveData(std::vector<uint64_t>& usedFilesIds, const ReflectiveData& reflectiveData)
{
	size_t usedFilesIdsCount = usedFilesIds.size();

	for (const ReflectiveEntry& reflectiveEntry : reflectiveData)
	{
		const VariableReference& variableRef = reflectiveEntry.variable.value();
		bool isFileFound = false;
		std::vector<uint64_t> foundFileIds;
		std::visit([&foundFileIds, &isFileFound](const auto& value)
			{
				isFileFound = GetFileRefId(&value, foundFileIds);
			}, variableRef);

		if (isFileFound)
		{
			const size_t foundFileIdsCount = foundFileIds.size();
			for (size_t foundFileIndex = 0; foundFileIndex < foundFileIdsCount; foundFileIndex++)
			{
				bool alreadyListed = false;
				for (size_t fileIndex = 0; fileIndex < usedFilesIdsCount; fileIndex++)
				{
					if (usedFilesIds[fileIndex] == foundFileIds[foundFileIndex])
					{
						alreadyListed = true;
						break;
					}
				}
				if (!alreadyListed)
				{
					usedFilesIds.push_back(foundFileIds[foundFileIndex]);
					usedFilesIdsCount++;
				}
			}
		}
	}
}