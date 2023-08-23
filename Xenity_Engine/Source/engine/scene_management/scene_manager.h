#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <vector>
#include <memory>
#include <json.hpp>

class Scene;
class Component;

enum SaveSceneType
{
	SaveSceneToFile,
	SaveSceneForPlayState,
	SaveSceneForHotReloading,
};

class API SceneManager
{
public:

	/**
	* Load a scene
	* @param scene Load scene
	*/
	static void LoadScene(Scene *scene);

#if defined(EDITOR)
	/**
	* [Internal] Save scene
	* @param saveForPlayState If false, save scene as a file; If true, save scene as a backup to reload it later
	*/
	static void SaveScene(SaveSceneType saveType);
#endif

	/**
	* [Internal] Restore the saved scene backup
	*/
	static void RestoreScene();
	static void RestoreSceneHotReloading();

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
};

