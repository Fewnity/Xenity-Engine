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
#include <iostream>
#include <engine/accessors/acc_gameobject.h>


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

std::shared_ptr<GameObject> CreateGameObjectEditor(const std::string& name)
{
	std::shared_ptr<GameObject> newGameObject = std::make_shared<GameObject>(name);
	GameplayManager::AddGameObjectEditor(newGameObject);
	GameObjectAccessor gameObjectAcc = GameObjectAccessor(newGameObject);
	gameObjectAcc.Setup();
	return newGameObject;
}

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
	this->name = DEFAULT_GAMEOBJECT_NAME;
}

GameObject::GameObject(const std::string& _name)
{
	if (!_name.empty())
		this->name = _name;
	else
		this->name = DEFAULT_GAMEOBJECT_NAME;
}

ReflectiveData GameObject::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, name, "name", true);
	Reflective::AddVariable(reflectedVariables, active, "active", true);
	Reflective::AddVariable(reflectedVariables, isStatic, "isStatic", true);
	return reflectedVariables;
}

GameObject::~GameObject()
{
	for (int i = 0; i < componentCount; i++)
	{
		if (components[i])
			components[i]->RemoveReferences();
	}
	components.clear();
}

void GameObject::Setup()
{
	transform = std::make_shared<Transform>(shared_from_this());
}

#pragma endregion

void GameObject::RemoveComponent(const std::shared_ptr<Component>& component)
{
	XASSERT(component != nullptr, "[GameObject::RemoveComponent] component is nullptr");

	// If the component is not already waiting for destroy
	if (component && !component->waitingForDestroy)
	{
		component->waitingForDestroy = true;
		GameplayManager::componentsToDestroy.push_back(component);

		// Remove the component from the gameobject's components list
		for (int componentIndex = 0; componentIndex < componentCount; componentIndex++)
		{
			if (components[componentIndex] == component)
			{
				components.erase(components.begin() + componentIndex);
				componentCount--;
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
		if (newChild->parent.lock())
		{
			std::shared_ptr<GameObject> oldParent = newChild->parent.lock();
			const int parentChildCount = oldParent->childCount;
			for (int i = 0; i < parentChildCount; i++)
			{
				if (oldParent->children[i].lock() == newChild)
				{
					oldParent->children.erase(oldParent->children.begin() + i);
					oldParent->childCount--;
					break;
				}
			}
		}

		// Check if the child to add is alrady a child of this gameobject
		bool add = true;
		for (int i = 0; i < childCount; i++)
		{
			if (children[i].lock() == newChild)
			{
				add = false;
				break;
			}
		}

		if (add)
		{
			children.push_back(newChild);
			childCount++;
			newChild->parent = shared_from_this();
			newChild->transform->OnParentChanged();
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
		if (auto lockParent = parent.lock())
		{
			const int parentChildCount = lockParent->childCount;
			for (int i = 0; i < parentChildCount; i++)
			{
				if (lockParent->children[i].lock() == shared_from_this())
				{
					lockParent->children.erase(lockParent->children.begin() + i);
					lockParent->childCount--;
					break;
				}
			}
		}
		parent.reset();
		transform->OnParentChanged();
		UpdateActive(shared_from_this());
	}
}

void GameObject::AddExistingComponent(const std::shared_ptr<Component>& componentToAdd)
{
	XASSERT(componentToAdd != nullptr, "[GameObject::AddExistingComponent] componentToAdd is nullptr");

	if (!componentToAdd.get())
		return;

	components.push_back(componentToAdd);
	componentToAdd->SetGameObject(shared_from_this());
	componentCount++;
	if (GameplayManager::GetGameState() == GameState::Playing && IsLocalActive())
	{
		componentToAdd->Awake();
		componentToAdd->isAwakeCalled = true;
	}
}

#pragma region Find GameObjects

std::vector<std::shared_ptr<GameObject>> FindGameObjectsByName(const std::string& name)
{
	std::vector<std::shared_ptr<GameObject>> foundGameObjects;

	if (name == "@")
		return foundGameObjects;

	const std::vector<std::shared_ptr<GameObject>>& gameObjects = GameplayManager::GetGameObjects();

	const int gameObjectCount = (int)gameObjects.size();

	for (int i = 0; i < gameObjectCount; i++)
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

	if (name == "@")
		return std::shared_ptr<GameObject>();

	const int gameObjectCount = (int)gameObjects.size();

	for (int i = 0; i < gameObjectCount; i++)
	{
		if (gameObjects[i]->GetName() == name)
			return gameObjects[i];
	}
	return std::shared_ptr<GameObject>();
}

std::shared_ptr<GameObject> FindGameObjectById(const uint64_t id)
{
	const std::vector<std::shared_ptr<GameObject>>& gameObjects = GameplayManager::GetGameObjects();

	const int gameObjectCount = (int)gameObjects.size();

	for (int i = 0; i < gameObjectCount; i++)
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
	if (active != this->active)
	{
		this->active = active;
		UpdateActive(shared_from_this());
		GameplayManager::componentsInitListDirty = true;
	}
}

#pragma endregion

void GameObject::UpdateActive(const std::shared_ptr<GameObject>& changed)
{
	XASSERT(changed != nullptr, "[GameObject::UpdateActive] changed is empty");

	const bool lastLocalActive = localActive;
	if (!changed->IsActive() || (!changed->IsLocalActive() && changed != shared_from_this())) // if the new parent's state is false, set local active to false
	{
		localActive = false;
	}
	else if (active)
	{
		bool newActive = true;
		std::weak_ptr<GameObject> gmToCheck = parent;
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
			gmToCheck = gm->parent;
		}
		localActive = newActive;
	}

	// If the gameobject has changed his state
	if (lastLocalActive != localActive)
	{
		for (int i = 0; i < componentCount; i++)
		{
			if (components[i])
			{
				if (localActive)
					components[i]->OnEnabled();
				else
					components[i]->OnDisabled();
			}
		}
		// Update children
		for (int i = 0; i < childCount; i++)
		{
			children[i].lock()->UpdateActive(changed);
		}
	}
}

bool GameObject::IsParentOf(const std::shared_ptr<GameObject>& gameObject)
{
	if (gameObject == nullptr)
		return false;

	for (int i = 0; i < childCount; i++)
	{
		if (children[i].lock() == gameObject)
		{
			return true;
		}
		else
		{
			const bool temp = children[i].lock()->IsParentOf(gameObject);
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
