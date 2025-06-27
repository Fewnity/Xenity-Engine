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
#include <engine/missing_script.h>
#include <engine/asset_management/project_manager.h>
#include <engine/game_elements/prefab.h>

using ordered_json = nlohmann::ordered_json;

template<typename T>
std::enable_if_t<std::is_base_of<FileReference, T>::value, bool>
FileReferenceFinder::GetFileRefId(const std::reference_wrapper<std::shared_ptr<T>>* valuePtr, std::set<uint64_t>& ids)
{
	if (valuePtr && valuePtr->get())
	{
		auto ret = ids.insert(valuePtr->get()->GetFileId());
		if(valuePtr->get()->GetFileType() == FileType::File_Prefab && ret.second)
		{
			GetUsedFilesInJson(ids, std::dynamic_pointer_cast<Prefab>(valuePtr->get())->GetData());
		}
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
		const std::vector<std::shared_ptr<T>>& fileList = valuePtr->get();
		const size_t vectorSize = fileList.size();
		for (size_t index = 0; index < vectorSize; index++)
		{
			if (fileList.at(index))
			{
				ids.insert(fileList.at(index)->GetFileId());
			}
		}
		return true;
	}
	else 
	{
		return false;
	}
}

// If the variable is not a file pointer
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
		std::visit([&usedFilesIds](const auto& value)
			{
				GetFileRefId(&value, usedFilesIds);
			}, variableRef);
	}
}

void FileReferenceFinder::ExtractInts(const ordered_json& j, std::vector<uint64_t>& result) 
{
	if (j.is_number_integer()) 
	{
		result.push_back(j.get<uint64_t>());
	}
	else if (j.is_array() || j.is_object()) 
	{
		for (const auto& el : j) 
		{
			ExtractInts(el, result);
		}
	}
}

void FileReferenceFinder::GetUsedFilesInJson(std::set<uint64_t>& usedFilesIds, const ordered_json& json)
{
	std::vector<uint64_t> ids;
	ExtractInts(json, ids);
	for (uint64_t id : ids)
	{
		if(ProjectManager::GetFileReferenceById(id))
		{
			usedFilesIds.insert(id);
		}
	}
}
