#pragma once

#include <memory>
#include <json.hpp>
#include <unordered_map>

#include <engine/game_elements/gameobject.h>
#include <engine/file_system/file_type.h>
#include <editor/ui/menus/menu.h>

class Component;
class Reflective;
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
class ProjectDirectory;

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
	static void DuplicateGameObject(const std::shared_ptr<GameObject>& gameObject);

	static std::shared_ptr<File> CreateNewFile(const std::string& fileName, FileType type, bool fillWithDefaultData);

	static void OpenExplorerWindow(std::string path, bool isSelected);

	static void AddDragAndDrop(const std::string& path);

	static void OnDragAndDropFileFinished();

	static std::vector<std::string> dragdropEntries;

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

	template <typename T>
	static std::shared_ptr<T> GetMenu() 
	{
		for (int i = 0; i < menuCount; i++)
		{
			if (auto menu = std::dynamic_pointer_cast<T>(menus[i])) 
			{
				return menu;
			}
		}
		return nullptr;
	}

	template <typename T>
	static void RemoveMenu()
	{
		for (int i = 0; i < menuCount; i++)
		{
			if (auto menu = std::dynamic_pointer_cast<T>(menus[i]))
			{
				menus.erase(menus.begin() + i);
				menuCount--;
				break;
			}
		}
	}

	static void RemoveMenu(Menu* menu)
	{
		for (int i = 0; i < menuCount; i++)
		{
			if (menu == menus[i].get())
			{
				menus.erase(menus.begin() + i);
				menuCount--;
				break;
			}
		}
	}

	template <typename T>
	static std::shared_ptr<T> AddMenu(bool active)
	{
		int count = 0;
		for (int i = 0; i < menuCount; i++)
		{
			if (auto menu = std::dynamic_pointer_cast<T>(menus[i]))
			{
				count++;
			}
		}

		std::shared_ptr<T> newMenu = std::make_shared<T>();
		menus.push_back(newMenu);
		newMenu->id = count;
		newMenu->Init();
		newMenu->SetActive(active);
		menuCount++;
		return newMenu;
	}

	static MenuGroup currentMenu;
	static std::weak_ptr<AudioSource> audioSource;

	static void SetSelectedFileReference(const std::shared_ptr<FileReference>& fileReference);
	static std::shared_ptr<FileReference> GetSelectedFileReference();
	static void SetCurrentProjectDirectory(std::shared_ptr <ProjectDirectory> dir);
	static std::shared_ptr <ProjectDirectory> GetCurrentProjectDirectory();

	static void OnWindowFocused();

	/**
	* Set selected GameObject
	* @param go New selected GameObject
	*/
	static void SetSelectedGameObject(const std::shared_ptr<GameObject>& go);
	static std::shared_ptr<GameObject> GetSelectedGameObject();

	static std::shared_ptr <MeshData> rightArrow;
	static std::shared_ptr <MeshData> upArrow;
	static std::shared_ptr <MeshData> forwardArrow;
	static std::shared_ptr <MeshData> rotationCircle;
	
	static std::shared_ptr <Texture> toolArrowsTexture;
	static void StartFolderCopy(std::string path, std::string newPath);

	static std::string GetIncrementedGameObjectName(std::string name);
	static std::vector<std::shared_ptr<Menu>> menus;
	static void ApplyEditorStyle();
	static void RemoveEditorStyle();
	static std::weak_ptr <Menu> lastFocusedGameMenu;
	static bool isToolLocalMode;
private:
	static int menuCount;
	static std::shared_ptr <ProjectDirectory> currentProjectDirectory;
	static std::weak_ptr<GameObject> selectedGameObject;
	static std::shared_ptr<FileReference> selectedFileReference;

	static void CreateMenus();
	static void GetIncrementedGameObjectNameInfo(const std::string& name, std::string& baseName, int& number);

	static std::shared_ptr<MainBarMenu> mainBar;
};

