#include "class_registry.h"

std::unordered_map <std::string, std::function<std::shared_ptr<Component>(const std::shared_ptr<GameObject>&)>> ClassRegistry::nameToComponent;

std::shared_ptr<Component> ClassRegistry::AddComponentFromName(const std::string& name, const std::shared_ptr<GameObject>& gameObject)
{
	if (nameToComponent.find(name) != nameToComponent.end()) // Check if the component is in the list
	{
		return nameToComponent[name](gameObject); // Call the function to add the component to the gameObject
	}
	else
	{
		return nullptr;
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
