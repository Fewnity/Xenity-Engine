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

class Editor
{
public:
	static void Start();
	static void Update();
	static void Draw();
	static void CreateEmpty();
	static void CreateEmptyChild();
	static void SaveScene();
	static void DuplicateGameObject(std::shared_ptr<GameObject> gameObject);

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
	static CompilingMenu* compilingMenu;
	static GameMenu* gameMenu;
	static SceneMenu* sceneMenu;
private:
	static ProjectSettingsMenu* projectSettings;
	static EngineSettingsMenu* engineSettings;
	static FileExplorerMenu* fileExplorer;
	static HierarchyMenu* hierarchy;
	static InspectorMenu* inspector;
	static MainBarMenu* mainBar;
	static ProfilerMenu* profiler;
	static std::weak_ptr<GameObject> cameraGO;
};

