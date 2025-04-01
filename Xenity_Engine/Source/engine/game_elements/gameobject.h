// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <string>
#include <vector>
#include <memory>

#include <engine/api.h>
#include <engine/reflection/reflection.h>
#include <engine/unique_id/unique_id.h>
#include <engine/component.h>
#include <engine/game_elements/component_manager.h>

class Transform;

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

#if defined(EDITOR)
/**
* @brief Create a GameObject not visible in the hierarchy
* @param name GameObject name
* @return The created GameObject
*/
API std::shared_ptr<GameObject> CreateGameObjectEditor(const std::string& name);
#endif

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
	GameObject(const GameObject& other) = delete;
	GameObject& operator=(const GameObject&) = delete;

	virtual ~GameObject();

	/*const*/ inline std::string & GetName()
	{
		return m_name;
	}

	inline void SetName(const std::string& name)
	{
		this->m_name = name;
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
		std::shared_ptr<T> newComponent = ComponentManager::CreateComponent<T>();
		AddExistingComponent(newComponent);
		return newComponent;
	}

	/**
	* @brief Get a component
	* @return The component
	*/
	template <typename T>
	std::enable_if_t<std::is_base_of<Component, T>::value, std::shared_ptr<T>>
	GetComponent() const
	{
		for (int i = 0; i < m_componentCount; i++)
		{
			if (auto result = std::dynamic_pointer_cast<T>(m_components[i]))
			{
				return result;
			}
		}
		return nullptr;
	}

	/**
	* @brief Get a component
	* @return The component
	*/
	template <typename T>
	std::enable_if_t<std::is_base_of<Component, T>::value, std::vector<std::shared_ptr<T>>>
	GetComponents() const
	{
		std::vector<std::shared_ptr<T>> componentList;
		for (int i = 0; i < m_componentCount; i++)
		{
			if (auto result = std::dynamic_pointer_cast<T>(m_components[i]))
			{
				componentList.push_back(result);
			}
		}
		return componentList;
	}

	/**
	* @brief Get if the GameObject is marked as active
	*/
	inline bool IsActive() const
	{
		return m_active;
	}

	/**
	* @brief Get if the GameObject is active based on his parents
	*/
	inline bool IsLocalActive() const
	{
		return m_localActive;
	}

	/**
	* @brief Set GameObject as active or not
	* @param active Active value
	*/
	void SetActive(const bool active);

	inline bool IsStatic() const
	{
		return m_isStatic;
	}

	/**
	* @brief Get children count
	*/
	inline uint32_t GetChildrenCount() const
	{
		return static_cast<uint32_t>(m_childCount);
	}

	/**
	* @brief Get component count
	*/
	inline uint32_t GetComponentCount() const
	{
		return static_cast<uint32_t>(m_componentCount);
	}

	/**
	* @brief Get transform
	*/
	inline const std::shared_ptr<Transform>& GetTransform() const
	{
		return m_transform;
	}

	inline const std::weak_ptr<GameObject>& GetParent() const
	{
		return m_parent;
	}

	inline const std::weak_ptr<GameObject>& GetChild(int index)
	{
		return m_children[index];
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
	friend class PhysicsManager;
	template<typename T>
	friend class ReflectiveChangeValueCommand;
	template<typename T>
	friend class InspectorItemSetActiveCommand;
	friend class Graphics;
	template<typename U, typename T>
	friend class InspectorChangeValueCommand;
	template<typename T>
	friend class InspectorItemSetStaticCommand;

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	std::vector<std::shared_ptr<Component>> m_components;
	std::vector<std::weak_ptr<GameObject>> m_children;
	std::string m_name = "GameObject";
	std::weak_ptr<GameObject> m_parent;

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
	* Get a list of all children
	*/
	inline std::vector<std::weak_ptr<GameObject>>& GetChildren()
	{
		return m_children;
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
	void UpdateActive(const GameObject& changed);

	/**
	* @brief Check if the GameObject is a parent of another GameObject
	*/
	bool IsParentOf(const std::shared_ptr<GameObject>& gameObject);

	std::shared_ptr<Transform> m_transform;

	uint16_t m_childCount = 0;
	uint16_t m_componentCount = 0;

#if defined(EDITOR)
	bool m_isSelected = false;
#endif
	bool m_waitingForDestroy = false;

	bool m_active = true;
	bool m_localActive = true;
	bool m_isStatic = false;
};
