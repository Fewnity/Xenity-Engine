// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "gameplay_utility.h"

#if defined(EDITOR)
#include <editor/editor.h>
#endif

#include <engine/game_elements/gameplay_manager.h>
#include <engine/class_registry/class_registry.h>
#include <engine/reflection/reflection_utils.h>
#include <engine/accessors/acc_gameobject.h>

using json = nlohmann::json;

struct ComponentAndId
{
	std::shared_ptr<Component> newComponent = nullptr;
	uint64_t oldId = 0;
};

struct GameObjectAndId
{
	std::shared_ptr<GameObject> newGameObject = nullptr;
	uint64_t oldId = 0;
};

void DuplicateChild(const std::shared_ptr<GameObject>& parent, const std::shared_ptr<GameObject>& goToDuplicate, std::vector<ComponentAndId>& ComponentsAndIds, std::vector<GameObjectAndId>& GameObjectsAndIds)
{
	XASSERT(goToDuplicate != nullptr, "[GamePlayUtility::DuplicateChild] goToDuplicate is nullptr");

	// Create new gameobject
	std::string newGameObjectName = goToDuplicate->GetName();
#if defined(EDITOR)
	if (parent == nullptr)
		newGameObjectName = Editor::GetIncrementedGameObjectName(goToDuplicate->GetName());
#endif
	std::shared_ptr<GameObject> newGameObject = CreateGameObject();

	ReflectionUtils::ReflectiveToReflective(*goToDuplicate.get(), *newGameObject.get());
	newGameObject->SetName(newGameObjectName);

	// Set parent 
	if (parent != nullptr)
	{
		newGameObject->SetParent(parent);

		// Set local position/rotation/scale
		const std::shared_ptr<Transform>& transformToDuplicate = goToDuplicate->GetTransform();
		const std::shared_ptr<Transform>& newTransform = newGameObject->GetTransform();
		newTransform->SetLocalPosition(transformToDuplicate->GetLocalPosition());
		newTransform->SetLocalRotation(transformToDuplicate->GetLocalEulerAngles());
		newTransform->SetLocalScale(transformToDuplicate->GetLocalScale());
	}

	GameObjectAccessor goToDuplicateAcc = GameObjectAccessor(goToDuplicate);
	std::vector<std::shared_ptr<Component>>& goToDuplicateComponents = goToDuplicateAcc.GetComponents();
	const size_t componentCount = goToDuplicateComponents.size();
	for (size_t i = 0; i < componentCount; i++)
	{
		std::shared_ptr<Component> componentToDuplicate = goToDuplicateComponents[i];
		std::shared_ptr<Component> newComponent = ClassRegistry::AddComponentFromName(componentToDuplicate->GetComponentName(), *newGameObject);
		newComponent->SetIsEnabled(componentToDuplicate->IsEnabled());
		ReflectiveData newReflection = newComponent->GetReflectiveData();
		ReflectiveData reflectionToCopy = componentToDuplicate->GetReflectiveData();

		json copiedValues;
		copiedValues["Values"] = ReflectionUtils::ReflectiveDataToJson(reflectionToCopy);

		ReflectionUtils::JsonToReflectiveData(copiedValues, newReflection);
		newComponent->OnReflectionUpdated();

		ComponentAndId newComponentAndId;
		newComponentAndId.newComponent = newComponent;
		newComponentAndId.oldId = componentToDuplicate->GetUniqueId();
		ComponentsAndIds.push_back(newComponentAndId);
	}

	GameObjectAndId gameObjectAndId;
	gameObjectAndId.newGameObject = newGameObject;
	gameObjectAndId.oldId = goToDuplicate->GetUniqueId();

	GameObjectsAndIds.push_back(gameObjectAndId);

	const int childCount = goToDuplicate->GetChildrenCount();
	for (int i = 0; i < childCount; i++)
	{
		DuplicateChild(newGameObject, goToDuplicateAcc.GetChildren()[i].lock(), ComponentsAndIds, GameObjectsAndIds);
	}
}

std::shared_ptr<GameObject> Instantiate(const std::shared_ptr<GameObject>& goToDuplicate)
{
	XASSERT(goToDuplicate != nullptr, "[GamePlayUtility::Instantiate] goToDuplicate is nullptr");

	if (!goToDuplicate)
		return nullptr;

	std::vector<ComponentAndId> ComponentsAndIds;
	std::vector<GameObjectAndId> GameObjectsAndIds;
	DuplicateChild(nullptr, goToDuplicate, ComponentsAndIds, GameObjectsAndIds);

	// If a component store in a variable a component/gameobject/transform from the duplicated gameobject, replace the reference by the component/gameobject/transform of the new gameobject
	const size_t componentCount = ComponentsAndIds.size();
	const size_t gameObjectCount = GameObjectsAndIds.size();
	for (size_t componentIndex = 0; componentIndex < componentCount; componentIndex++)
	{
		const ReflectiveData newReflection = ComponentsAndIds[componentIndex].newComponent->GetReflectiveData();
		for (const ReflectiveEntry& reflectiveEntry : newReflection)
		{
			const VariableReference& variableRef = reflectiveEntry.variable.value();
			if (auto valuePtr = std::get_if<std::reference_wrapper<std::weak_ptr<Component>>>(&variableRef))
			{
				if (valuePtr->get().lock())
				{
					for (size_t j = 0; j < componentCount; j++)
					{
						if (valuePtr->get().lock()->GetUniqueId() == ComponentsAndIds[j].oldId)
						{
							valuePtr->get() = ComponentsAndIds[j].newComponent;
						}
					}
				}
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::weak_ptr<GameObject>>>(&variableRef))
			{
				if (valuePtr->get().lock())
				{
					for (size_t j = 0; j < gameObjectCount; j++)
					{
						if (valuePtr->get().lock()->GetUniqueId() == GameObjectsAndIds[j].oldId)
						{
							valuePtr->get() = GameObjectsAndIds[j].newGameObject;
						}
					}
				}
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::weak_ptr<Transform>>>(&variableRef))
			{
				if (valuePtr->get().lock())
				{
					for (size_t j = 0; j < gameObjectCount; j++)
					{
						if (valuePtr->get().lock()->GetGameObject()->GetUniqueId() == GameObjectsAndIds[j].oldId)
						{
							valuePtr->get() = GameObjectsAndIds[j].newGameObject->GetTransform();
						}
					}
				}
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::weak_ptr<Component>>>>(&variableRef))
			{
				const size_t vectorSize = valuePtr->get().size();
				for (size_t vectorIndex = 0; vectorIndex < vectorSize; vectorIndex++)
				{
					for (size_t j = 0; j < componentCount; j++)
					{
						if (valuePtr->get()[vectorIndex].lock()->GetUniqueId() == ComponentsAndIds[j].oldId)
						{
							valuePtr->get()[vectorIndex] = ComponentsAndIds[j].newComponent;
						}
					}
				}
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::weak_ptr<GameObject>>>>(&variableRef))
			{
				const size_t vectorSize = valuePtr->get().size();
				for (size_t vectorIndex = 0; vectorIndex < vectorSize; vectorIndex++)
				{
					for (size_t j = 0; j < gameObjectCount; j++)
					{
						if (valuePtr->get()[vectorIndex].lock()->GetUniqueId() == GameObjectsAndIds[j].oldId)
						{
							valuePtr->get()[vectorIndex] = GameObjectsAndIds[j].newGameObject;
						}
					}
				}
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::weak_ptr<Transform>>>>(&variableRef))
			{
				const size_t vectorSize = valuePtr->get().size();
				for (size_t vectorIndex = 0; vectorIndex < vectorSize; vectorIndex++)
				{
					for (size_t j = 0; j < gameObjectCount; j++)
					{
						if (valuePtr->get()[vectorIndex].lock()->GetGameObject()->GetUniqueId() == GameObjectsAndIds[j].oldId)
						{
							valuePtr->get()[vectorIndex] = GameObjectsAndIds[j].newGameObject->GetTransform();
						}
					}
				}
			}
		}
	}

	return GameObjectsAndIds[0].newGameObject;
}

void DestroyGameObjectAndChild(const std::shared_ptr<GameObject>& gameObject)
{
	XASSERT(gameObject != nullptr, "[GamePlayUtility::DestroyGameObjectAndChild] gameObject is nullptr");

	GameplayManager::gameObjectsToDestroy.push_back(gameObject);
	GameObjectAccessor gameObjectAcc = GameObjectAccessor(gameObject);
	gameObjectAcc.SetWaitingForDestroy(true);

	// Remove the destroyed gameobject from his parent's children list
	if (auto parent = gameObject->GetParent().lock())
	{
		const int parentChildCount = parent->GetChildrenCount();
		GameObjectAccessor parentAcc = GameObjectAccessor(parent);
		std::vector<std::weak_ptr<GameObject>>& parentChildren = parentAcc.GetChildren();

		for (int i = 0; i < parentChildCount; i++)
		{
			if (parentChildren[i].lock() == gameObject)
			{
				parentChildren.erase(parentChildren.begin() + i);
				parentAcc.SetChildrenCount(parent->GetChildrenCount() - 1);
				break;
			}
		}
	}

	std::vector<std::weak_ptr<GameObject>>& gameObjectChildren = gameObjectAcc.GetChildren();

	int childCount = gameObject->GetChildrenCount();
	for (int i = 0; i < childCount; i++)
	{
		DestroyGameObjectAndChild(gameObjectChildren[0].lock());
		i--;
		childCount--;
	}
}


void Destroy(const std::weak_ptr<GameObject>& gameObject)
{
	Destroy(gameObject.lock());
}

void Destroy(const std::shared_ptr<GameObject>& gameObject)
{
	GameObjectAccessor gameObjectAcc = GameObjectAccessor(gameObject);
	if (gameObject && !gameObjectAcc.IsWaitingForDestroy())
	{
		DestroyGameObjectAndChild(gameObject);
	}
}