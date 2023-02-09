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
	GameObject(std::string name);
	~GameObject();
	bool active = true;
	std::string name = "gameObject";
	Transform transform = Transform(this);
	std::vector<GameObject*> childs;
	std::vector<Component*> components;

	void AddChild(GameObject* gameObject);
	void AddComponent(Component* component);

	template <typename T>
	T* GetComponent() {
		int c = components.size();
		for (int i = 0; i < c; i++)
		{
			if (T* result = dynamic_cast<T*>(components[i])) {
				return result;
			}
		}
	}

	void SetChildsWorldPositions();
	static GameObject* FindGameObjectByName(std::string name);
	static std::vector<GameObject*> FindGameObjectsByName(std::string name);

private:
};
