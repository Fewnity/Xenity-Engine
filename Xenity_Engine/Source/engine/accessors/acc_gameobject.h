// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <memory>
#include <vector>
#include <engine/game_elements/gameobject.h>	

class Component;

/**
* @brief class to access GameObject private members
*/
class GameObjectAccessor
{
public:
	GameObjectAccessor() = delete;
	inline explicit GameObjectAccessor(const std::shared_ptr<GameObject>& gameObject)
	{
		this->gameObject = gameObject;
	}

	inline std::vector<std::shared_ptr<Component>>& GetComponents()
	{
		return gameObject->m_components;
	}

	inline std::vector<std::weak_ptr<GameObject>>& GetChildren()
	{
		return gameObject->m_children;
	}

	inline void RemoveComponent(const std::shared_ptr<Component>& component)
	{
		gameObject->RemoveComponent(component);
	}

	inline void Setup()
	{
		gameObject->Setup();
	}

	inline bool IsWaitingForDestroy()
	{
		return gameObject->m_waitingForDestroy;
	}

	inline void SetWaitingForDestroy(bool waitingForDestroy)
	{
		gameObject->m_waitingForDestroy = waitingForDestroy;
	}

	inline void SetChildrenCount(int count)
	{
		gameObject->m_childCount = count;
	}


private:
	std::shared_ptr<GameObject> gameObject;
};

