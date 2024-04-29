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
	* @brief Load a scene
	* @param scene Load scene
	*/
	API static void LoadScene(const std::shared_ptr<Scene>& scene);

#if defined(EDITOR)
	/**
	* @brief [Internal] Save scene
	* @param saveType If SaveSceneToFile, save scene as a file; If SaveSceneForPlayState/SaveSceneForHotReloading, save scene as a backup to reload it later
	*/
	static void SaveScene(SaveSceneType saveType);
#endif

	static void ReloadScene();

	/**
	* @brief [Internal] Restore the saved scene backup
	*/
	static void RestoreScene();

	/**
	* @brief [Internal] Restore the saved scene backup for hot reloading
	*/
	static void RestoreSceneHotReloading();

	/**
	* @brief [Internal] Clear scene
	*/
	static void ClearScene();

	/**
	* @brief [Internal] Create empty scene
	*/
	static void CreateEmptyScene();

	/**
	* @brief [Internal] Get opened scene
	*/
	static std::shared_ptr<Scene> GetOpenedScene()
	{
		return openedScene;
	}

	/**
	* @brief [Internal] Set opened scene
	*/
	static void SetOpenedScene(const std::shared_ptr<Scene>& _openedScene)
	{
		openedScene = _openedScene;
	}

	/**
	 * @brief [Internal]
	 */
	static void SetSceneModified(bool value);

	/**
	 * @brief [Internal]
	 */
	static bool GetSceneModified() 
	{
		return sceneModified;
	}

	/**
	 * @brief [Internal]
	 */
	static bool OnQuit();

private:

	/**
	* @brief [Internal] Load scene from json data
	*/
	static void LoadScene(const nlohmann::json& jsonData);

	static std::shared_ptr<Scene> openedScene;
	static bool sceneModified;
	static const int sceneVersion;
};

