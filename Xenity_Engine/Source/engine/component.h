// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <string>
#include <memory>

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
	Component();

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
	* @brief Called when the component is enabled
	*/
	virtual void OnDisabled() {};

	/**
	* @brief Called when the component is disabled
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
	inline bool IsEnabled() const
	{
		return m_isEnabled;
	}

	/**
	* @brief Enable or disable the component*
	* @param isEnabled: true to enable, false to disable
	*/
	void SetIsEnabled(bool isEnabled);

	/**
	* @brief Get component's GameObject
	*/
	inline std::shared_ptr <GameObject> GetGameObject() const
	{
		XASSERT(m_gameObject.lock(), "The gameobject is null");
		return m_gameObject.lock();
	}
	
	/**
	* @brief Get component's GameObject raw pointer for faster access
	*/
	inline GameObject* GetGameObjectRaw() const
	{
		XASSERT(m_gameObjectRaw, "The gameobject is null");
		return m_gameObjectRaw;
	}

	/**
	* @brief Get component's Transform
	*/
	inline std::shared_ptr <Transform> GetTransform() const
	{
		XASSERT(m_transform.lock(), "The transform is null");
		return m_transform.lock();
	}

	/**
	* @brief Get component's Transform raw pointer for faster access
	*/
	inline Transform* GetTransformRaw() const
	{
		//XASSERT(m_transformRaw, "The transform is null"); // Disabled for a small hack in the lighting system
		return m_transformRaw;
	}

	/**
	* @brief Get component's name
	*/
	inline const std::string& GetComponentName() const
	{
		XASSERT(!m_componentName.empty(), "The component's name is empty");
		return m_componentName;
	}

	/**
	* @brief Return a string representation of the component
	*/
	inline virtual std::string ToString()
	{
		return "{" + m_componentName + "}";
	}

	inline int GetUpdatePriority() const
	{
		return m_updatePriority;
	}

	void SetUpdatePriority(int priority);

private:
	friend class GameplayManager;
	friend class GameObject;
	friend class InspectorMenu;
	friend class SceneManager;
	friend class PhysicsManager;

	/**
	* @brief [Internal] Set component's GameObject
	* @param gameObject: GameObject to set
	*/
	void SetGameObject(const std::shared_ptr<GameObject>& gameObject);

protected:
	/**
	* @brief [Internal] Remove references of this component for some specific cases
	*/
	virtual void RemoveReferences() {};

	virtual void OnCollisionEnter(CollisionEvent info) {};
	virtual void OnCollisionStay(CollisionEvent info) {};
	virtual void OnCollisionExit(CollisionEvent info) {};

	virtual void OnTriggerEnter(CollisionEvent info) {};
	virtual void OnTriggerStay(CollisionEvent info) {};
	virtual void OnTriggerExit(CollisionEvent info) {};

	std::string m_componentName = "";

private:
	std::weak_ptr <GameObject> m_gameObject;
	std::weak_ptr <Transform> m_transform;
	// Raw pointer for faster access
	Transform* m_transformRaw = nullptr;
	GameObject* m_gameObjectRaw = nullptr;

protected:
	int m_updatePriority = 5000; //Lower is more priority

private:
	bool m_initiated = false;
	bool m_isAwakeCalled = false;
	bool m_waitingForDestroy = false;
	bool m_isEnabled = true;
};