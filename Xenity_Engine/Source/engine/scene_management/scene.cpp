#include "scene.h"
#include <engine/asset_management/asset_manager.h>

Scene::Scene()
{
}

ReflectiveData Scene::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

ReflectiveData Scene::GetMetaReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

std::shared_ptr<Scene> Scene::MakeScene()
{
	std::shared_ptr<Scene> newFileRef = std::make_shared<Scene>();
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}