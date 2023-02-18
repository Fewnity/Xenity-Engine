#include "SceneManager.h"
#include "../debug/debug.h"

#include "scene.h"

std::vector<Scene*> SceneManager::loadedScenes;

void SceneManager::LoadScene(Scene* scene) 
{
	UnloadScenes();
	scene->Load();
}

void SceneManager::LoadSceneAdditive(Scene* scene)
{
	Debug::Print("Not Implemented: SceneManager::LoadSceneAdditive");
}

void SceneManager::UnloadScene(Scene* scene)
{
	Debug::Print("Not Implemented: SceneManager::UnloadScene");
}

void SceneManager::UnloadScenes() 
{
	int loadedScenesCount = loadedScenes.size();
	for (int i = 0; i < loadedScenesCount; i++)
	{
		delete loadedScenes[i];
	}
	loadedScenes.clear();
}
