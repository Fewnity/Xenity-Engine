// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "skybox.h"

#include <engine/reflection/reflection_utils.h>
#include <engine/debug/debug.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/file_system/file.h>

#include <json.hpp>

using json = nlohmann::json;

SkyBox::SkyBox()
{
	AssetManager::AddReflection(this);
}

SkyBox::~SkyBox()
{
	AssetManager::RemoveReflection(this);
}

SkyBox::SkyBox(const std::shared_ptr<Texture>& front, const std::shared_ptr<Texture>& back, const std::shared_ptr<Texture>& up, const std::shared_ptr<Texture>& down, const std::shared_ptr<Texture>& left, const std::shared_ptr<Texture>& right)
{
	this->front = front;
	this->back = back;
	this->up = up;
	this->down = down;
	this->left = left;
	this->right = right;
}

ReflectiveData SkyBox::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	AddVariable(reflectedVariables, front, "front", true, true);
	AddVariable(reflectedVariables, back, "back", true, true);
	AddVariable(reflectedVariables, up, "up", true, true);
	AddVariable(reflectedVariables, down, "down", true, true);
	AddVariable(reflectedVariables, left, "left", true, true);
	AddVariable(reflectedVariables, right, "right", true, true);
	return reflectedVariables;
}

ReflectiveData SkyBox::GetMetaReflectiveData(AssetPlatform platform)
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

void SkyBox::OnReflectionUpdated()
{
#if defined(EDITOR)
	json jsonData;
	jsonData["Values"] = ReflectionUtils::ReflectiveDataToJson(GetReflectiveData());
	jsonData["Version"] = version;

	const bool saveResult = ReflectionUtils::JsonToFile(jsonData, file);
	if (!saveResult)
	{
		Debug::PrintError("[SkyBox::OnReflectionUpdated] Fail to save the Skybox file: " + file->GetPath(), true);
	}
#endif
}

void SkyBox::LoadFileReference()
{
	if (!isLoaded)
	{
		bool openResult = true;
#if defined(EDITOR)
		openResult = file->Open(FileMode::ReadOnly);
#endif
		if (openResult)
		{
			std::string jsonString;
#if defined(EDITOR)
			jsonString = file->ReadAll();
			file->Close();
#else
			unsigned char* binData = ProjectManager::fileDataBase.bitFile.ReadBinary(filePosition, fileSize);
			jsonString = std::string(reinterpret_cast<const char*>(binData), fileSize);
			free(binData);
#endif

			json j;
			try
			{
				j = json::parse(jsonString);
			}
			catch (const std::exception&)
			{
				Debug::PrintError("[ProjectManager::LoadFileReference] Failed to load the material file", true);
				return;
			}
			ReflectionUtils::JsonToReflectiveData(j, GetReflectiveData());

			isLoaded = true;
		}
		else 
		{
			Debug::PrintError("[SkyBox::LoadFileReference] Fail to load the skybox file: " + file->GetPath(), true);
		}
	}
}

std::shared_ptr<SkyBox> SkyBox::MakeSkyBox()
{
	std::shared_ptr<SkyBox> newFileRef = std::make_shared<SkyBox>();
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}
