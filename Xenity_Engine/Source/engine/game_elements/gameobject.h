#pragma once

#include <string>
#include <vector>
#include "transform.h"
#include <memory>

class GameObject;
class Component;
#include "../component.h"

class GameObject
{
public:
	GameObject();
	GameObject(std::string name);
	~GameObject();
	std::string name = "gameObject";
	Transform transform = Transform(this);
	std::vector<GameObject*> children;
	std::vector<Component*> components;

	GameObject* parent = nullptr;
	void OnTransformMove();

	//void SetChildrenWorldPositions();

	void AddChild(GameObject* gameObject);
	void SetParent(GameObject* gameObject);
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
		for (int i = 0; i < componentCount; i++)
		{
			if (T* result = dynamic_cast<T*>(components[i])) {
				return result;
			}
		}
	}

	static GameObject* FindGameObjectByName(const std::string name);
	static std::vector<GameObject*> FindGameObjectsByName(const std::string name);
	
	bool GetActive() const;
	bool GetLocalActive() const;
	void SetActive(const bool active);

	int GetChildrenCount() 
	{
		return childCount;
	}

	int GetComponentCount() 
	{
		return componentCount;
	}

private:
	void UpdateActive(GameObject * changed);

	bool active = true;
	bool localActive = true;
	int childCount = 0;
	int componentCount = 0;
};
