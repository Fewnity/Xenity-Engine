#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <string>
#include <vector>
#include "transform.h"
#include <memory>

class Component;
#include "../component.h"
#include "../debug/debug.h"
#include "../reflection/reflection.h"
#include "../unique_id/unique_id.h"

API std::shared_ptr<GameObject> CreateGameObject();
API std::shared_ptr<GameObject> CreateGameObject(std::string name);
API std::shared_ptr<GameObject> CreateGameObjectEditor(std::string name);
API std::shared_ptr<GameObject> FindGameObjectByName(const std::string name);
API std::shared_ptr<GameObject> FindGameObjectById(const uint64_t id);
API std::vector<std::shared_ptr<GameObject>> FindGameObjectsByName(const std::string name);

class API GameObject : public Reflection, public UniqueId, public std::enable_shared_from_this<GameObject>
{
public:
	GameObject();
	GameObject(std::string name);
	//void SetReflection();
	std::unordered_map<std::string, Variable> GetReflection();
	void OnReflectionUpdated();

	virtual ~GameObject();
	void Setup();

	std::string name = "";
	std::vector<std::weak_ptr<GameObject>> children;
	std::vector<std::shared_ptr<Component>> components;

	std::weak_ptr<GameObject> parent;

	void AddChild(std::weak_ptr<GameObject> gameObject);
	void SetParent(std::weak_ptr<GameObject> gameObject);
	bool waitingForDestroy = false;

	template <typename T>
	std::shared_ptr<T> AddComponent()
	{
		std::shared_ptr<Component> newC = std::make_shared<T>();
		AddExistingComponent(newC);
		return std::shared_ptr<T>(std::dynamic_pointer_cast<T>(newC));
	}

	void RemoveComponent(std::weak_ptr <Component> weakComponent);
	void InternalDestroyComponent(std::weak_ptr <Component> weakComponent);

	template <typename T>
	std::weak_ptr<T> GetComponent()
	{
		for (int i = 0; i < componentCount; i++)
		{
			if (auto result = std::dynamic_pointer_cast<T>(components[i]))
			{
				return result;
			}
		}
		return std::weak_ptr<T>();
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

	std::shared_ptr<Transform> GetTransform()
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
