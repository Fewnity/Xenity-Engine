#pragma once

#include <string>
#include <vector>
#include "transform.h"
#include <memory>

class GameObject;
class Component;
#include "component.h"

class GameObject
{
public:
	GameObject();
	GameObject(std::string name);
	~GameObject();
	std::string name = "gameObject";
	Transform transform = Transform(this);
	std::vector<GameObject*> childs;
	std::vector<Component*> components;

	GameObject* parent = nullptr;
	void SetChildsWorldPositions();

	void AddChild(GameObject* gameObject);
	void AddExistingComponent(Component* component);

	template <typename T>
	Component* AddComponent()
	{
		Component* newC = new T();
		AddExistingComponent(newC);
		return newC;
	}

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

	static GameObject* FindGameObjectByName(std::string name);
	static std::vector<GameObject*> FindGameObjectsByName(std::string name);
	
	bool GetActive();
	bool GetLocalActive();
	void SetActive(bool active);
private:
	void UpdateActive(GameObject * changed);
	bool active = true;
	bool localActive = true;
};
