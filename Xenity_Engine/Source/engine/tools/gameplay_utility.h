// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <memory>

#include <engine/api.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/component.h>
#include <engine/accessors/acc_gameobject.h>

class Prefab;

/**
* @brief Check if a GameObject or a Component is valid to use
* @brief If an object is waiting to be destroyed, the object is not valid to use
* @brief Only takes Component, GameObject and Transform
* @param pointer The pointer to check
* @return True if the pointer is valid, false otherwise
*/
template <typename T>
[[nodiscard]] bool IsValid(const std::shared_ptr<T>& pointer)
{
	return IsValid(std::weak_ptr<T>(pointer)); // TODO why creating a weak ptr? Better to check if the pointer is valid here instead of checking in the other function
}

/**
* @brief Check of a GameObject or a Component is valid to use
* @brief If an object is waiting to be destroyed, the object is not valid to use
* @brief Only takes Component, GameObject and Transform
* @param pointer The pointer to check
* @return True if the pointer is valid, false otherwise
*/
template <typename T>
[[nodiscard]] bool IsValid(const std::weak_ptr<T>& pointer)
{
	//TODO: This function is not very efficient, it should be optimized
	bool valid = true;
	if (const auto lockPointer = pointer.lock())
	{
		if (const std::shared_ptr<Component> component = std::dynamic_pointer_cast<Component>(lockPointer))
		{
			if (component->m_waitingForDestroy)
			{
				valid = false;
			}
		}
		else if (const std::shared_ptr<GameObject> gameObject = std::dynamic_pointer_cast<GameObject>(lockPointer))
		{
			if (gameObject->m_waitingForDestroy)
			{
				valid = false;
			}
		}
		else if (const std::shared_ptr<Transform> transform = std::dynamic_pointer_cast<Transform>(lockPointer))
		{
			if (!IsValid(transform->GetGameObject()))
			{
				valid = false;
			}
		}
	}
	else
	{
		valid = false;
	}
	return valid;
}

/**
* @brief Create a new GameObject from another 
* @brief (Not very recommended, can be buggy, use prefabs instead)
* @param gameObject GameObject to duplicate
*/
API std::shared_ptr<GameObject> Instantiate(const std::shared_ptr<GameObject>& gameObject);

/**
* @brief Create GameObjects from a prefab
* @param prefab Prefab to instanciate
*/
API std::shared_ptr<GameObject> Instantiate(const std::shared_ptr<Prefab>& prefab);

/**
* @brief Destroy a GameObject
* @param gameObject GameObject to destroy
*/
API void Destroy(const std::weak_ptr<GameObject>& gameObject);

/**
* @brief Destroy a component
* @param component Component to destroy
*/
template<typename T>
std::enable_if_t<std::is_base_of<Component, T>::value, void>
Destroy(const std::weak_ptr<T>& weakComponent)
{
	Destroy(weakComponent.lock());
}

/**
* @brief Destroy a GameObject
* @param gameObject GameObject to destroy
*/
API void Destroy(const std::shared_ptr<GameObject>& gameObject);

/**
* @brief Destroy a component
* @param component Component to destroy
*/
template<typename T>
std::enable_if_t<std::is_base_of<Component, T>::value, void>
Destroy(const std::shared_ptr<T>& component) 
{
	// Remove the component from the his parent's components list
	if (component) 
	{
		GameObjectAccessor gameObjectAcc = GameObjectAccessor(component->GetGameObject());
		gameObjectAcc.RemoveComponent(component);
	}
}