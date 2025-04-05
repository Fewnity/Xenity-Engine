// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
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

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

class InspectorDeleteGameObjectCommand : public Command
{
public:
	InspectorDeleteGameObjectCommand() = delete;
	InspectorDeleteGameObjectCommand(std::weak_ptr<GameObject>& gameObjectToDestroy);
	void Execute() override;
	void Undo() override;
private:
	struct GameObjectComponent
	{
		nlohmann::json componentData;
		std::string componentName = "";
		bool isEnabled = true;
		uint64_t componentId = 0;
	};

	struct GameObjectChild
	{
		nlohmann::json gameObjectData;
		nlohmann::json transformData;
		uint64_t gameObjectId = 0;
		uint64_t parentGameObjectId = 0;
		std::vector<GameObjectChild> children;
		std::vector<GameObjectComponent> components;
	};
	GameObjectChild AddChild(const std::shared_ptr<GameObject>& child);
	void ReCreateChild(const GameObjectChild& child, const std::shared_ptr<GameObject>& parent);
	void UpdateChildComponents(const GameObjectChild& child);
	GameObjectChild gameObjectChild;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

class InspectorDeleteComponentCommand : public Command
{
public:
	InspectorDeleteComponentCommand() = delete;
	InspectorDeleteComponentCommand(Component& componentToDestroy);
	void Execute() override;
	void Undo() override;
	[[nodiscard]] uint64_t GetComponentId() const
	{
		return componentId;
	}
private:
	uint64_t gameObjectId = 0;
	uint64_t componentId = 0;
	nlohmann::json componentData;
	std::string componentName = "";
	bool isEnabled = true;
};

inline InspectorDeleteComponentCommand::InspectorDeleteComponentCommand(Component& componentToDestroy)
{
	this->componentId = componentToDestroy.GetUniqueId();
	this->gameObjectId = componentToDestroy.GetGameObject()->GetUniqueId();
	this->componentData["Values"] = ReflectionUtils::ReflectiveDataToJson(componentToDestroy.GetReflectiveData());
	this->componentName = componentToDestroy.GetComponentName();
	isEnabled = componentToDestroy.IsEnabled();
}

inline void InspectorDeleteComponentCommand::Execute()
{
	std::shared_ptr<Component> componentToDestroy = FindComponentById(componentId);
	if (componentToDestroy)
	{
		Destroy(componentToDestroy);
		SceneManager::SetSceneModified(true);
	}
}

inline void InspectorDeleteComponentCommand::Undo()
{
	std::shared_ptr<GameObject> gameObject = FindGameObjectById(gameObjectId);
	if (gameObject)
	{
		std::shared_ptr<Component> component = ClassRegistry::AddComponentFromName(componentName, *gameObject);
		ReflectionUtils::JsonToReflectiveData(componentData, component->GetReflectiveData());
		component->SetIsEnabled(isEnabled);
		component->SetUniqueId(componentId);
		component->OnReflectionUpdated();
		SceneManager::SetSceneModified(true);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------