#include "skybox.h"
#include "../asset_management/asset_manager.h"
#include <json.hpp>
#include "../reflection/reflection_utils.h"
#include "../debug/debug.h"
#include "../file_system/file_system.h"

using json = nlohmann::json;

SkyBox::SkyBox()
{
	AssetManager::AddReflection(this);
}

SkyBox::~SkyBox()
{
	AssetManager::RemoveReflection(this);
}

SkyBox::SkyBox(std::shared_ptr<Texture> front, std::shared_ptr<Texture>back, std::shared_ptr<Texture> up, std::shared_ptr<Texture>down, std::shared_ptr<Texture> left, std::shared_ptr<Texture> right)
{
	this->front = front;
	this->back = back;
	this->up = up;
	this->down = down;
	this->left = left;
	this->right = right;
}

std::unordered_map<std::string, ReflectionEntry> SkyBox::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	AddReflectionVariable(reflectedVariables, front, "front", true, true);
	AddReflectionVariable(reflectedVariables, back, "back", true, true);
	AddReflectionVariable(reflectedVariables, up, "up", true, true);
	AddReflectionVariable(reflectedVariables, down, "down", true, true);
	AddReflectionVariable(reflectedVariables, left, "left", true, true);
	AddReflectionVariable(reflectedVariables, right, "right", true, true);
	return reflectedVariables;
}

std::unordered_map<std::string, ReflectionEntry> SkyBox::GetMetaReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	return reflectedVariables;
}

void SkyBox::OnReflectionUpdated() 
{
	json myJson;
	myJson["Values"] = ReflectionUtils::MapToJson(GetReflection());
	FileSystem::fileSystem->DeleteFile(file->GetPath());
	file->Open(true);
	file->Write(myJson.dump(0));
	file->Close();
	Debug::Print(myJson.dump(3));
}

void SkyBox::LoadFileReference()
{
	json myJson;
	file->Open(true);
	std::string jsonString = file->ReadAll();
	file->Close();
	if (jsonString != "")
	{
		myJson = json::parse(jsonString);
		ReflectionUtils::JsonToMap(myJson, GetReflection());
	}
}

std::shared_ptr<SkyBox> SkyBox::MakeSkyBox()
{
	std::shared_ptr<SkyBox> newFileRef = std::make_shared<SkyBox>();
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}
