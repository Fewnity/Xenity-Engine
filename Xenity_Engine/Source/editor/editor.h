#pragma once

/**
 * [Internal]
 */

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
class ConsoleMenu;
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
	* Create a new file with the given name and type
	* @param fileName Name of the file
	* @param type Type of the file
	* @param fillWithDefaultData Fill the file with default data
	* @return The created file
	*/
	static std::shared_ptr<File> CreateNewFile(const std::string& fileName, FileType type, bool fillWithDefaultData);

	/**
	* Open a file in the file explorer of the OS
	* @param path Path of the file
	* @param isSelected If the file should be selected in the file explorer
	*/
	static void OpenExplorerWindow(std::string path, bool isSelected);

	/**
	* Add a file into a wait list from drag and drop
	* @param path Path of the file
	*/
	static void AddDragAndDrop(const std::string& path);

	/**
	* Called when all files were dropped in the editor and copy them into the project
	*/
	static void OnDragAndDropFileFinished();

	/**
	* Check if a GameObject is a parent of another GameObject (recursive)
	* @param parent Parent GameObject
	* @param child Child GameObject
	* @return True if the parent is a parent of the child
	*/
	static bool IsParentOf(const std::shared_ptr<GameObject>& parent, const std::shared_ptr<GameObject>& child);

	/**
	* Remove all children and keep parents of a list with a mix of parents and children
	* @param parentsAndChildren List of GameObjects
	*/
	static std::vector<std::shared_ptr<GameObject>> RemoveChildren(const std::vector<std::shared_ptr<GameObject>> parentsAndChildren);

	/**
	* Get menu of type T
	* @return Menu of type T or nullptr if not found
	*/
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

	/**
	* Get all menus of type T
	* @return List of menus of type T or empty list if nothing found
	*/
	template <typename T>
	static std::vector<std::shared_ptr<T>> GetMenus()
	{
		std::vector<std::shared_ptr<T>> menusListT;
		for (int i = 0; i < menuCount; i++)
		{
			if (auto menu = std::dynamic_pointer_cast<T>(menus[i]))
			{
				menusListT.push_back(menu);
			}
		}
		return menusListT;
	}

	/**
	* Remove a menu of type T
	*/
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

	/**
	* Remove a menu
	*/
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

	/**
	* Add a menu of type T
	* @param active If the menu should be active
	* @return The created menu
	*/
	template <typename T>
	static std::shared_ptr<T> AddMenu(bool active)
	{
		int count = 0;
		for (int i = 0; i < menuCount; i++)
		{
			if (std::shared_ptr<T> menu = std::dynamic_pointer_cast<T>(menus[i]))
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

	/**
	* Set selected file reference
	* @param fileReference New selected file reference
	*/
	static void SetSelectedFileReference(const std::shared_ptr<FileReference>& fileReference);

	/**
	* Get selected file reference
	*/
	static std::shared_ptr<FileReference> GetSelectedFileReference();

	/**
	* Set current project directory
	* @param dir New current project directory
	*/
	static void SetCurrentProjectDirectory(std::shared_ptr <ProjectDirectory> dir);

	/**
	* Get current project directory
	* @return Current project directory
	*/
	static std::shared_ptr <ProjectDirectory> GetCurrentProjectDirectory();

	/**
	* Function called when the window is focused
	*/
	static void OnWindowFocused();

	/**
	* Set selected GameObject
	* @param go New selected GameObject
	*/
	static void SetSelectedGameObject(const std::shared_ptr<GameObject>& go);

	/**
	* Clear selected GameObjects
	*/
	static void ClearSelectedGameObjects();

	/**
	* Add a GameObject to the selected GameObjects list
	* @param gameObjectToAdd GameObject to add
	*/
	static void AddSelectedGameObject(const std::shared_ptr<GameObject>& gameObjectToAdd);

	/**
	* Remove a GameObject from the selected GameObjects list
	* @param gameObjectToRemove GameObject to remove
	*/
	static void RemoveSelectedGameObject(const std::shared_ptr<GameObject>& gameObjectToRemove);

	/**
	* Check if a GameObject is in the selected GameObjects list
	* @param gameObjectToCheck GameObject to check
	* @return True if the GameObject is in the selected GameObjects list
	*/
	static bool IsInSelectedGameObjects(const std::shared_ptr<GameObject>& gameObjectToCheck);

	/**
	* Get selected GameObjects
	*/
	static std::vector<std::weak_ptr<GameObject>> GetSelectedGameObjects();

	/**
	* Start folder copy
	*/
	static void StartFolderCopy(const std::string& path, const std::string& newPath);

	/**
	* Get the name of a GameObject incremented by the number of GameObjects with the same name
	*/
	static std::string GetIncrementedGameObjectName(std::string name);

	/**
	* Apply editor style to all next ImGui calls
	*/
	static void ApplyEditorStyle();

	/**
	* Remove editor style for all next ImGui calls
	*/
	static void RemoveEditorStyle();

	/**
	* Open a link in the web browser of the OS
	*/
	static void OpenLinkInWebBrowser(const std::string& link);

	/**
	* Start a executable file
	* @return True on success
	*/
	static bool OpenExecutableFile(const std::string& executablePath);

	/**
	* Get folder path and file name from a full path
	* @param fullPath Full path
	* @param folderPath Folder path
	* @param fileName File name
	*/
	static bool SeparateFileFromPath(const std::string& fullPath, std::string& folderPath, std::string& fileName);

	static std::shared_ptr <MeshData> rightArrow;
	static std::shared_ptr <MeshData> upArrow;
	static std::shared_ptr <MeshData> forwardArrow;
	static std::shared_ptr <MeshData> rotationCircleX;
	static std::shared_ptr <MeshData> rotationCircleY;
	static std::shared_ptr <MeshData> rotationCircleZ;
	static std::shared_ptr <Texture> toolArrowsTexture;

	static std::vector<std::shared_ptr<Menu>> menus;
	static std::weak_ptr <Menu> lastFocusedGameMenu;
	static bool isToolLocalMode;


private:

	/**
	* Create all menus
	*/
	static void CreateMenus();

	/**
	* Get info from a GameObject name with a number at the end (or not)
	* @param name Name of the GameObject
	* @param baseName Base name of the GameObject without the number
	* @param number New number at the end of the GameObject name 
	*/
	static void GetIncrementedGameObjectNameInfo(const std::string& name, std::string& baseName, int& number);

	static int menuCount;
	static std::shared_ptr <ProjectDirectory> currentProjectDirectory;
	static std::vector<std::weak_ptr<GameObject>> selectedGameObjects;
	static std::shared_ptr<FileReference> selectedFileReference;

	static std::shared_ptr<MainBarMenu> mainBar;
	static std::vector<std::string> dragdropEntries;
};

