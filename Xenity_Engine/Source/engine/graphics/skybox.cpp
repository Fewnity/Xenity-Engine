// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "skybox.h"

#include <json.hpp>

#include <engine/reflection/reflection_utils.h>
#include <engine/debug/debug.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/file_system/file.h>
#include <engine/debug/stack_debug_object.h>
#include <engine/graphics/texture/texture.h>

using ordered_json = nlohmann::ordered_json;

SkyBox::SkyBox(const std::shared_ptr<Texture>& _front, const std::shared_ptr<Texture>& _back, const std::shared_ptr<Texture>& _up, const std::shared_ptr<Texture>& _down, const std::shared_ptr<Texture>& _left, const std::shared_ptr<Texture>& _right)
	: front(_front), back(_back), up(_up), down(_down), left(_left), right(_right)
{
}

ReflectiveData SkyBox::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	AddVariable(reflectedVariables, front, "front", true);
	AddVariable(reflectedVariables, back, "back", true);
	AddVariable(reflectedVariables, up, "up", true);
	AddVariable(reflectedVariables, down, "down", true);
	AddVariable(reflectedVariables, left, "left", true);
	AddVariable(reflectedVariables, right, "right", true);
	return reflectedVariables;
}

ReflectiveData SkyBox::GetMetaReflectiveData([[maybe_unused]] AssetPlatform platform)
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

void SkyBox::OnReflectionUpdated()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

#if defined(EDITOR)
	ordered_json jsonData;
	jsonData["Values"] = ReflectionUtils::ReflectiveDataToJson(GetReflectiveData());
	jsonData["Version"] = s_version;

	const bool saveResult = ReflectionUtils::JsonToFile(jsonData, m_file);
	if (!saveResult)
	{
		Debug::PrintError("[SkyBox::OnReflectionUpdated] Fail to save the Skybox file: " + m_file->GetPath(), true);
	}
#endif
}

void SkyBox::LoadFileReference(const LoadOptions& loadOptions)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	if (m_fileStatus == FileStatus::FileStatus_Not_Loaded)
	{
		const std::string jsonString = ReadString();
		if (!jsonString.empty())
		{
			ordered_json j;
			try
			{
				j = ordered_json::parse(jsonString);
			}
			catch (const std::exception&)
			{
				Debug::PrintError("[ProjectManager::LoadFileReference] Failed to load the material file", true);
				m_fileStatus = FileStatus::FileStatus_Failed;
				return;
			}
			ReflectionUtils::JsonToReflectiveData(j, GetReflectiveData());

			m_fileStatus = FileStatus::FileStatus_Loaded;
		}
		else
		{
			Debug::PrintError("[SkyBox::LoadFileReference] Fail to load the skybox file: " + m_file->GetPath(), true);
			m_fileStatus = FileStatus::FileStatus_Failed;
		}
	}
}

void SkyBox::UnloadFileReference()
{
	front = nullptr;
	back = nullptr;
	up = nullptr;
	down = nullptr;
	left = nullptr;
	right = nullptr;
}

std::shared_ptr<SkyBox> SkyBox::MakeSkyBox()
{
	const std::shared_ptr<SkyBox> newFileRef = std::make_shared<SkyBox>();
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}
