#include "scene.h"
#include "../asset_management/asset_manager.h"

Scene::Scene()
{
}

ReflectiveData Scene::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

ReflectiveData Scene::GetMetaReflection()
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