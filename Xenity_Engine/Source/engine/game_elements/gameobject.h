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

/**
* Create a GameObject with the default name
*/
API std::shared_ptr<GameObject> CreateGameObject();

/**
* Create a GameObject
* @param name GameObject name
*/
API std::shared_ptr<GameObject> CreateGameObject(const std::string& name);

/**
* Create a GameObject not visible in the hierarchy
* @param name GameObject name
*/
API std::shared_ptr<GameObject> CreateGameObjectEditor(const std::string& name);

/**
* Find a GameObject with a name
* @param name GameObject name
*/
API std::shared_ptr<GameObject> FindGameObjectByName(const std::string& name);

/**
* Find a GameObject with an id
* @param id GameObject id
*/
API std::shared_ptr<GameObject> FindGameObjectById(const uint64_t id);

/**
* Find a component with an id
* @param id Component id
*/
API std::shared_ptr<Component> FindComponentById(const uint64_t id);

/**
* Find GameObjects with a name
* @param name GameObjects name
*/
API std::vector<std::shared_ptr<GameObject>> FindGameObjectsByName(const std::string& name);

class API GameObject : public Reflection, public UniqueId, public std::enable_shared_from_this<GameObject>
{
public:
	GameObject();
	GameObject(const std::string& name);
	//void SetReflection();
	std::unordered_map<std::string, ReflectionEntry> GetReflection() override;
	void OnReflectionUpdated() override;

	virtual ~GameObject();

	/**
	* [Internal] Setup the GameObject
	*/
	void Setup();

	std::string name = "";
	std::vector<std::weak_ptr<GameObject>> children;
	std::vector<std::shared_ptr<Component>> components;

	std::weak_ptr<GameObject> parent;

	/**
	* Add a child to the GameObject
	* @param gameObject Child to add
	*/
	void AddChild(const std::weak_ptr<GameObject>& gameObject);

	/**
	* Set GameObject's parent
	* @param gameObject New parent
	*/
	void SetParent(const std::weak_ptr<GameObject>& gameObject);
	bool waitingForDestroy = false;

	/**
	* Add a component
	*/
	template <typename T>
	std::shared_ptr<T> AddComponent()
	{
		std::shared_ptr<Component> newC = std::make_shared<T>();
		AddExistingComponent(newC);
		return std::shared_ptr<T>(std::dynamic_pointer_cast<T>(newC));
	}

	/**
	* Remove a component
	*/
	void RemoveComponent(const std::weak_ptr <Component>& weakComponent);

	/**
	* Get a component
	*/
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

	/**
	* Get if the GameObject is marked as active
	*/
	bool GetActive() const;

	/**
	* Get if the GameObject is active based on his parents
	*/
	bool GetLocalActive() const;

	/**
	* Set GameObject as active or not
	*/
	void SetActive(const bool active);

	/**
	* Get children count
	*/
	int GetChildrenCount()
	{
		return childCount;
	}

	/**
	* Get component count
	*/
	int GetComponentCount()
	{
		return componentCount;
	}

	/**
	* Get transform
	*/
	std::shared_ptr<Transform> GetTransform()
	{
		return transform;
	}

private:
	std::shared_ptr<Transform> transform;

	/**
	* Add an existing component
	*/
	void AddExistingComponent(std::shared_ptr <Component> component);

	/**
	* Update local active value
	*/
	void UpdateActive(const std::weak_ptr<GameObject>& changed);

	bool active = true;
	bool localActive = true;
	int childCount = 0;
	int componentCount = 0;
};
