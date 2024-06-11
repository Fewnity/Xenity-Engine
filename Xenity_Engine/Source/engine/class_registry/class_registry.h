// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include <unordered_map>
#include <functional>
#include <string>
#include <memory>
#include <vector>

#include <engine/game_elements/gameobject.h>
#include <engine/debug/debug.h>
#include <engine/file_system/file_type.h>
#include <engine/assertions/assertions.h>
#if defined (EDITOR)
class Menu;
#endif
class Component;

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
		XASSERT(!name.empty(), "[ClassRegistry::AddComponentClass] name is empty")

		auto function = [](std::shared_ptr<GameObject> go)
		{
			return go->AddComponent<T>();
		};
		nameToComponent[name] = { function , isVisible };

		ClassInfo classInfo;
		classInfo.name = name;
		classInfo.typeId = typeid(T).hash_code();
		classInfos.push_back(classInfo);
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
		XASSERT(!name.empty(), "[ClassRegistry::AddComponentClass] name is empty")

		auto function = []()
		{
			return std::make_shared<T>();
		};
		nameToMenu[name] = { function , isVisible };

		MenuClassInfo classInfo;
		classInfo.name = name;
		classInfo.typeId = typeid(T).hash_code();
		menuClassInfos.push_back(classInfo);
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
	static std::shared_ptr<Component> AddComponentFromName(const std::string& name, const std::shared_ptr<GameObject>& gameObject);

	static std::shared_ptr<Menu> CreateMenuFromName(const std::string& name);

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
		XASSERT(!name.empty(), "[ClassRegistry::AddFileClass] name is empty")

		FileClassInfo fileClassInfo;
		fileClassInfo.name = name;
		fileClassInfo.typeId = typeid(T).hash_code();
		fileClassInfo.fileType = fileType;
		fileClassInfos.push_back(fileClassInfo);
	}

	/**
	* @brief Get a file class info from the class type
	*/
	template<typename T>
	std::enable_if_t<std::is_base_of<FileReference, T>::value, const FileClassInfo*>
	static GetFileClassInfo()
	{
		const uint64_t classId = typeid(T).hash_code();
		const size_t fileClassInfosCount = fileClassInfos.size();
		for (size_t i = 0; i < fileClassInfosCount; i++)
		{
			const FileClassInfo& info = fileClassInfos[i];
			if (classId == info.typeId)
			{
				return &info;
			}
		}

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
		const size_t classInfosCount = classInfos.size();
		for (size_t i = 0; i < classInfosCount; i++)
		{
			const ClassInfo& info = classInfos[i];
			if (classId == info.typeId)
			{
				return &info;
			}
		}

		return nullptr;
	}

	/**
	* @brief Get a class name from the class type id (hash code)
	* @param classId Class type id (hash code)
	* @return Class name (Component if not found)
	*/
	static std::string GetClassNameById(uint64_t classId)
	{
		const size_t classInfosCount = classInfos.size();
		for (size_t i = 0; i < classInfosCount; i++)
		{
			const ClassInfo& info = classInfos[i];
			if (classId == info.typeId)
			{
				return info.name;
			}
		}
		const size_t fileClassInfosCount = fileClassInfos.size();
		for (size_t i = 0; i < fileClassInfosCount; i++)
		{
			const FileClassInfo& info = fileClassInfos[i];
			if (classId == info.typeId)
			{
				return info.name;
			}
		}

		return "Component";
	}

private:

	static std::unordered_map <std::string, std::pair<std::function<std::shared_ptr<Component>(const std::shared_ptr<GameObject>&)>, bool>> nameToComponent;
	static std::vector<FileClassInfo> fileClassInfos;
	static std::vector<ClassInfo> classInfos;
#if defined(EDITOR)
	static std::unordered_map <std::string, std::pair<std::function<std::shared_ptr<Menu>()>, bool>> nameToMenu;
	static std::vector<MenuClassInfo> menuClassInfos;
#endif
};

