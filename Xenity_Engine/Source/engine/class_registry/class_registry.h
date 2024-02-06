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

class Component;



class API ClassRegistry
{
public:
	struct FileClassInfo
	{
		std::string name;
		uint64_t typeId;
		FileType fileType = FileType::File_Other;
	};

	/**
	* Add a function to create a component
	* @param name Component name
	*/
	template<typename T>
	static void AddComponentClass(const std::string& name)
	{
		if (name.empty())
		{
			Debug::PrintError("[ClassRegistry::AddComponentClass] Empty component name");
		}
		else
		{
			nameToComponent[name] = [](std::shared_ptr<GameObject> go)
				{
					return go->AddComponent<T>();
				};
		}
	}

	/**
	* Register all engine components
	*/
	static void RegisterEngineComponents();

	static void RegisterEngineFileClasses();

	/**
	* Add a component to a GameObject from the component name
	*/
	static std::shared_ptr<Component> AddComponentFromName(const std::string& name, const std::shared_ptr<GameObject>& gameObject);

	/**
	* Get a list of all component names
	*/
	static std::vector<std::string> GetComponentNames();

	/**
	* Reset all registered components
	*/
	static void Reset();

	template<typename T>
	static void AddFileClass(const std::string& name, const FileType fileType)
	{
		if (name.empty())
		{
			Debug::PrintError("[ClassRegistry::AddComponentClass] Empty component name");
		}
		else
		{
			FileClassInfo fileClassInfo;
			fileClassInfo.name = name;
			fileClassInfo.typeId = typeid(T).hash_code();
			fileClassInfo.fileType = fileType;
			fileClassInfos.push_back(fileClassInfo);
		}
	}

	template<typename T>
	static const FileClassInfo& GetFileClassInfo()
	{
		uint64_t classId = typeid(T).hash_code();
		int fileClassInfosCount = fileClassInfos.size();
		for (int i = 0; i < fileClassInfosCount; i++)
		{
			const FileClassInfo& info = fileClassInfos[i];
			if (classId == info.typeId)
			{
				return info;
			}
		}
	}

private:

	static std::unordered_map <std::string, std::function<std::shared_ptr<Component>(const std::shared_ptr<GameObject>&)>> nameToComponent;
	static std::vector<FileClassInfo> fileClassInfos;
};

