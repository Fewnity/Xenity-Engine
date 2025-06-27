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
	InspectorDeleteGameObjectCommand(GameObject& gameObjectToDestroy);
	void Execute() override;
	void Undo() override;
private:
	struct GameObjectComponent
	{
		nlohmann::ordered_json componentData;
		std::string componentName = "";
		bool isEnabled = true;
		uint64_t componentId = 0;
	};

	struct GameObjectChild
	{
		nlohmann::ordered_json gameObjectData;
		nlohmann::ordered_json transformData;
		uint64_t gameObjectId = 0;
		uint64_t parentGameObjectId = 0;
		std::vector<GameObjectChild> children;
		std::vector<GameObjectComponent> components;
	};
	GameObjectChild AddChild(GameObject& child);
	void ReCreateChild(const GameObjectChild& child, const std::shared_ptr<GameObject>& parent);
	void UpdateChildComponents(const GameObjectChild& child);
	GameObjectChild m_gameObjectChild;
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
		return m_componentId;
	}
private:
	uint64_t m_gameObjectId = 0;
	uint64_t m_componentId = 0;
	nlohmann::ordered_json m_componentData;
	std::string m_componentName = "";
	bool m_isEnabled = true;
};

inline InspectorDeleteComponentCommand::InspectorDeleteComponentCommand(Component& componentToDestroy)
{
	m_componentId = componentToDestroy.GetUniqueId();
	m_gameObjectId = componentToDestroy.GetGameObject()->GetUniqueId();
	m_componentData["Values"] = ReflectionUtils::ReflectiveDataToJson(componentToDestroy.GetReflectiveData());
	m_componentName = componentToDestroy.GetComponentName();
	m_isEnabled = componentToDestroy.IsEnabled();
}

inline void InspectorDeleteComponentCommand::Execute()
{
	std::shared_ptr<Component> componentToDestroy = FindComponentById(m_componentId);
	if (componentToDestroy)
	{
		Destroy(componentToDestroy);
		SceneManager::SetIsSceneDirty(true);
	}
}

inline void InspectorDeleteComponentCommand::Undo()
{
	std::shared_ptr<GameObject> gameObject = FindGameObjectById(m_gameObjectId);
	if (gameObject)
	{
		std::shared_ptr<Component> component = ClassRegistry::AddComponentFromName(m_componentName, *gameObject);
		ReflectionUtils::JsonToReflectiveData(m_componentData, component->GetReflectiveData());
		component->SetIsEnabled(m_isEnabled);
		component->SetUniqueId(m_componentId);
		component->OnReflectionUpdated();
		SceneManager::SetIsSceneDirty(true);
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------