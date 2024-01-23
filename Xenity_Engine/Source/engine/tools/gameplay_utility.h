#pragma once
#include <engine/api.h>

#include <memory>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/component.h>

/**
* Check if a GameObject or a Component is valid
*/
template <typename T>
bool IsValid(const std::shared_ptr<T>& pointer)
{
	return IsValid(std::weak_ptr<T>(pointer));
}

/**
* Check of a GameObject or a Component is valid
*/
template <typename T>
bool IsValid(const std::weak_ptr<T>& pointer)
{
	bool valid = true;
	if (const auto lockPointer = pointer.lock())
	{
		if (const std::shared_ptr<Component> component = std::dynamic_pointer_cast<Component>(lockPointer))
		{
			if (component->waitingForDestroy)
			{
				valid = false;
			}
		}
		else if (const std::shared_ptr<GameObject> gameObject = std::dynamic_pointer_cast<GameObject>(lockPointer))
		{
			if (gameObject->waitingForDestroy)
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
* Create a new GameObject from another
*/
API std::shared_ptr<GameObject> Instantiate(const std::shared_ptr<GameObject>& gameObject);

/**
* Destroy a gameObject
*/
API void Destroy(const std::weak_ptr<GameObject>& gameObject);

/**
* Destroy a component
*/
API void Destroy(const std::weak_ptr<Component>& component);

/**
*Destroy a gameObject
*/
API void Destroy(const std::shared_ptr<GameObject>& gameObject);

/**
* Destroy a component
*/
API void Destroy(const std::shared_ptr<Component>& component);