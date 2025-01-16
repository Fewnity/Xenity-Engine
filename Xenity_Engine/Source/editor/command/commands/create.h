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

#include <editor/editor.h>

#include <editor/command/command.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/class_registry/class_registry.h>
#include <engine/tools/gameplay_utility.h>
#include <engine/scene_management/scene_manager.h>
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

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

ENUM(CreateGameObjectMode, CreateEmpty, CreateChild, CreateParent);

class InspectorCreateGameObjectCommand : public Command
{
public:
	InspectorCreateGameObjectCommand() = delete;
	InspectorCreateGameObjectCommand(const std::vector<std::weak_ptr<GameObject>>& targets, CreateGameObjectMode mode);
	void Execute() override;
	void Undo() override;
	std::vector<uint64_t> createdGameObjects;
private:
	std::vector<uint64_t> targets;
	std::vector<uint64_t> oldParents;
	CreateGameObjectMode mode; // 0 Create Empty, 1 Create Child, 2 Create parent
	bool alreadyExecuted = false;
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------