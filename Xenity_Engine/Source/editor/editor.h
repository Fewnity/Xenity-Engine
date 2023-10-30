#pragma once

#include <memory>
#include "../engine/engine.h"
#include "../engine/game_elements/gameobject.h"
#include <json.hpp>
#include <unordered_map>

#include "../engine/file_system/file_type.h"

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
class CreateClassMenu;
class AudioSource;
class File;

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
	* Create an Empty GameObject and set his parent and his child
	*/
	static void CreateEmptyParent();

	/**
	* Create a new GameObject from another
	*/
	static void DuplicateGameObject(std::shared_ptr<GameObject> gameObject);

	static std::shared_ptr<File> CreateNewFile(std::string fileName, FileType type, bool fillWithDefaultData);

	/**
	* Add a component to the selected GameObject
	*/
	template <typename T>
	static std::shared_ptr<T> AddComponentToSelection()
	{
		if (auto selectedGO = selectedGameObject.lock())
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
	static CreateClassMenu* createClassMenu;
	static std::weak_ptr<AudioSource> audioSource;

	static void SetSelectedFileReference(std::shared_ptr<FileReference> fileReference);
	static std::shared_ptr<FileReference> GetSelectedFileReference();

	/**
* Set selected GameObject
* @param go New selected GameObject
*/
	static void SetSelectedGameObject(const std::weak_ptr<GameObject>& go);
	static std::weak_ptr<GameObject> GetSelectedGameObject();

private:
	static std::weak_ptr<GameObject> selectedGameObject;
	static std::shared_ptr<FileReference> selectedFileReference;
	static void CreateMenus();

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

