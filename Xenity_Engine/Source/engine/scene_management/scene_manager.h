#pragma once

#include <vector>
#include <memory>
#include <json.hpp>

class Scene;
class Component;

class SceneManager
{
public:

	/**
	* Load a scene
	* @param scene Load scene
	*/
	static void LoadScene(Scene *scene);

	/**
	* [Internal] Save scene
	* @param saveForPlayState If false, save scene as a file; If true, save scene as a backup to reload it later
	*/
	static void SaveScene(bool saveForPlayState);

	/**
	* [Internal] Restore the saved scene backup
	*/
	static void RestoreScene();

	/**
	* [Internal] Clear scene
	*/
	static void EmptyScene();

	/**
	* [Internal] Get opened scene
	*/
	static Scene* GetOpenedScene() 
	{
		return openedScene;
	}

	/**
	* [Internal] Set opened scene
	*/
	static void SetOpenedScene(Scene* _openedScene)
	{
		openedScene = _openedScene;
	}

private:
	static Scene* openedScene;
	static void LoadScene(nlohmann::json jsonData);
	static std::vector<std::shared_ptr<Component>> allCreatedComponents;
	static int allCreatedComponentsCount;
};

