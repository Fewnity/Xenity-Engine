// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "component.h"

#include <engine/game_elements/gameobject.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/physics/physics_manager.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/iDrawable.h>
#include <engine/graphics/camera.h>
#include <engine/graphics/3d_graphics/lod.h>
#include <engine/lighting/lighting.h>
#include <engine/physics/rigidbody.h>
#include <engine/physics/collider.h>

#pragma region Constructors / Destructor

Component::Component(bool canBeDisabled)
{
	m_canBeDisabled = canBeDisabled;
}


Component::~Component()
{
}

#pragma endregion

void Component::SetUpdatePriority(int priority)
{
	m_updatePriority = priority;
	GameplayManager::componentsListDirty = true;
}

void Component::SetGameObject(const std::shared_ptr<GameObject>& newGameObject)
{
	XASSERT(newGameObject != nullptr, "[Component::SetGameObject] newGameObject is empty");

	// Check if the component has been just instanciated
	bool firstUse = false;
	if (m_gameObject.expired())
	{
		GameplayManager::componentsListDirty = true;
		firstUse = true;
	}

	m_gameObject = newGameObject;
	m_transform = newGameObject->GetTransform();
	m_transformRaw = newGameObject->GetTransform().get();
	m_gameObjectRaw = newGameObject.get();

	if (firstUse)
	{
		// Move this code in a OnGameObjectSet function in the specific component?
		const std::shared_ptr<Component> thisShared = shared_from_this();
		// If the component is a drawble, add to the drawable list
		if (auto result = std::dynamic_pointer_cast<IDrawable>(thisShared))
		{
			Graphics::AddDrawable(static_cast<IDrawable*>(this));
		}
		else if (auto result = std::dynamic_pointer_cast<Lod>(thisShared))
		{
			Graphics::AddLod(result);
		}
		else if (auto result = std::dynamic_pointer_cast<Light>(thisShared))
		{
			AssetManager::AddLight(result.get());
		}
		else if (auto result = std::dynamic_pointer_cast<Camera>(thisShared))
		{
			Graphics::cameras.push_back(result);
		}
		else if (auto result = std::dynamic_pointer_cast<RigidBody>(thisShared))
		{
			PhysicsManager::AddRigidBody(result.get());
		}
		else if (auto result = std::dynamic_pointer_cast<Collider>(thisShared))
		{
			PhysicsManager::AddCollider(result.get());
		}
	}

	OnComponentAttached();
}

void Component::SetIsEnabled(bool isEnabled)
{
	if (!m_canBeDisabled)
	{
		m_isEnabled = true;
		return;
	}

	if (m_isEnabled == isEnabled)
		return;

	m_isEnabled = isEnabled;

	if (!m_isAwakeCalled && isEnabled)
	{
		m_isAwakeCalled = true;
		Awake();
	}

	if (m_isEnabled)
		OnEnabled();
	else
		OnDisabled();

	GameplayManager::componentsInitListDirty = true;
}
