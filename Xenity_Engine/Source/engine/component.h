// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include <string>
#include <memory>
#include <engine/reflection/reflection.h>
#include <engine/reflection/enum_utils.h>
#include <engine/unique_id/unique_id.h>

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

#if defined(EDITOR)
	/**
	* @brief Function called every frame in the editor only
	*/
	virtual void UpdateEditor() {}
#endif
	//virtual void OnParentChanged() {}

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

	int updatePriority = 5000; //Lower is more priotary

	/**
	* @brief Get if the component is enabled
	*/
	bool IsEnabled() const;

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
		return gameObject.lock();
	}

	/**
	* @brief Get component's Transform
	*/
	inline std::shared_ptr <Transform> GetTransform() const
	{
		return transform.lock();
	}

	/**
	* @brief Get component's name
	*/
	inline const std::string& GetComponentName() const
	{
		return componentName;
	}

	/**
	* @brief Return a string representation of the component
	*/
	virtual std::string ToString();

private:
	friend class GameplayManager;
	friend class GameObject;
	friend class InspectorMenu;
	friend class SceneManager;

	bool initiated = false;
	bool isAwakeCalled = false;
	bool waitingForDestroy = false;
	bool isEnabled = true;

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

	std::string componentName = "";

private:
	std::weak_ptr <GameObject> gameObject;
	std::weak_ptr <Transform> transform;
};