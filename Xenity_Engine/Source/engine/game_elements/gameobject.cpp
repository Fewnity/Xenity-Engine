// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "gameobject.h"

#include <engine/debug/debug.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/game_elements/transform.h>
#include <engine/component.h>
#include <engine/accessors/acc_gameobject.h>
#include <engine/debug/performance.h>
#include <engine/debug/memory_tracker.h>

#pragma region Constructors / Destructor

#define DEFAULT_GAMEOBJECT_NAME "GameObject"

std::shared_ptr<GameObject> CreateGameObject()
{
	std::shared_ptr<GameObject> newGameObject = std::make_shared<GameObject>();
	GameplayManager::AddGameObject(newGameObject);
	GameObjectAccessor gameObjectAcc = GameObjectAccessor(newGameObject);
	gameObjectAcc.Setup();
	return newGameObject;
}

std::shared_ptr<GameObject> CreateGameObject(const std::string& name)
{
	std::shared_ptr<GameObject> newGameObject = std::make_shared<GameObject>(name);
	GameplayManager::AddGameObject(newGameObject);
	GameObjectAccessor gameObjectAcc = GameObjectAccessor(newGameObject);
	gameObjectAcc.Setup();
	return newGameObject;
}

#if defined(EDITOR)
std::shared_ptr<GameObject> CreateGameObjectEditor(const std::string& name)
{
	std::shared_ptr<GameObject> newGameObject = std::make_shared<GameObject>(name);
	GameplayManager::AddGameObjectEditor(newGameObject);
	GameObjectAccessor gameObjectAcc = GameObjectAccessor(newGameObject);
	gameObjectAcc.Setup();
	return newGameObject;
}
#endif

std::shared_ptr<Component> FindComponentById(const uint64_t id)
{
	for (int i = 0; i < GameplayManager::gameObjectCount; i++)
	{
		std::shared_ptr<GameObject> gameobject = GameplayManager::gameObjects[i];

		GameObjectAccessor gameobjectAcc = GameObjectAccessor(gameobject);
		std::vector<std::shared_ptr<Component>>& gameobjectComponents = gameobjectAcc.GetComponents();

		const int componentCount = gameobject->GetComponentCount();
		for (int compI = 0; compI < componentCount; compI++)
		{
			if (gameobjectComponents[compI]->GetUniqueId() == id)
			{
				return gameobjectComponents[compI];
			}
		}
	}
	return std::shared_ptr<Component>();
}

GameObject::GameObject()
{
	m_name = DEFAULT_GAMEOBJECT_NAME;

#if defined (DEBUG)
	Performance::s_gameObjectMemoryTracker->Allocate(sizeof(GameObject));
#endif
}

GameObject::GameObject(const std::string& _name)
{
	if (!_name.empty())
		m_name = _name;
	else
		m_name = DEFAULT_GAMEOBJECT_NAME;

#if defined (DEBUG)
	Performance::s_gameObjectMemoryTracker->Allocate(sizeof(GameObject));
#endif
}

ReflectiveData GameObject::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, m_name, "name", true);
	Reflective::AddVariable(reflectedVariables, m_active, "active", true);
	Reflective::AddVariable(reflectedVariables, m_isStatic, "isStatic", true);
	return reflectedVariables;
}

GameObject::~GameObject()
{
	for (int i = 0; i < m_componentCount; i++)
	{
		if (m_components[i])
			m_components[i]->RemoveReferences();
	}
	m_components.clear();

#if defined (DEBUG)
	Performance::s_gameObjectMemoryTracker->Deallocate(sizeof(GameObject));
#endif
}

void GameObject::Setup()
{
	// Create the transform after the constructor because we can't use shared_from_this() in the constructor
	m_transform = std::make_shared<Transform>(shared_from_this());
}

#pragma endregion

void GameObject::RemoveComponent(const std::shared_ptr<Component>& component)
{
	XASSERT(component != nullptr, "[GameObject::RemoveComponent] component is nullptr");

	// If the component is not already waiting for destroy
	if (component && !component->m_waitingForDestroy)
	{
		component->m_waitingForDestroy = true;
		GameplayManager::componentsToDestroy.push_back(component);

		// Remove the component from the gameobject's components list
		for (int componentIndex = 0; componentIndex < m_componentCount; componentIndex++)
		{
			if (m_components[componentIndex] == component)
			{
				m_components.erase(m_components.begin() + componentIndex);
				m_componentCount--;
				break;
			}
		}
	}
}

void GameObject::AddChild(const std::shared_ptr<GameObject>& newChild)
{
	XASSERT(newChild != nullptr, "[GameObject::AddChild] newChild is nullptr");

	if (newChild)
	{
		if (newChild->IsParentOf(shared_from_this()))
		{
			return;
		}

		// Remove the new child from his old parent's children list
		if (newChild->m_parent.lock())
		{
			std::shared_ptr<GameObject> oldParent = newChild->m_parent.lock();
			const int parentChildCount = oldParent->m_childCount;
			for (int i = 0; i < parentChildCount; i++)
			{
				if (oldParent->m_children[i].lock() == newChild)
				{
					oldParent->m_children.erase(oldParent->m_children.begin() + i);
					oldParent->m_childCount--;
					break;
				}
			}
		}

		// Check if the child to add is alrady a child of this gameobject
		bool add = true;
		for (int i = 0; i < m_childCount; i++)
		{
			if (m_children[i].lock() == newChild)
			{
				add = false;
				break;
			}
		}

		if (add)
		{
			m_children.push_back(newChild);
			m_childCount++;
			newChild->m_parent = shared_from_this();
			newChild->m_transform->OnParentChanged();
			newChild->UpdateActive(newChild);
		}
	}
}

void GameObject::SetParent(const std::shared_ptr<GameObject>& gameObject)
{
	if (gameObject)
	{
		gameObject->AddChild(shared_from_this());
	}
	else
	{
		// If the new parent is the root
		if (auto lockParent = m_parent.lock())
		{
			const int parentChildCount = lockParent->m_childCount;
			for (int i = 0; i < parentChildCount; i++)
			{
				if (lockParent->m_children[i].lock().get() == this)
				{
					lockParent->m_children.erase(lockParent->m_children.begin() + i);
					lockParent->m_childCount--;
					break;
				}
			}
		}
		m_parent.reset();
		m_transform->OnParentChanged();
		UpdateActive(shared_from_this());
	}
}

void GameObject::AddExistingComponent(const std::shared_ptr<Component>& componentToAdd)
{
	XASSERT(componentToAdd != nullptr, "[GameObject::AddExistingComponent] componentToAdd is nullptr");

	if (!componentToAdd)
		return;

	m_components.push_back(componentToAdd);
	componentToAdd->SetGameObject(shared_from_this());
	m_componentCount++;
	if ((GameplayManager::GetGameState() == GameState::Playing || GameplayManager::GetGameState() == GameState::Paused) && IsLocalActive())
	{
		componentToAdd->Awake();
		componentToAdd->m_isAwakeCalled = true;
	}
}

#pragma region Find GameObjects

std::vector<std::shared_ptr<GameObject>> FindGameObjectsByName(const std::string& name)
{
	std::vector<std::shared_ptr<GameObject>> foundGameObjects;

	//if (name == "@")
	//	return foundGameObjects;

	const std::vector<std::shared_ptr<GameObject>>& gameObjects = GameplayManager::GetGameObjects();

	const size_t gameObjectCount = gameObjects.size();

	for (size_t i = 0; i < gameObjectCount; i++)
	{
		if (gameObjects[i]->GetName() == name)
			foundGameObjects.push_back(gameObjects[i]);
	}
	return foundGameObjects;
}

std::shared_ptr<GameObject> FindGameObjectByName(const std::string& name)
{
	XASSERT(!name.empty(), "[GameObject::FindGameObjectByName] name is empty");

	const std::vector<std::shared_ptr<GameObject>>& gameObjects = GameplayManager::GetGameObjects();

	//if (name == "@")
	//	return std::shared_ptr<GameObject>();

	const size_t gameObjectCount = gameObjects.size();

	for (size_t i = 0; i < gameObjectCount; i++)
	{
		if (gameObjects[i]->GetName() == name)
			return gameObjects[i];
	}
	return std::shared_ptr<GameObject>();
}

std::shared_ptr<GameObject> FindGameObjectById(const uint64_t id)
{
	const std::vector<std::shared_ptr<GameObject>>& gameObjects = GameplayManager::GetGameObjects();

	const size_t gameObjectCount = gameObjects.size();

	for (size_t i = 0; i < gameObjectCount; i++)
	{
		if (gameObjects[i]->GetUniqueId() == id)
			return gameObjects[i];
	}
	return std::shared_ptr<GameObject>();
}

#pragma endregion

#pragma region Accessors

void GameObject::SetActive(const bool active)
{
	if (active != this->m_active)
	{
		this->m_active = active;
		UpdateActive(shared_from_this());
		GameplayManager::componentsInitListDirty = true;
	}
}

#pragma endregion

void GameObject::UpdateActive(const std::shared_ptr<GameObject>& changed)
{
	XASSERT(changed != nullptr, "[GameObject::UpdateActive] changed is empty");

	const bool lastLocalActive = m_localActive;
	if (!changed->IsActive() || (!changed->IsLocalActive() && changed.get() != this)) // if the new parent's state is false, set local active to false
	{
		m_localActive = false;
	}
	else if (m_active)
	{
		bool newActive = true;
		std::weak_ptr<GameObject> gmToCheck = m_parent;
		while (!gmToCheck.expired())
		{
			const std::shared_ptr<GameObject> gm = gmToCheck.lock();

			if (!gm->IsActive() || !gm->IsLocalActive()) // If a parent is disabled, set local active to false
			{
				newActive = false;
				break;
			}
			if (gm == changed)
			{
				break;
			}
			gmToCheck = gm->m_parent;
		}
		m_localActive = newActive;
	}

	// If the gameobject has changed his state
	if (lastLocalActive != m_localActive)
	{
		for (int i = 0; i < m_componentCount; i++)
		{
			std::shared_ptr<Component>& component = m_components[i];
			if (component)
			{
				if (m_localActive) 
				{
					component->OnEnabled();
					if (!component->m_isAwakeCalled)
					{
						component->m_isAwakeCalled = true;
						component->Awake();
					}
				}
				else 
				{
					component->OnDisabled();
				}
			}
		}

		// Update children
		for (int i = 0; i < m_childCount; i++)
		{
			m_children[i].lock()->UpdateActive(changed);
		}
	}
}

bool GameObject::IsParentOf(const std::shared_ptr<GameObject>& gameObject)
{
	if (gameObject == nullptr)
		return false;

	for (int i = 0; i < m_childCount; i++)
	{
		if (m_children[i].lock() == gameObject)
		{
			return true;
		}
		else
		{
			const bool temp = m_children[i].lock()->IsParentOf(gameObject);
			if (temp)
				return true;
		}
	}
	return false;
}

void GameObject::OnReflectionUpdated()
{
	UpdateActive(shared_from_this());
}
