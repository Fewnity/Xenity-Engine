// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <memory>
#include <set>
#include <json.hpp>

#include <engine/api.h>

class Scene;
class Component;
class GameObject;

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

	/**
	* @brief Reload the current scene
	*/
	API static void ReloadScene();

	/**
	* @brief Get opened scene
	*/
	API static inline const std::shared_ptr<Scene>& GetOpenedScene()
	{
		return s_openedScene;
	}

#if defined(EDITOR)
	/**
	 * @brief Get if the scene has been modified
	 */
	API static inline bool GetSceneModified()
	{
		return s_sceneModified;
	}

	/**
	 * @brief [Internal]
	 */
	API static void SetSceneModified(bool value);
#endif

	static std::unordered_map<uint64_t, uint64_t> idRedirection;

private:
	friend class Engine;
	friend class Compiler;
	friend class ProjectManager;
	friend class GameplayManager;
	friend class Editor;
	friend class MainBarMenu;
	friend class Prefab;
	friend class SceneMenu;
	friend API std::shared_ptr<GameObject> Instantiate(const std::shared_ptr<Prefab>& prefab);


	/**
	* @brief [Internal] Create gameobjects and component from json data
	* @param jsonData Json data
	* @param createNewIds If true, create new ids for gameobjects and components (true used for prefabs)
	* @param rootGameObject If not null, get the root gameobject, the parent of all gameobjects (used for prefabs)
	*/
	static void CreateObjectsFromJson(const nlohmann::ordered_json& jsonData, bool createNewIds, std::shared_ptr<GameObject>* rootGameObject = nullptr);

#if defined(EDITOR)
	/**
	* @brief [Internal] Save scene
	* @param saveType If SaveSceneToFile, save scene as a file; If SaveSceneForPlayState/SaveSceneForHotReloading, save scene as a backup to reload it later
	*/
	static void SaveScene(SaveSceneType saveType);

	static nlohmann::ordered_json GameObjectToJson(GameObject& gameObject, std::set<uint64_t>& uniqueIds);
#endif
	static size_t FindSceneDataPosition(const std::string& jsonString);

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
	* @brief [Internal] Set opened scene
	*/
	static inline void SetOpenedScene(const std::shared_ptr<Scene>& _openedScene)
	{
		s_openedScene = _openedScene;
	}

	/**
	 * @brief [Internal] Show a dialog to ask if the user wants to save the scene if it has been modified
	 * @return True if canceled
	 */
	static bool OnQuit();

	/**
	* @brief [Internal] Load scene from json data
	*/
	static void LoadScene(const nlohmann::ordered_json& jsonData);

	static std::shared_ptr<Scene> s_openedScene;
	static bool s_sceneModified;
	static constexpr int s_sceneVersion = 1;
};

