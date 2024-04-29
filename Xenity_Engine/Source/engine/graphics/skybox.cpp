#include "skybox.h"

#include <engine/reflection/reflection_utils.h>
#include <engine/debug/debug.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/file_system/file.h>

#include <json.hpp>

const int SkyBox::version = 1;

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

ReflectiveData SkyBox::GetMetaReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

void SkyBox::OnReflectionUpdated()
{
	json jsonData;
	jsonData["Values"] = ReflectionUtils::ReflectiveDataToJson(GetReflectiveData());
	jsonData["Version"] = version;

	const bool saveResult = ReflectionUtils::JsonToFile(jsonData, file);
	if (!saveResult)
	{
		Debug::PrintError("[SkyBox::OnReflectionUpdated] Fail to save the Skybox file: " + file->GetPath(), true);
	}
}

void SkyBox::LoadFileReference()
{
	const bool loadResult = ReflectionUtils::FileToReflectiveData(file, GetReflectiveData());
	if (!loadResult) 
	{
		Debug::PrintError("[SkyBox::LoadFileReference] Fail to load the skybox file: " + file->GetPath(), true);
	}
}

std::shared_ptr<SkyBox> SkyBox::MakeSkyBox()
{
	std::shared_ptr<SkyBox> newFileRef = std::make_shared<SkyBox>();
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}
