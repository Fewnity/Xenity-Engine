#pragma once
#include <engine/api.h>

#include <unordered_map>
#include <functional>
#include <string>
#include <memory>
#include <vector>

#include <engine/game_elements/gameobject.h>
#include <engine/debug/debug.h>

class Component;

class API ClassRegistry
{
public:

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

private:
	static std::unordered_map <std::string, std::function<std::shared_ptr<Component>(const std::shared_ptr<GameObject>&)>> nameToComponent;
};

