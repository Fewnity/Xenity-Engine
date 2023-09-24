#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <unordered_map>
#include <functional>
#include <string>
#include <memory>
#include <vector>

class GameObject;
class Component;

class API ClassRegistry
{
public:

	/**
	* Add a function to create a component
	* @param name Component name
	* @param function Function that returns the GameObject with the new component
	*/
	static void AddComponentClass(const std::string& name, std::function<std::shared_ptr<Component>(std::shared_ptr<GameObject>)> function);

	/**
	* Add a component to a GameObject from the component name
	*/
	static std::shared_ptr<Component> AddComponentFromName(const std::string& name, std::shared_ptr<GameObject> gameObject);

	/**
	* Get a list of all component names
	*/
	static std::vector<std::string> GetComponentNames();

	/**
	* Reset all registered components
	*/
	static void Reset();

private:
	static std::unordered_map <std::string, std::function<std::shared_ptr<Component>(std::shared_ptr<GameObject>)>> nameToComponent;
};

