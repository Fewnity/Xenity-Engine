// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "create.h"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

InspectorAddComponentCommand::InspectorAddComponentCommand(const GameObject& target, const std::string& _componentName) : componentName(_componentName)
{
	m_targetId = target.GetUniqueId();
}

void InspectorAddComponentCommand::Execute()
{
	const std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(m_targetId);
	if (foundGameObject)
	{
		const std::shared_ptr<Component> newComponent = ClassRegistry::AddComponentFromName(componentName, *foundGameObject);
		if (newComponent)
		{
			if (componentId != 0)
			{
				newComponent->SetUniqueId(componentId);
			}
			else 
			{
				componentId = newComponent->GetUniqueId();
			}
			SceneManager::SetIsSceneDirty(true);
		}
		else
		{
			componentId = 0;
		}
	}
}

void InspectorAddComponentCommand::Undo()
{
	std::shared_ptr<GameObject> foundGameObject = FindGameObjectById(m_targetId);
	if (foundGameObject && componentId != 0)
	{
		std::shared_ptr<Component> oldComponent = FindComponentById(componentId);
		if (oldComponent)
		{
			Destroy(oldComponent);
			SceneManager::SetIsSceneDirty(true);
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

InspectorCreateGameObjectCommand::InspectorCreateGameObjectCommand(const std::vector<std::weak_ptr<GameObject>>& _targets, CreateGameObjectMode mode)// : targets(_targets)
{
	m_mode = mode;

	for (std::weak_ptr<GameObject> weakTarget : _targets)
	{
		if (auto target = weakTarget.lock())
		{
			m_targets.push_back(target->GetUniqueId());
		}
	}
}

void InspectorCreateGameObjectCommand::Execute()
{
	bool done = false;

	if (m_mode == CreateGameObjectMode::CreateEmpty)
	{
		std::shared_ptr<GameObject> newGameObject = CreateGameObject();
		if (!m_alreadyExecuted)
			createdGameObjects.push_back(newGameObject->GetUniqueId());
		else
			newGameObject->SetUniqueId(createdGameObjects[0]);
		done = true;
	}
	else if (m_mode == CreateGameObjectMode::CreateChild)
	{
		const size_t targetCount = m_targets.size();
		for (size_t i = 0; i < targetCount; i++)
		{
			std::shared_ptr<GameObject> target = FindGameObjectById(m_targets[i]);
			if (target)
			{
				std::shared_ptr<GameObject> newGameObject = CreateGameObject();
				if (!m_alreadyExecuted)
					createdGameObjects.push_back(newGameObject->GetUniqueId());
				else
					newGameObject->SetUniqueId(createdGameObjects[i]);
				std::shared_ptr<Transform> transform = newGameObject->GetTransform();
				newGameObject->SetParent(target);
				transform->SetLocalPosition(Vector3(0));
				transform->SetLocalEulerAngles(Vector3(0));
				transform->SetLocalScale(Vector3(1));
				done = true;
			}
		}
	}
	else if (m_mode == CreateGameObjectMode::CreateParent)
	{
		const size_t targetCount = m_targets.size();
		for (size_t i = 0; i < targetCount; i++)
		{
			std::shared_ptr<GameObject> target = FindGameObjectById(m_targets[i]);
			if (target)
			{
				std::shared_ptr<GameObject> newGameObject = CreateGameObject();
				if (!m_alreadyExecuted)
					createdGameObjects.push_back(newGameObject->GetUniqueId());
				else
					newGameObject->SetUniqueId(createdGameObjects[i]);
				std::shared_ptr<Transform> transform = newGameObject->GetTransform();
				std::shared_ptr<Transform> selectedTransform = target->GetTransform();
				transform->SetPosition(selectedTransform->GetPosition());
				transform->SetEulerAngles(selectedTransform->GetEulerAngles());
				transform->SetLocalScale(selectedTransform->GetScale());

				if (target->GetParent().lock())
				{
					newGameObject->SetParent(target->GetParent().lock());
				}
				if (target->GetParent().lock())
					m_oldParents.push_back(target->GetParent().lock()->GetUniqueId());
				else
					m_oldParents.push_back(0);

				target->SetParent(newGameObject);
				done = true;
			}
		}
	}

	m_alreadyExecuted = true;

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
		SceneManager::SetIsSceneDirty(true);
	}
}

void InspectorCreateGameObjectCommand::Undo()
{
	if (createdGameObjects.size() > 0)
	{
		bool done = false;
		if (m_mode != CreateGameObjectMode::CreateParent)
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
			const size_t targetCount = m_targets.size();
			for (size_t i = 0; i < targetCount; i++)
			{
				std::shared_ptr<GameObject> target = FindGameObjectById(m_targets[i]);
				if (m_oldParents[i] != 0)
				{
					std::shared_ptr<GameObject> oldParent = FindGameObjectById(m_oldParents[i]);
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
			SceneManager::SetIsSceneDirty(true);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------