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
	this->front = std::move(front);
	this->back = std::move(back);
	this->up = std::move(up);
	this->down = std::move(down);
	this->left = std::move(left);
	this->right = std::move(right);
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
	bool loadResult = ReflectionUtils::ReflectiveDataToFile(GetReflectiveData(), file);
	if (!loadResult)
	{
		Debug::PrintError("[SkyBox::OnReflectionUpdated] Fail to save the Skybox file: " + file->GetPath());
	}
}

void SkyBox::LoadFileReference()
{
	bool loadResult = ReflectionUtils::FileToReflectiveData(file, GetReflectiveData());
	if (!loadResult) 
	{
		Debug::PrintError("[SkyBox::LoadFileReference] Fail to load the skybox file: " + file->GetPath());
	}
}

std::shared_ptr<SkyBox> SkyBox::MakeSkyBox()
{
	std::shared_ptr<SkyBox> newFileRef = std::make_shared<SkyBox>();
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}
