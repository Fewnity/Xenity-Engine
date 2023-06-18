#pragma once

#include <string>
#include <vector>
#include "transform.h"
#include <memory>

class Component;
#include "../component.h"

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
	GameObject();
	GameObject(std::string name);
	~GameObject();
	void Setup();

	std::string name = "gameObject";
	std::vector<std::weak_ptr<GameObject>> children;
	std::vector<Component*> components;

	std::weak_ptr<GameObject> parent;

	//void SetChildrenWorldPositions();

	void AddChild(std::weak_ptr<GameObject> gameObject);
	void SetParent(std::weak_ptr<GameObject> gameObject);
	bool waitingForDestroy = false;

	template <typename T>
	T* AddComponent()
	{
		Component* newC = new T();
		AddExistingComponent(newC);
		return dynamic_cast<T*>(newC);
	}

	void RemoveComponent(Component* component)
	{
		if (!component->waitingForDestroy)
		{
			component->waitingForDestroy = true;
			componentCount--;
		}
		/*std::weak_ptr<GameObject> gameOject = component->GetGameObject();
		for (int i = 0; i < componentCount; i++)
		{
			if (gameOject.lock()->components[i] == component)
			{
				gameOject.lock()->components.erase(gameOject.lock()->components.begin() + i);
				delete component;
				componentCount--;
				break;
			}
		}*/
	}

	template <typename T>
	T* GetComponent() {
		for (int i = 0; i < componentCount; i++)
		{
			if (T* result = dynamic_cast<T*>(components[i]))
			{
				return result;
			}
		}
	}

	static std::weak_ptr<GameObject> FindGameObjectByName(const std::string name);
	static std::vector<std::weak_ptr<GameObject>> FindGameObjectsByName(const std::string name);

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

	std::weak_ptr<Transform> GetTransform()
	{
		return transform;
	}

private:
	std::shared_ptr<Transform> transform;
	void AddExistingComponent(Component* component);
	void UpdateActive(std::weak_ptr<GameObject> changed);

	bool active = true;
	bool localActive = true;
	int childCount = 0;
	int componentCount = 0;
};
