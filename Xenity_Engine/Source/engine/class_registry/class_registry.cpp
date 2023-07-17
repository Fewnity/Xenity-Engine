#include "class_registry.h"
#include "../../xenity.h"

std::unordered_map <std::string, std::function<std::shared_ptr<Component>(std::shared_ptr<GameObject>)>> ClassRegistry::nameToComponent;

void ClassRegistry::AddComponentClass(std::string name, std::function<std::shared_ptr<Component>(std::shared_ptr<GameObject>)> function)
{
	nameToComponent[name] = function;
}

std::shared_ptr<Component> ClassRegistry::AddComponentFromName(std::string name, std::shared_ptr<GameObject> gameObject)
{
	return nameToComponent[name](gameObject);
}
