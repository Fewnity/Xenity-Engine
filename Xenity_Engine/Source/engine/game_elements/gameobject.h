#pragma once

#include <string>
#include <vector>
#include "transform.h"
#include <memory>

class Component;
#include "../component.h"

std::weak_ptr<GameObject> CreateGameObject();
std::weak_ptr<GameObject> CreateGameObject(std::string name);
std::weak_ptr<GameObject> FindGameObjectByName(const std::string name);
std::vector<std::weak_ptr<GameObject>> FindGameObjectsByName(const std::string name);

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
	GameObject();
	GameObject(std::string name);
	~GameObject();
	void Setup();

	std::string name = "";
	std::vector<std::weak_ptr<GameObject>> children;
	std::vector<std::shared_ptr<Component>> components;

	std::weak_ptr<GameObject> parent;

	void AddChild(std::weak_ptr<GameObject> gameObject);
	void SetParent(std::weak_ptr<GameObject> gameObject);
	bool waitingForDestroy = false;

	template <typename T>
	std::weak_ptr<T> AddComponent()
	{
		std::shared_ptr<Component> newC = std::make_shared<T>();
		AddExistingComponent(newC);
		return std::weak_ptr<T>(std::dynamic_pointer_cast<T>(newC));
	}

	void RemoveComponent(std::weak_ptr <Component> weakComponent);

	void RemoveComponentInternal(std::shared_ptr<Component> sharedComponent);

	template <typename T>
	T* GetComponent() 
	{
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
	void AddExistingComponent(std::shared_ptr <Component> component);
	void UpdateActive(std::weak_ptr<GameObject> changed);

	bool active = true;
	bool localActive = true;
	int childCount = 0;
	int componentCount = 0;
};
