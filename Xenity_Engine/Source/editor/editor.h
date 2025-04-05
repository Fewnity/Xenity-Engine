// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

#include <memory>

#include <editor/ui/menus/menu.h>

#include <engine/game_elements/gameobject.h>
#include <engine/file_system/file_type.h>
#include <engine/assertions/assertions.h>
#include <engine/class_registry/class_registry.h>

class Component;
class Reflective;
class FileReference;
class ProjectSettingsMenu;
class EngineSettingsMenu;
class BottomBarMenu;
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
class MeshData;
class Texture;

template<typename... Args>
class Event;

class Editor
{
public:

	/**
	* @brief Init editor (Create menus, camera...)
	*/
	static void Init();

	static void Stop();

	/**
	* @brief Update editor (to call each frame)
	*/
	static void Update();

	/**
	* @brief Draw editor menus
	*/
	static void Draw();

	/**
	* @brief Create an empty GameObject
	*/
	static void CreateEmpty();

	/**
	* @brief Create an Empty GameObject and set his parent
	*/
	static void CreateEmptyChild();

	/**
	* @brief Create an Empty GameObject and set his parent and his child
	*/
	static void CreateEmptyParent();

	/**
	* @brief Create a new file with the given name and type
	* @param fileName Name of the file
	* @param type Type of the file
	* @param fillWithDefaultData Fill the file with default data
	* @return The created file
	*/
	[[nodiscard]] static std::shared_ptr<File> CreateNewFile(const std::string& fileName, FileType type, bool fillWithDefaultData);

	/**
	* @brief Open a file in the file explorer of the OS
	* @param path Path of the file
	* @param isSelected If the file should be selected in the file explorer
	*/
	static void OpenExplorerWindow(const std::string& path, bool isSelected);

	/**
	* @brief Add a file into a wait list from drag and drop
	* @param path Path of the file
	*/
	static void AddDragAndDrop(const std::string& path);

	/**
	* @brief Called when all files were dropped in the editor and copy them into the project
	*/
	static void OnDragAndDropFileFinished();

	/**
	* @brief Check if a GameObject is a parent of another GameObject (recursive)
	* @param parent Parent GameObject
	* @param child Child GameObject
	* @return True if the parent is a parent of the child
	*/
	[[nodiscard]] static bool IsParentOf(const std::shared_ptr<GameObject>& parent, const std::shared_ptr<GameObject>& child);

	/**
	* @brief Remove all children and keep parents of a list with a mix of parents and children
	* @param parentsAndChildren List of GameObjects
	*/
	[[nodiscard]] static std::vector<std::shared_ptr<GameObject>> RemoveChildren(std::vector<std::shared_ptr<GameObject>> parentsAndChildren);

	/**
	* @brief Get menu of type T
	* @return Menu of type T or nullptr if not found
	*/
	template <typename T>
	[[nodiscard]] static std::shared_ptr<T> GetMenu(bool createIfNotFound = true)
	{
		for (int i = 0; i < menuCount; i++)
		{
			if (auto menu = std::dynamic_pointer_cast<T>(menus[i]))
			{
				return menu;
			}
		}
		if (createIfNotFound)
		{
			return AddMenu<T>(true);
		}
		return nullptr;
	}

	/**
	* @brief Get all menus of type T
	* @return List of menus of type T or empty list if nothing found
	*/
	template <typename T>
	[[nodiscard]] static std::vector<std::shared_ptr<T>> GetMenus()
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
	* @brief Remove a menu of type T
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
	* @brief Remove a menu
	*/
	static void RemoveMenu(Menu* menu)
	{
		XASSERT(menu != nullptr, "[Editor::RemoveMenu] menu is nullptr");

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
	* @brief Add a menu of type T
	* @param active If the menu should be active
	* @return The created menu
	*/
	template <typename T>
	[[nodiscard]] static std::shared_ptr<T> AddMenu(bool active)
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

	static void OnMenuActiveStateChange(const std::string& menuName, bool active, int id)
	{
		const size_t menuSize = menuSettings.settings.size();
		for (size_t i = 0; i < menuSize; i++)
		{
			MenuSetting& setting = *menuSettings.settings[i];
			if (setting.name == menuName && setting.id == id)
			{
				setting.isActive = active;
				break;
			}
		}
		SaveMenuSettings();
	}

	static std::shared_ptr<Menu> AddMenu(const std::string& menuName, bool active, int id = -1)
	{
		// Make a list of all ids and use the smallest one that is not used
		std::vector<int> usedIds;
		int idCount = 0;
		if (id == -1)
		{
			//int count = 0;
			int highestId = 0;
			for (int i = 0; i < menuCount; i++)
			{
				if (menus[i]->name == menuName)
				{
					idCount++;
					usedIds.push_back(menus[i]->id);
					if (menus[i]->id > highestId)
					{
						highestId = menus[i]->id;
					}
				}
			}
			//id = count;
			if (idCount != 0)
			{
				int lowestId = highestId+1;
				for (int i = 0; i < idCount; i++)
				{
					bool isUsed = false;
					for (int j = 0; j < idCount; j++)
					{
						if (usedIds[j] == i)
						{
							isUsed = true;
							break;
						}
					}
					if (!isUsed && i < lowestId)
					{
						lowestId = i;
					}
				}
				id = lowestId;
				//lowestId = usedIds[0] + 1;
			}
			else
			{
				id = 0;
			}
		}

		std::shared_ptr<Menu> menu = ClassRegistry::CreateMenuFromName(menuName);
		menus.push_back(menu);

		menu->id = id;
		menu->name = menuName;
		menu->Init();
		menu->SetActive(active);
		menuCount++;

		bool menuSettingFound = false;
		const size_t menuSize = menuSettings.settings.size();
		MenuSetting* settingToUse = nullptr;
		for (size_t i = 0; i < menuSize; i++)
		{
			MenuSetting& setting = *menuSettings.settings[i];
			if (setting.name == menuName)
			{
				settingToUse = &setting;
				if (setting.id == id)
				{
					setting.isActive = active;
					menuSettingFound = true;
					break;
				}
			}
		}
		if (!menuSettingFound && settingToUse)
		{
			UpdateOrAddMenuSetting(menuSettings.settings, menuName, active, settingToUse->isUnique, id);
		}
		SaveMenuSettings();

		return menu;
	}

	static MenuGroup currentMenu;
	static std::weak_ptr<AudioSource> audioSource;

	/**
	* @brief Set selected file reference
	* @param fileReference New selected file reference
	*/
	static void SetSelectedFileReference(const std::shared_ptr<FileReference>& fileReference);

	/**
	* @brief Get selected file reference
	*/
	[[nodiscard]] static std::shared_ptr<FileReference> GetSelectedFileReference();

	/**
	* @brief Set current project directory
	* @param dir New current project directory
	*/
	static void SetCurrentProjectDirectory(const std::shared_ptr <ProjectDirectory>& dir);

	/**
	* @brief Get current project directory
	* @return Current project directory
	*/
	[[nodiscard]] static std::shared_ptr <ProjectDirectory> GetCurrentProjectDirectory();

	/**
	* @brief Function called when the window is focused
	*/
	static void OnWindowFocused();

	/**
	* @brief Set selected GameObject
	* @param go New selected GameObject
	*/
	static void SetSelectedGameObject(const std::shared_ptr<GameObject>& go);

	/**
	* @brief Clear selected GameObjects
	*/
	static void ClearSelectedGameObjects();

	/**
	* @brief Add a GameObject to the selected GameObjects list
	* @param gameObjectToAdd GameObject to add
	*/
	static void AddSelectedGameObject(const std::shared_ptr<GameObject>& gameObjectToAdd);

	/**
	* @brief Remove a GameObject from the selected GameObjects list
	* @param gameObjectToRemove GameObject to remove
	*/
	static void RemoveSelectedGameObject(const std::shared_ptr<GameObject>& gameObjectToRemove);

	/**
	* @brief Check if a GameObject is in the selected GameObjects list
	* @param gameObjectToCheck GameObject to check
	* @return True if the GameObject is in the selected GameObjects list
	*/
	static bool IsInSelectedGameObjects(const std::shared_ptr<GameObject>& gameObjectToCheck);

	/**
	* @brief Get selected GameObjects
	*/
	static const std::vector<std::weak_ptr<GameObject>>& GetSelectedGameObjects();

	/**
	* @brief Start folder copy
	*/
	static void StartFolderCopy(const std::string& path, const std::string& newPath);

	/**
	* @brief Get the name of a GameObject incremented by the number of GameObjects with the same name
	*/
	[[nodiscard]] static std::string GetIncrementedGameObjectName(const std::string& name);

	/**
	* @brief Apply editor style to all next ImGui calls
	*/
	static void ApplyEditorStyle();

	/**
	* @brief Remove editor style for all next ImGui calls
	*/
	static void RemoveEditorStyle();

	/**
	* @brief Open a link in the web browser of the OS
	*/
	static void OpenLinkInWebBrowser(const std::string& link);

	/**
	* @brief Start a executable file
	* @return True on success
	*/
	static bool OpenExecutableFile(const std::string& executablePath);

	/**
	 * 
	 */
	static int ExecuteSystemCommand(const std::string& command, std::string& outputText);

	/**
	* @brief Get folder path and file name from a full path
	* @param fullPath Full path
	* @param folderPath Folder path
	* @param fileName File name
	*/
	static bool SeparateFileFromPath(const std::string& fullPath, std::string& folderPath, std::string& fileName);

	static bool IsUpdateAvailable() 
	{
		return updateAvailable;
	}

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
	static float cameraSpeed;

private:
	class MenuSetting : public Reflective
	{
	public:
		[[nodiscard]] ReflectiveData GetReflectiveData() override
		{
			ReflectiveData reflectedVariables;
			Reflective::AddVariable(reflectedVariables, name, "name", true);
			Reflective::AddVariable(reflectedVariables, id, "id", true);
			Reflective::AddVariable(reflectedVariables, isActive, "isActive", true);
			Reflective::AddVariable(reflectedVariables, isUnique, "isUnique", true);
			return reflectedVariables;
		}

		std::string name = "";
		int id = 0;
		bool isActive = true;
		bool isUnique = false;
	};

	class MenuSettings : public Reflective
	{
	public:
		[[nodiscard]] ReflectiveData GetReflectiveData() override
		{
			ReflectiveData reflectedVariables;
			Reflective::AddVariable(reflectedVariables, settings, "settings", true);
			Reflective::AddVariable(reflectedVariables, version, "version", true);
			return reflectedVariables;
		}

		std::vector<MenuSetting*> settings;
		int version = 1;
	};
	static MenuSettings menuSettings;

	[[nodiscard]] static bool CheckIntegrity();
	static void CheckItemIntegrity(const std::string& itemPath, bool& success);
	static void SaveMenuSettings();
	static void LoadMenuSettings();
	static void CreateNewMenuSettings();
	static MenuSetting* AddMenuSetting(std::vector<MenuSetting*>& menuSettingList, const std::string& name, bool isActive, bool isUnique, int id);
	static MenuSetting* UpdateOrAddMenuSetting(std::vector<MenuSetting*>& menuSettingList, const std::string& name, bool isActive, bool isUnique, int id);

	static void OnFileModified();
	static void OnCodeModified();

	/**
	* @brief Create all menus
	*/
	static void CreateMenus();

	/**
	* @brief Get info from a GameObject name with a number at the end (or not)
	* @param name Name of the GameObject
	* @param baseName Base name of the GameObject without the number
	* @param number New number at the end of the GameObject name
	*/
	static void GetIncrementedGameObjectNameInfo(const std::string& name, std::string& baseName, int& number);

	static void OnUpdateChecked(bool newVersionAvailable);

	static bool updateAvailable;
	static int menuCount;
	static std::shared_ptr <ProjectDirectory> currentProjectDirectory;
	static std::vector<std::weak_ptr<GameObject>> selectedGameObjects;
	static std::shared_ptr<FileReference> selectedFileReference;

	static std::shared_ptr<MainBarMenu> mainBar;
	static std::shared_ptr <BottomBarMenu> bottomBar;
	static std::vector<std::string> dragdropEntries;
	static Event<bool>* onUpdateCheckedEvent;
	static bool needProjectDiretoryUpdate;
};

