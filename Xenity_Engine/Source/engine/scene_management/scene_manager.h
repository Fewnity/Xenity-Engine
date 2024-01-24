#pragma once
#include <engine/api.h>

#include <vector>
#include <memory>
#include <json.hpp>

class Scene;
class Component;

enum class SaveSceneType
{
	SaveSceneToFile,
	SaveSceneForPlayState,
	SaveSceneForHotReloading,
};

class SceneManager
{
public:

	/**
	* Load a scene
	* @param scene Load scene
	*/
	API static void LoadScene(const std::shared_ptr<Scene>& scene);

#if defined(EDITOR)
	/**
	* [Internal] Save scene
	* @param saveType If SaveSceneToFile, save scene as a file; If SaveSceneForPlayState/SaveSceneForHotReloading, save scene as a backup to reload it later
	*/
	static void SaveScene(SaveSceneType saveType);
#endif

	static void ReloadScene();

	/**
	* [Internal] Restore the saved scene backup
	*/
	static void RestoreScene();

	/**
	* [Internal] Restore the saved scene backup for hot reloading
	*/
	static void RestoreSceneHotReloading();

	/**
	* [Internal] Clear scene
	*/
	static void ClearScene();

	/**
	* [Internal] Create empty scene
	*/
	static void CreateEmptyScene();

	/**
	* [Internal] Get opened scene
	*/
	static std::shared_ptr<Scene> GetOpenedScene()
	{
		return openedScene;
	}

	/**
	* [Internal] Set opened scene
	*/
	static void SetOpenedScene(const std::shared_ptr<Scene>& _openedScene)
	{
		openedScene = _openedScene;
	}

	/**
	 * [Internal]
	 */
	static void SetSceneModified(bool value);

	/**
	 * [Internal]
	 */
	static bool GetSceneModified() 
	{
		return sceneModified;
	}

	/**
	 * [Internal]
	 */
	static bool OnQuit();

private:
	static std::shared_ptr<Scene> openedScene;
	static void LoadScene(const nlohmann::json& jsonData);
	static bool sceneModified;
};

