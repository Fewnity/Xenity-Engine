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
	static void AddComponentClass(std::string name, std::function<std::shared_ptr<Component>(std::shared_ptr<GameObject>)> function);
	static std::shared_ptr<Component> AddComponentFromName(std::string name, std::shared_ptr<GameObject> gameObject);
	static std::vector<std::string> GetComponentNames();
	static void Reset();

private:
	static std::unordered_map <std::string, std::function<std::shared_ptr<Component>(std::shared_ptr<GameObject>)>> nameToComponent;
};

