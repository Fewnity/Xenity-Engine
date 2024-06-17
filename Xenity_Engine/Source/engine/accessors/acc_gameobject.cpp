// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "acc_gameobject.h"

#include <engine/game_elements/gameobject.h>	

std::vector<std::shared_ptr<Component>>& GameObjectAccessor::GetComponents()
{
	return gameObject->components;
}

std::vector<std::weak_ptr<GameObject>>& GameObjectAccessor::GetChildren()
{
	return gameObject->children;
}

void GameObjectAccessor::RemoveComponent(const std::shared_ptr<Component>& component)
{
	gameObject->RemoveComponent(component);
}

void GameObjectAccessor::Setup()
{
	gameObject->Setup();
}

bool GameObjectAccessor::IsWaitingForDestroy()
{
	return gameObject->waitingForDestroy;
}

void GameObjectAccessor::SetWaitingForDestroy(bool waitingForDestroy)
{
	gameObject->waitingForDestroy = waitingForDestroy;
}

void GameObjectAccessor::SetChildrenCount(int count)
{
	gameObject->childCount = count;
}
