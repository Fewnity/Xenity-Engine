// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "delete.h"

InspectorDeleteGameObjectCommand::GameObjectChild InspectorDeleteGameObjectCommand::AddChild(const std::shared_ptr<GameObject>& child)
{
	GameObjectChild gameObjectChild;
	gameObjectChild.gameObjectId = child->GetUniqueId();
	gameObjectChild.gameObjectData["Values"] = ReflectionUtils::ReflectiveDataToJson(child->GetReflectiveData());
	gameObjectChild.transformData["Values"] = ReflectionUtils::ReflectiveDataToJson(child->GetTransform()->GetReflectiveData());
	for (std::weak_ptr<GameObject> childChild : child->GetChildren())
	{
		gameObjectChild.children.push_back(AddChild(childChild.lock()));
	}

	for (std::shared_ptr<Component> component : child->m_components)
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
		newGameObject->SetParent(parent);
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
		//ReflectionUtils::JsonToReflectiveData(childChild.componentData, component->GetReflectiveData());
		component->SetIsEnabled(childChild.isEnabled);
		component->SetUniqueId(childChild.componentId);

		//child.componentData["Values"] = ReflectionUtils::ReflectiveDataToJson(componentToUse.lock()->GetReflectiveData());
		//this->componentName = componentToUse.lock()->GetComponentName();
	}
}

InspectorDeleteGameObjectCommand::InspectorDeleteGameObjectCommand(std::weak_ptr<GameObject>& gameObjectToDestroy)
{
	std::shared_ptr<GameObject> gameObjectToDestroyLock = gameObjectToDestroy.lock();
	gameObjectChild = AddChild(gameObjectToDestroyLock);
	//this->gameObjectId = gameObjectToDestroyLock->GetUniqueId();
	//this->gameObjectData["Values"] = ReflectionUtils::ReflectiveDataToJson(gameObjectToDestroyLock->GetReflectiveData());
	//this->componentName = componentToDestroyLock->GetComponentName();
	//isEnabled = componentToDestroyLock->IsEnabled();
}

void InspectorDeleteGameObjectCommand::Execute()
{
	std::shared_ptr<GameObject> gameObjectToDestroy = FindGameObjectById(gameObjectChild.gameObjectId);
	if (gameObjectToDestroy)
	{
		Destroy(gameObjectToDestroy);
		SceneManager::SetSceneModified(true);
	}
}

void InspectorDeleteGameObjectCommand::Undo()
{
	ReCreateChild(gameObjectChild, nullptr);
	UpdateChildComponents(gameObjectChild);
	/*std::shared_ptr<GameObject> gameObject = CreateGameObject();
	ReflectionUtils::JsonToReflectiveData(gameObjectData, gameObject->GetReflectiveData());
	gameObject->OnReflectionUpdated();
	gameObject->SetUniqueId(gameObjectId);
	SceneManager::SetSceneModified(true);*/
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------