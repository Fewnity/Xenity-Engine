// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
* IMPORTANT: Do not store pointers to GameObjects, Components, Transforms, etc. in commands.
* This is because the pointers can become invalid if the object is deleted. Use the unique id instead.
*/

#include <memory>
#include <json.hpp>
#include <editor/command/command.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/reflection/reflection_utils.h>
#include <engine/class_registry/class_registry.h>
#include <engine/tools/gameplay_utility.h>
#include <engine/scene_management/scene_manager.h>
#include <editor/editor.h>
#include <engine/assertions/assertions.h>

class InspectorAddComponentCommand : public Command
{
public:
	InspectorAddComponentCommand() = delete;
	InspectorAddComponentCommand(const GameObject& target, const std::string& componentName);
	void Execute() override;
	void Undo() override;

	std::string componentName = "";
	uint64_t componentId = 0;
private:
	uint64_t targetId = 0;
};

inline InspectorAddComponentCommand::InspectorAddComponentCommand(const GameObject& target, const std::string& _componentName) : componentName(_componentName)
{
	targetId = target.GetUniqueId();
}

inline void InspectorAddComponentCommand::Execute()
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

inline void InspectorAddComponentCommand::Undo()
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

class InspectorCreateGameObjectCommand : public Command
{
public:
	InspectorCreateGameObjectCommand() = delete;
	InspectorCreateGameObjectCommand(const std::vector<std::weak_ptr<GameObject>>& targets, int mode);
	void Execute() override;
	void Undo() override;
	std::vector<uint64_t> createdGameObjects;
private:
	std::vector<uint64_t> targets;
	std::vector<uint64_t> oldParents;
	int mode; // 0 Create Empty, 1 Create Child, 2 Create parent
	bool alreadyExecuted = false;
};

inline InspectorCreateGameObjectCommand::InspectorCreateGameObjectCommand(const std::vector<std::weak_ptr<GameObject>>& _targets, int mode)// : targets(_targets)
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

inline void InspectorCreateGameObjectCommand::Execute()
{
	bool done = false;

	if (mode == 0)
	{
		std::shared_ptr<GameObject> newGameObject = CreateGameObject();
		if (!alreadyExecuted)
			createdGameObjects.push_back(newGameObject->GetUniqueId());
		else
			newGameObject->SetUniqueId(createdGameObjects[0]);
		done = true;
	}
	else if (mode == 1)
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
	else if (mode == 2)
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

inline void InspectorCreateGameObjectCommand::Undo()
{
	if (createdGameObjects.size() > 0)
	{
		bool done = false;
		if (mode != 2)
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