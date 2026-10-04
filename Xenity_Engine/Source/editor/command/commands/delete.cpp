// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2026 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "delete.h"

#include <engine/debug/debug.h>

InspectorDeleteGameObjectCommand::GameObjectChild InspectorDeleteGameObjectCommand::AddChild(GameObject& child)
{
	GameObjectChild gameObjectChild;
	gameObjectChild.gameObjectId = child.GetUniqueId();
	gameObjectChild.parentGameObjectId = child.GetParent().lock() ? child.GetParent().lock()->GetUniqueId() : 0;
	gameObjectChild.gameObjectData["Values"] = ReflectionUtils::ReflectiveDataToJson(child.GetReflectiveData());
	gameObjectChild.transformData["Values"] = ReflectionUtils::ReflectiveDataToJson(child.GetTransform()->GetReflectiveData());
	for (std::weak_ptr<GameObject> childChild : child.GetChildren())
	{
		if (const std::shared_ptr<GameObject> childChildLock = childChild.lock())
		{
			gameObjectChild.children.push_back(AddChild(*childChildLock));
		}
	}

	for (std::shared_ptr<Component> component : child.m_components)
	{
		GameObjectComponent gameObjectComponent;
		gameObjectComponent.componentData["Values"] = ReflectionUtils::ReflectiveDataToJson(component->GetReflectiveData());
		gameObjectComponent.componentName = component->GetComponentName();
		gameObjectComponent.componentId = component->GetUniqueId();
		gameObjectComponent.isEnabled = component->IsEnabled();
		gameObjectChild.components.push_back(gameObjectComponent);
	}
	return gameObjectChild;
}

void InspectorDeleteGameObjectCommand::UpdateChildComponents(const GameObjectChild& child)
{
	for (const GameObjectChild& childChild : child.children)
	{
		UpdateChildComponents(childChild);
	}
	for (const GameObjectComponent& childChild : child.components)
	{
		std::shared_ptr<Component> component = FindComponentById(childChild.componentId);
		// The component may not have been recreated (unknown class, script not compiled)
		if (!component)
			continue;

		ReflectionUtils::JsonToReflectiveData(childChild.componentData, component->GetReflectiveData());
		component->OnReflectionUpdated();
	}
}

void InspectorDeleteGameObjectCommand::ReCreateChild(const GameObjectChild& child, const std::shared_ptr<GameObject>& parent)
{
	std::shared_ptr<GameObject> newGameObject = CreateGameObject();
	std::shared_ptr<Transform> transformToUpdate = newGameObject->GetTransform();
	ReflectionUtils::JsonToReflectiveData(child.gameObjectData, newGameObject->GetReflectiveData());
	newGameObject->OnReflectionUpdated();
	newGameObject->SetUniqueId(child.gameObjectId);
	if (parent)
	{
		newGameObject->SetParent(parent);
	}
	ReflectionUtils::JsonToReflectiveData(child.transformData, transformToUpdate->GetReflectiveData());
	transformToUpdate->m_isTransformationMatrixDirty = true;
	transformToUpdate->UpdateWorldValues();
	transformToUpdate->OnReflectionUpdated();
	for (const GameObjectChild& childChild : child.children)
	{
		ReCreateChild(childChild, newGameObject);
	}
	for (const GameObjectComponent& childChild : child.components)
	{
		std::shared_ptr<Component> component = ClassRegistry::AddComponentFromName(childChild.componentName, *newGameObject);
		if (!component)
		{
			Debug::PrintError("[InspectorDeleteGameObjectCommand::ReCreateChild] Failed to recreate component: " + childChild.componentName, true);
			continue;
		}

		//ReflectionUtils::JsonToReflectiveData(childChild.componentData, component->GetReflectiveData());
		component->SetIsEnabled(childChild.isEnabled);
		component->SetUniqueId(childChild.componentId);

		//child.componentData["Values"] = ReflectionUtils::ReflectiveDataToJson(componentToUse.lock()->GetReflectiveData());
		//this->componentName = componentToUse.lock()->GetComponentName();
	}
}

InspectorDeleteGameObjectCommand::InspectorDeleteGameObjectCommand(GameObject& gameObjectToDestroy)
{
	m_gameObjectChild = AddChild(gameObjectToDestroy);
	//this->gameObjectId = gameObjectToDestroyLock->GetUniqueId();
	//this->gameObjectData["Values"] = ReflectionUtils::ReflectiveDataToJson(gameObjectToDestroyLock->GetReflectiveData());
	//this->componentName = componentToDestroyLock->GetComponentName();
	//isEnabled = componentToDestroyLock->IsEnabled();
}

void InspectorDeleteGameObjectCommand::Execute()
{
	std::shared_ptr<GameObject> gameObjectToDestroy = FindGameObjectById(m_gameObjectChild.gameObjectId);
	if (gameObjectToDestroy)
	{
		Destroy(gameObjectToDestroy);
		SceneManager::SetIsSceneDirty(true);
	}
}

void InspectorDeleteGameObjectCommand::Undo()
{
	// Do not recreate the GameObject if it already exists, it would create two GameObjects with the same id
	if (FindGameObjectById(m_gameObjectChild.gameObjectId))
	{
		Debug::PrintWarning("[InspectorDeleteGameObjectCommand::Undo] The GameObject already exists");
		return;
	}

	std::shared_ptr<GameObject> parentGameObject = nullptr;
	if (m_gameObjectChild.parentGameObjectId != 0)
	{
		parentGameObject = FindGameObjectById(m_gameObjectChild.parentGameObjectId);
	}

	ReCreateChild(m_gameObjectChild, parentGameObject);
	UpdateChildComponents(m_gameObjectChild);
	/*std::shared_ptr<GameObject> gameObject = CreateGameObject();
	ReflectionUtils::JsonToReflectiveData(gameObjectData, gameObject->GetReflectiveData());
	gameObject->OnReflectionUpdated();
	gameObject->SetUniqueId(gameObjectId);
	SceneManager::SetIsSceneDirty(true);*/
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------