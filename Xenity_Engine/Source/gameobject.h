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
	Transform transform = Transform(this);
	std::vector<GameObject*> childs;
	std::vector<Component*> components;

	void AddChild(GameObject* gameObject);
	void AddComponent(Component* component);
	void SetChildsWorldPositions();

private:
};
