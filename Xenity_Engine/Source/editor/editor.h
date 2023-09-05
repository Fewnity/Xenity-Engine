#pragma once

#include <memory>
#include "../engine/engine.h"
#include "../engine/game_elements/gameobject.h"
#include <json.hpp>
#include <unordered_map>

class Component;
class Reflection;
class FileReference;
class ProjectSettingsMenu;
class EngineSettingsMenu;
class FileExplorerMenu;
class HierarchyMenu;
class InspectorMenu;
class MainBarMenu;
class ProfilerMenu;
class GameMenu;
class SceneMenu;
class CompilingMenu;
class SelectProjectMenu;
class CreateProjectMenu;
class LightingMenu;

enum MenuNames
{
	Menu_Select_Project,
	Menu_Create_Project,
	Menu_Editor
};

class Editor
{
public:

	/**
	* Init editor (Create menus, camera...)
	*/
	static void Init();

	/**
	* Update editor (to call each frame)
	*/
	static void Update();

	/**
	* Draw editor menus
	*/
	static void Draw();

	/**
	* Create an empty GameObject
	*/
	static void CreateEmpty();

	/**
	* Create an Empty GameObject and set his parent
	*/
	static void CreateEmptyChild();

	/**
	* Create a new GameObject from another
	*/
	static void DuplicateGameObject(std::shared_ptr<GameObject> gameObject);

	/**
	* Add a component to the selected GameObject
	*/
	template <typename T>
	static std::shared_ptr<T> AddComponentToSelection()
	{
		if (auto selectedGO = Engine::selectedGameObject.lock())
		{
			return selectedGO->AddComponent<T>();
		}
		else
		{
			return nullptr;
		}
	}

	static MenuNames currentMenu;
	static CompilingMenu* compilingMenu;
	static GameMenu* gameMenu;
	static SceneMenu* sceneMenu;
	static InspectorMenu* inspector;
	static LightingMenu* lightingMenu;
private:
	static ProjectSettingsMenu* projectSettings;
	static EngineSettingsMenu* engineSettings;
	static FileExplorerMenu* fileExplorer;
	static HierarchyMenu* hierarchy;
	static MainBarMenu* mainBar;
	static ProfilerMenu* profiler;
	static SelectProjectMenu* selectProjectMenu;
	static CreateProjectMenu* createProjectMenu;
	static std::weak_ptr<GameObject> cameraGO;
};

