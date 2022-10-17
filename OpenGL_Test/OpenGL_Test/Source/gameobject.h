#pragma once

#include <string>
#include <vector>
#include "transform.h"

class GameObject;
class Component;
#include "component.h"
template <class  T>
void AddComponent(T newClass);

class GameObject
{
public:
	GameObject();
	bool active = true;
	std::string name;
	Transform transform = Transform();
	std::vector<Component> components;
};
