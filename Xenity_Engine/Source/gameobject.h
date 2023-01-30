#pragma once

#include <string>
#include <vector>
#include "transform.h"

class GameObject;
class Component;
#include "component.h"

class GameObject
{
public:
	GameObject();
	~GameObject();
	bool active = true;
	std::string name = "gameObject";
	Transform transform = Transform();
	std::vector<Component*> components;

	template <typename T>
	void AddComponent()
	{
		T newClass = T();
		components.push_back(newClass);
	}
};
