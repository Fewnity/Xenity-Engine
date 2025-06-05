// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/api.h>
#include <engine/reflection/reflection.h>
#include <engine/reflection/enum_utils.h>
#include <engine/unique_id/unique_id.h>
#include <engine/physics/collision_event.h>
#include <engine/assertions/assertions.h>

class GameObject;
class Transform;

/*
* @brief Class used to create something that can be attached to a GameObject
*/
class API Component : public UniqueId, public Reflective, public std::enable_shared_from_this<Component>
{
public:
	Component() : Component(true, true) {}
	Component(bool canBeDisabled, bool allowOtherInstanceOnGameObject);
	Component(const Component& other) = delete;
	Component& operator=(const Component&) = delete;

	virtual ~Component();

	/**
	* @brief Function called once before Start() at the creation of the component
	*/
	virtual void Awake() {}

	/**
	* @brief Function called once after Awake() at the creation of the component
	*/
	virtual void Start() {}

	/**
	* @brief Function called every frame
	*/
	virtual void Update() {}

	//virtual void OnParentChanged() {}

	/**
	 * @brief Event called when a component is attached to a GameObject (Called once after AddComponent) 
	 */
	virtual void OnComponentAttached() {};

	/**
	* @brief Called when the component is disabled
	*/
	virtual void OnDisabled() {};

	/**
	* @brief Called when the component is enabled
	*/
	virtual void OnEnabled() {};

	/**
	* @brief Called each frame to draw gizmos
	*/
	virtual void OnDrawGizmos() {};

	/**
	* @brief Called each frame to draw gizmos if the object is selected
	*/
	virtual void OnDrawGizmosSelected() {};

	/**
	* @brief Get if the component is enabled
	*/
	[[nodiscard]] inline bool IsEnabled() const
	{
		return m_isEnabled;
	}

	/**
	* @brief Enable or disable the component
	* @param isEnabled: true to enable, false to disable
	*/
	void SetIsEnabled(bool isEnabled);

	/**
	* @brief Get component's GameObject
	*/
	[[nodiscard]] inline std::shared_ptr <GameObject> GetGameObject() const
	{
		XASSERT(m_gameObject.lock(), "The gameobject is null");
		return m_gameObject.lock();
	}
	
	/**
	* @brief Get component's GameObject raw pointer for faster access (Not safe, use with caution)
	*/
	[[nodiscard]] inline GameObject* GetGameObjectRaw() const
	{
		XASSERT(m_gameObjectRaw, "The gameobject is null");
		return m_gameObjectRaw;
	}

	/**
	* @brief Get component's Transform
	*/
	[[nodiscard]] inline std::shared_ptr<Transform> GetTransform() const
	{
		XASSERT(m_transform.lock(), "The transform is null");
		return m_transform.lock();
	}

	/**
	* @brief Get component's Transform raw pointer for faster access (Not safe, use with caution)
	*/
	[[nodiscard]] inline Transform* GetTransformRaw() const
	{
		//XASSERT(m_transformRaw, "The transform is null"); // Disabled for a small hack in the lighting system
		return m_transformRaw;
	}

	/**
	* @brief Get component's name
	*/
	[[nodiscard]] inline const std::string& GetComponentName() const
	{
		XASSERT(m_componentName, "The component's name is null");
		XASSERT(!(*m_componentName).empty(), "The component's name is empty");
		return *m_componentName;
	}

	/**
	* @brief Return a string representation of the component
	*/
	[[nodiscard]] inline virtual std::string ToString()
	{
		XASSERT(m_componentName, "The component's name is null");
		XASSERT(!(*m_componentName).empty(), "The component's name is empty");
		return "{" + *m_componentName + "}";
	}

private:
	friend class GameplayManager;
	template<class T>
	friend class ComponentList;
	friend class GameObject;
	friend class InspectorMenu;
	friend class SceneManager;
	friend class PhysicsManager;
	friend class ClassRegistry;
	template <typename T>
	friend bool IsValid(const std::weak_ptr<T>& pointer);

	/**
	* @brief [Internal] Set component's GameObject
	* @param gameObject: GameObject to set
	*/
	void SetGameObject(const std::shared_ptr<GameObject>& gameObject);

protected:
	/**
	* @brief [Internal] Remove references of this component for some specific cases, should not be used by the user
	*/
	virtual void RemoveReferences() {};

	/**
	* @brief Function called when a collision has just occured with another collider
	*/
	virtual void OnCollisionEnter(CollisionEvent info) {};

	/**
	* @brief Function called every frame if a collider of the GameObject is still colliding with another collider
	*/
	virtual void OnCollisionStay(CollisionEvent info) {};

	/**
	* @brief Function called when the collision has ended
	*/
	virtual void OnCollisionExit(CollisionEvent info) {};

	/**
	* @brief Function called when a collider of the GameObject just entered a trigger collider
	*/
	virtual void OnTriggerEnter(CollisionEvent info) {};

	/**
	* @brief Function called when a collider of the GameObject is still in a trigger collider
	*/
	virtual void OnTriggerStay(CollisionEvent info) {};

	/**
	* @brief Function called when a collider of the GameObject just leave a trigger collider
	*/
	virtual void OnTriggerExit(CollisionEvent info) {};

	const std::string* m_componentName = nullptr;

private:
	std::weak_ptr <GameObject> m_gameObject;
	std::weak_ptr <Transform> m_transform;
	// Raw pointer for faster access
	Transform* m_transformRaw = nullptr;
	GameObject* m_gameObjectRaw = nullptr;

private:
	bool m_initiated = false;
	bool m_isAwakeCalled = false;
	bool m_waitingForDestroy = false;
	bool m_isEnabled = true;
	bool m_canBeDisabled = true;
	bool m_allowOtherInstanceOnGameObject = true;
};