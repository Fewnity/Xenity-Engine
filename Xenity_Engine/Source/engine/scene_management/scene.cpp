#include "scene.h"
#include "../asset_management/asset_manager.h"

Scene::Scene()
{
}

std::unordered_map<std::string, Variable> Scene::GetReflection()
{
	std::unordered_map<std::string, Variable> reflectedVariables;
	reflectedVariables.insert_or_assign("fileId", fileId);
	return reflectedVariables;
}

std::unordered_map<std::string, Variable> Scene::GetMetaReflection()
{
	std::unordered_map<std::string, Variable> reflectedVariables;
	return reflectedVariables;
}

std::shared_ptr<Scene> Scene::MakeScene()
{
	std::shared_ptr<Scene> newFileRef = std::make_shared<Scene>();
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}