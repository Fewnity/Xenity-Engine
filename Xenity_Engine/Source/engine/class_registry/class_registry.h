// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <unordered_map>
#include <functional>
#include <string>
#include <memory>
#include <vector>

#include <engine/api.h>
#include <engine/game_elements/gameobject.h>
#include <engine/file_system/file_type.h>
#include <engine/assertions/assertions.h>
#include <engine/debug/debug.h>

#if defined (EDITOR)
class Menu;
#define REGISTER_MENU(menu) AddMenuClass<menu>(#menu)
#endif
class Component;

#define REGISTER_COMPONENT(component) ClassRegistry::AddComponentClass<component>(#component)
#define REGISTER_INVISIBLE_COMPONENT(component) ClassRegistry::AddComponentClass<component>(#component, false)

#define REGISTER_FILE(fileClass, fileType) AddFileClass<fileClass>(#fileClass, fileType)


class API ClassRegistry
{
public:
	struct FileClassInfo
	{
		std::string name = "";
		uint64_t typeId = 0;
		FileType fileType = FileType::File_Other;
	};

	struct ClassInfo
	{
		std::string name = "";
		uint64_t typeId = 0;
	};

#if defined (EDITOR)
	struct MenuClassInfo
	{
		std::string name = "";
		uint64_t typeId = 0;
	};
#endif

	/**
	* @brief Add a function to create a component
	* @param name Component name
	* @param isVisible Is the component visible in the editor
	*/
	template<typename T>
	std::enable_if_t<std::is_base_of<Component, T>::value, void>
	static AddComponentClass(const std::string& name, bool isVisible = true)
	{
		XASSERT(!name.empty(), "[ClassRegistry::AddComponentClass] name is empty");

		auto function = [](GameObject& go)
		{
			return go.AddComponent<T>();
		};
		s_nameToComponent[name] = { function , isVisible };

		ClassInfo classInfo;
		classInfo.name = name;
		classInfo.typeId = typeid(T).hash_code();
		s_classInfos.push_back(classInfo);
	}

#if defined (EDITOR)
	/**
	* @brief Add a function to create a component
	* @param name Component name
	* @param isVisible Is the component visible in the editor
	*/
	template<typename T>
	std::enable_if_t<std::is_base_of<Menu, T>::value, void>
	static AddMenuClass(const std::string& name, bool isVisible = true)
	{
		XASSERT(!name.empty(), "[ClassRegistry::AddComponentClass] name is empty");

		auto function = []()
		{
			return std::make_shared<T>();
		};
		s_nameToMenu[name] = { function , isVisible };

		MenuClassInfo classInfo;
		classInfo.name = name;
		classInfo.typeId = typeid(T).hash_code();
		s_menuClassInfos.push_back(classInfo);
	}
#endif

	/**
	* @brief Register all engine components
	*/
	static void RegisterEngineComponents();

	/**
	* @brief Register all engine file classes
	*/
	static void RegisterEngineFileClasses();

#if defined (EDITOR)
	/**
	* @brief Register all editor menus
	*/
	static void RegisterMenus();
#endif

	/**
	* @brief Add a component to a GameObject from the component name
	* @param name Component name
	* @param gameObject GameObject to add the component to
	*/
	static std::shared_ptr<Component> AddComponentFromName(const std::string& name, GameObject& gameObject);
#if defined (EDITOR)
	static std::shared_ptr<Menu> CreateMenuFromName(const std::string& name);
#endif
	/**
	* @brief Get a list of all component names
	*/
	static std::vector<std::string> GetComponentNames();

	/**
	* @brief Reset all registered components
	*/
	static void Reset();

	/**
	* @brief Add a file class info into the list
	* @param name File class name
	* @param fileType File type
	*/
	template<typename T>
	std::enable_if_t<std::is_base_of<FileReference, T>::value, void>
	static AddFileClass(const std::string& name, const FileType fileType)
	{
		XASSERT(!name.empty(), "[ClassRegistry::AddFileClass] name is empty");

		FileClassInfo fileClassInfo;
		fileClassInfo.name = name;
		fileClassInfo.typeId = typeid(T).hash_code();
		fileClassInfo.fileType = fileType;
		s_fileClassInfos.push_back(fileClassInfo);
	}

	/**
	* @brief Get a file class info from the class type
	*/
	template<typename T>
	std::enable_if_t<std::is_base_of<FileReference, T>::value, const FileClassInfo*>
	static GetFileClassInfo()
	{
		const uint64_t classId = typeid(T).hash_code();
		const size_t fileClassInfosCount = s_fileClassInfos.size();
		for (size_t i = 0; i < fileClassInfosCount; i++)
		{
			const FileClassInfo& info = s_fileClassInfos[i];
			if (classId == info.typeId)
			{
				return &info;
			}
		}

		XASSERT(false, "[ClassRegistry::GetFileClassInfo] FileClassInfo not found");
		return nullptr;
	}

	/**
	* @brief Get a class info from the class type
	*/
	template<typename T>
	std::enable_if_t<std::is_base_of<Component, T>::value, const ClassInfo*>
	static GetClassInfo()
	{
		const uint64_t classId = typeid(T).hash_code();
		const size_t classInfosCount = s_classInfos.size();
		for (size_t i = 0; i < classInfosCount; i++)
		{
			const ClassInfo& info = s_classInfos[i];
			if (classId == info.typeId)
			{
				return &info;
			}
		}

		XASSERT(false, "[ClassRegistry::GetClassInfo] ClassInfo not found");
		return nullptr;
	}

	/**
	* @brief Get a class name from the class type id (hash code)
	* @param classId Class type id (hash code)
	* @return Class name (Component if not found)
	*/
	static const std::string& GetClassNameById(uint64_t classId)
	{
		const size_t classInfosCount = s_classInfos.size();
		for (size_t i = 0; i < classInfosCount; i++)
		{
			const ClassInfo& info = s_classInfos[i];
			if (classId == info.typeId)
			{
				return info.name;
			}
		}
		const size_t fileClassInfosCount = s_fileClassInfos.size();
		for (size_t i = 0; i < fileClassInfosCount; i++)
		{
			const FileClassInfo& info = s_fileClassInfos[i];
			if (classId == info.typeId)
			{
				return info.name;
			}
		}

		return s_classInfos[0].name;
	}

	static const FileClassInfo* GetFileClassInfoById(uint64_t classId)
	{
		const size_t fileClassInfosCount = s_fileClassInfos.size();
		for (size_t i = 0; i < fileClassInfosCount; i++)
		{
			const FileClassInfo& info = s_fileClassInfos[i];
			if (classId == info.typeId)
			{
				return &info;
			}
		}

		return nullptr;
	}

	static size_t GetClassInfosCount()
	{
		return s_classInfos.size();
	}

	static size_t GetFileClassInfosCount()
	{
		return s_fileClassInfos.size();
	}

#if defined(EDITOR)
	static size_t GetMenuClassInfosCount()
	{
		return s_menuClassInfos.size();
	}
#endif

private:

	static std::unordered_map <std::string, std::pair<std::function<std::shared_ptr<Component>(GameObject&)>, bool>> s_nameToComponent;
	static std::vector<FileClassInfo> s_fileClassInfos;
	static std::vector<ClassInfo> s_classInfos;
#if defined(EDITOR)
	static std::unordered_map <std::string, std::pair<std::function<std::shared_ptr<Menu>()>, bool>> s_nameToMenu;
	static std::vector<MenuClassInfo> s_menuClassInfos;
#endif
};

