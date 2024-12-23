// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "create.h"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

InspectorAddComponentCommand::InspectorAddComponentCommand(const GameObject& target, const std::string& _componentName) : componentName(_componentName)
{
	targetId = target.GetUniqueId();
}

void InspectorAddComponentCommand::Execute()
{
	const std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
	if (foundGameObject)
	{
		const std::shared_ptr<Component> newComponent = ClassRegistry::AddComponentFromName(componentName, *foundGameObject);
		if (newComponent)
		{
			componentId = newComponent->GetUniqueId();
			SceneManager::SetSceneModified(true);
		}
		else
		{
			componentId = 0;
		}
	}
}

void InspectorAddComponentCommand::Undo()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(targetId);
	if (foundGameObject && componentId != 0)
	{
		std::shared_ptr<Component> oldComponent = FindComponentById(componentId);
		if (oldComponent)
		{
			Destroy(oldComponent);
			SceneManager::SetSceneModified(true);
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

InspectorCreateGameObjectCommand::InspectorCreateGameObjectCommand(const std::vector<std::weak_ptr<GameObject>>& _targets, CreateGameObjectMode mode)// : targets(_targets)
{
	this->mode = mode;

	for (std::weak_ptr<GameObject> weakTarget : _targets)
	{
		if (auto target = weakTarget.lock())
		{
			targets.push_back(target->GetUniqueId());
		}
	}
}

void InspectorCreateGameObjectCommand::Execute()
{
	bool done = false;

	if (mode == CreateGameObjectMode::CreateEmpty)
	{
		std::shared_ptr<GameObject> newGameObject = CreateGameObject();
		if (!alreadyExecuted)
			createdGameObjects.push_back(newGameObject->GetUniqueId());
		else
			newGameObject->SetUniqueId(createdGameObjects[0]);
		done = true;
	}
	else if (mode == CreateGameObjectMode::CreateChild)
	{
		const size_t targetCount = targets.size();
		for (size_t i = 0; i < targetCount; i++)
		{
			std::shared_ptr<GameObject> target = FindGameObjectById(targets[i]);
			if (target)
			{
				std::shared_ptr<GameObject> newGameObject = CreateGameObject();
				if (!alreadyExecuted)
					createdGameObjects.push_back(newGameObject->GetUniqueId());
				else
					newGameObject->SetUniqueId(createdGameObjects[i]);
				std::shared_ptr<Transform> transform = newGameObject->GetTransform();
				newGameObject->SetParent(target);
				transform->SetLocalPosition(Vector3(0));
				transform->SetLocalRotation(Vector3(0));
				transform->SetLocalScale(Vector3(1));
				done = true;
			}
		}
	}
	else if (mode == CreateGameObjectMode::CreateParent)
	{
		const size_t targetCount = targets.size();
		for (size_t i = 0; i < targetCount; i++)
		{
			std::shared_ptr<GameObject> target = FindGameObjectById(targets[i]);
			if (target)
			{
				std::shared_ptr<GameObject> newGameObject = CreateGameObject();
				if (!alreadyExecuted)
					createdGameObjects.push_back(newGameObject->GetUniqueId());
				else
					newGameObject->SetUniqueId(createdGameObjects[i]);
				std::shared_ptr<Transform> transform = newGameObject->GetTransform();
				std::shared_ptr<Transform> selectedTransform = target->GetTransform();
				transform->SetPosition(selectedTransform->GetPosition());
				transform->SetRotation(selectedTransform->GetEulerAngles());
				transform->SetLocalScale(selectedTransform->GetScale());

				if (target->GetParent().lock())
				{
					newGameObject->SetParent(target->GetParent().lock());
				}
				if (target->GetParent().lock())
					oldParents.push_back(target->GetParent().lock()->GetUniqueId());
				else
					oldParents.push_back(0);

				target->SetParent(newGameObject);
				done = true;
			}
		}
	}

	alreadyExecuted = true;

	if (done)
	{
		Editor::ClearSelectedGameObjects();
		for (uint64_t createdGameObjectId : createdGameObjects)
		{
			std::shared_ptr<GameObject> createdGameObject = FindGameObjectById(createdGameObjectId);
			if (createdGameObject)
				Editor::AddSelectedGameObject(createdGameObject);
		}
		Editor::SetSelectedFileReference(nullptr);
		SceneManager::SetSceneModified(true);
	}
}

void InspectorCreateGameObjectCommand::Undo()
{
	if (createdGameObjects.size() > 0)
	{
		bool done = false;
		if (mode != CreateGameObjectMode::CreateParent)
		{
			for (uint64_t gameObjectId : createdGameObjects)
			{
				std::shared_ptr<GameObject> createdGameObject = FindGameObjectById(gameObjectId);
				Destroy(createdGameObject);
			}
			done = true;
		}
		else
		{
			const size_t targetCount = targets.size();
			for (size_t i = 0; i < targetCount; i++)
			{
				std::shared_ptr<GameObject> target = FindGameObjectById(targets[i]);
				if (oldParents[i] != 0)
				{
					std::shared_ptr<GameObject> oldParent = FindGameObjectById(oldParents[i]);
					target->SetParent(oldParent);
				}
				else
					target->SetParent(nullptr);

				std::shared_ptr<GameObject> createdGameObject = FindGameObjectById(createdGameObjects[i]);
				Destroy(createdGameObject);
				done = true;
			}
		}

		if (done)
			SceneManager::SetSceneModified(true);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------