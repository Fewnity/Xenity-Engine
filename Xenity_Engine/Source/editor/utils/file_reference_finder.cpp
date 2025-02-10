// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "file_reference_finder.h"

// List of all file types drawn by the EditorUI or the editor wont compile
#include <engine/file_system/file_reference.h>
#include <engine/graphics/texture/texture.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/audio/audio_clip.h>
#include <engine/scene_management/scene.h>
#include <engine/graphics/material.h>
#include <engine/graphics/shader/shader.h>
#include <engine/graphics/ui/font.h>
#include <engine/graphics/skybox.h>
#include <engine/debug/debug.h>
#include <engine/graphics/ui/icon.h>

template<typename T>
std::enable_if_t<std::is_base_of<FileReference, T>::value, bool>
FileReferenceFinder::GetFileRefId(const std::reference_wrapper<std::shared_ptr<T>>* valuePtr, std::set<uint64_t>& ids)
{
	if (valuePtr && valuePtr->get())
	{
		ids.insert(valuePtr->get()->GetFileId());
		return true;
	}
	else
	{
		return false;
	}
}

template<typename T>
std::enable_if_t<std::is_base_of<FileReference, T>::value, bool>
FileReferenceFinder::GetFileRefId(const std::reference_wrapper<std::vector<std::shared_ptr<T>>>* valuePtr, std::set<uint64_t>& ids)
{
	if (valuePtr)
	{
		const std::vector <std::shared_ptr<T>>& getVal = valuePtr->get();
		const size_t vectorSize = getVal.size();
		for (size_t vIndex = 0; vIndex < vectorSize; vIndex++)
		{
			if (getVal.at(vIndex))
			{
				ids.insert(getVal.at(vIndex)->GetFileId());
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
bool FileReferenceFinder::GetFileRefId(const T& var, std::set<uint64_t>& ids)
{
	return false;
}

void FileReferenceFinder::GetUsedFilesInReflectiveData(std::set<uint64_t>& usedFilesIds, const ReflectiveData& reflectiveData)
{
	for (const ReflectiveEntry& reflectiveEntry : reflectiveData)
	{
		const VariableReference& variableRef = reflectiveEntry.variable.value();
		bool isFileFound = false;
		std::set<uint64_t> foundFileIds;
		std::visit([&foundFileIds, &isFileFound](const auto& value)
			{
				isFileFound = GetFileRefId(&value, foundFileIds);
			}, variableRef);

		if (isFileFound)
		{
			for (uint64_t id : foundFileIds)
			{
				usedFilesIds.insert(id);
			}
		}
	}
}