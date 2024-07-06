// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

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

/**
* @brief Class to load and save scenes
*/
class SceneManager
{
public:

	/**
	* @brief Load a scene
	* @param scene Scene to load
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
	static inline const std::shared_ptr<Scene>& GetOpenedScene()
	{
		return openedScene;
	}

	/**
	* @brief [Internal] Set opened scene
	*/
	static inline void SetOpenedScene(const std::shared_ptr<Scene>& _openedScene)
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
	static inline bool GetSceneModified()
	{
		return sceneModified;
	}

	/**
	 * @brief [Internal] Show a dialog to ask if the user wants to save the scene if it has been modified
	 * @return True if canceled
	 */
	static bool OnQuit();

private:

	/**
	* @brief [Internal] Load scene from json data
	*/
	static void LoadScene(const nlohmann::ordered_json& jsonData);

	static std::shared_ptr<Scene> openedScene;
	static bool sceneModified;
	static constexpr int sceneVersion = 1;
};

