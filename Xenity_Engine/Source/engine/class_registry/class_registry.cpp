#include "class_registry.h"
#include "../../xenity.h"

std::unordered_map <std::string, std::function<std::shared_ptr<Component>(std::shared_ptr<GameObject>)>> ClassRegistry::nameToComponent;

void ClassRegistry::AddComponentClass(const std::string& name, std::function<std::shared_ptr<Component>(std::shared_ptr<GameObject>)> function)
{
	nameToComponent[name] = function;
}

std::shared_ptr<Component> ClassRegistry::AddComponentFromName(const std::string& name, std::shared_ptr<GameObject> gameObject)
{
	if (nameToComponent.find(name) != nameToComponent.end()) 
	{
		return nameToComponent[name](gameObject);
	}
	else 
	{
		return std::shared_ptr<Component>();
	}
}

std::vector<std::string> ClassRegistry::GetComponentNames()
{
	std::vector<std::string> names;
	for (auto& kv : nameToComponent)
	{
		names.push_back(kv.first);
	}
	return names;
}

void ClassRegistry::Reset()
{
	nameToComponent.clear();
}
