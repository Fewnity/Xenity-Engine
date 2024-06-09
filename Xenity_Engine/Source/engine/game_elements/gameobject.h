// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include <string>
#include <vector>
#include <memory>

class Component;
class Transform;

#include <engine/reflection/reflection.h>
#include <engine/unique_id/unique_id.h>

/**
* @brief Create a GameObject with the default name
* @return The created GameObject
*/
API std::shared_ptr<GameObject> CreateGameObject();

/**
* @brief Create a GameObject
* @param name GameObject name
* @return The created GameObject
*/
API std::shared_ptr<GameObject> CreateGameObject(const std::string& name);

/**
* @brief Create a GameObject not visible in the hierarchy
* @param name GameObject name
* @return The created GameObject
*/
API std::shared_ptr<GameObject> CreateGameObjectEditor(const std::string& name);

/**
* @brief Find a GameObject with a name
* @param name GameObject name
* @return The found GameObject
*/
API std::shared_ptr<GameObject> FindGameObjectByName(const std::string& name);

/**
* @brief Find a GameObject with an id
* @param id GameObject id
* @return The found GameObject
*/
API std::shared_ptr<GameObject> FindGameObjectById(const uint64_t id);

/**
* @brief Find a component with an id
* @param id Component id
* @return The found component
*/
API std::shared_ptr<Component> FindComponentById(const uint64_t id);

/**
* @brief Find GameObjects with a name
* @param name GameObjects name
* @return The found GameObjects
*/
API std::vector<std::shared_ptr<GameObject>> FindGameObjectsByName(const std::string& name);

class API GameObject : public Reflective, public UniqueId, public std::enable_shared_from_this<GameObject>
{
public:

	GameObject();
	explicit GameObject(const std::string& name);

	virtual ~GameObject();

	/*const*/ inline std::string & GetName()
	{
		return name;
	}

	inline void SetName(const std::string& name)
	{
		this->name = name;
	}

	/**
	* @brief Add a child to the GameObject
	* @param gameObject Child to add
	*/
	void AddChild(const std::shared_ptr<GameObject>& gameObject);

	/**
	* @brief Set GameObject's parent
	* @param gameObject New parent
	*/
	void SetParent(const std::shared_ptr<GameObject>& gameObject);

	/**
	* @brief Add a component
	* @return The added component
	*/
	template <typename T>
	std::enable_if_t<std::is_base_of<Component, T>::value, std::shared_ptr<T>>
	AddComponent()
	{
		std::shared_ptr<Component> newC = std::make_shared<T>();
		AddExistingComponent(newC);
		return std::shared_ptr<T>(std::dynamic_pointer_cast<T>(newC));
	}

	/**
	* @brief Get a component
	* @return The component
	*/
	template <typename T>
	std::enable_if_t<std::is_base_of<Component, T>::value, std::shared_ptr<T>>
	GetComponent() const
	{
		for (int i = 0; i < componentCount; i++)
		{
			if (auto result = std::dynamic_pointer_cast<T>(components[i]))
			{
				return result;
			}
		}
		return nullptr;
	}

	/**
	* @brief Get if the GameObject is marked as active
	*/
	bool GetActive() const;

	/**
	* @brief Get if the GameObject is active based on his parents
	*/
	bool GetLocalActive() const;

	/**
	* @brief Set GameObject as active or not
	* @param active Active value
	*/
	void SetActive(const bool active);

	/**
	* @brief Get children count
	*/
	inline int GetChildrenCount() const
	{
		return childCount;
	}

	/**
	* @brief Get component count
	*/
	inline int GetComponentCount() const
	{
		return componentCount;
	}

	/**
	* @brief Get transform
	*/
	inline std::shared_ptr<Transform> GetTransform() const
	{
		return transform;
	}

	inline std::weak_ptr<GameObject> GetParent() const
	{
		return parent;
	}

	inline std::weak_ptr<GameObject> GetChild(int index)
	{
		return children[index];
	}

private:
	friend class GameObjectAccessor;
	friend class GameplayManager;
	friend class SceneManager;
	friend class EditorUI;
	friend class InspectorMenu;
	friend class InspectorDeleteGameObjectCommand;
	friend class Transform;
	friend class Canvas;
	friend class Editor;
	template<typename T>
	friend class ReflectiveChangeValueCommand;
	template<typename T>
	friend class InspectorItemSetActiveCommand;
	friend class Graphics;
	template<typename U, typename T>
	friend class InspectorChangeValueCommand;

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	std::vector<std::shared_ptr<Component>> components;
	std::vector<std::weak_ptr<GameObject>> children;
	std::string name = "GameObject";
	std::weak_ptr<GameObject> parent;

	/**
	* @brief [Internal] Setup the GameObject
	*/
	void Setup();

	/**
	* @brief Remove a component
	* @param component Component to remove
	*/
	void RemoveComponent(const std::shared_ptr <Component>& component);

	/**
	* 
	*/
	inline std::vector<std::weak_ptr<GameObject>>& GetChildren()
	{
		return children;
	}

	/**
	* @brief Add an existing component
	* @param component Component to add
	*/
	void AddExistingComponent(const std::shared_ptr<Component>& component);

	/**
	* @brief Update local active value
	* @param changed The changed GameObject
	*/
	void UpdateActive(const std::shared_ptr<GameObject>& changed);

	/**
	* @brief Check if the GameObject is a parent of another GameObject
	*/
	bool IsParentOf(const std::shared_ptr<GameObject>& gameObject);

	std::shared_ptr<Transform> transform;

	int childCount = 0;
	int componentCount = 0;

#if defined(EDITOR)
	bool isSelected = false;
#endif
	bool waitingForDestroy = false;

	bool active = true;
	bool localActive = true;
};
