// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <memory>

#include <engine/component.h>
#include <engine/api.h>

class RigidBody;
class btCollisionObject;
class btCollisionShape;

class API Collider : public Component
{
public:
	~Collider();
	void SetIsTrigger(bool isTrigger)
	{
		m_isTrigger = isTrigger;
		OnTransformScaled();
		OnTransformUpdated();
	}

	[[nodiscard]] bool IsTrigger() const
	{
		return m_isTrigger;
	}

	void SetGenerateCollisionEvents(bool generateCollisionEvents)
	{
		m_generateCollisionEvents = generateCollisionEvents;
	}

	[[nodiscard]] bool GetGenerateCollisionEvents() const
	{
		return m_generateCollisionEvents;
	}

protected:
	friend class PhysicsManager;
	friend class RigidBody;
	friend class InspectorMenu;
	friend class MainBarMenu;

	[[nodiscard]] const std::weak_ptr<RigidBody>& GetAttachedRigidbody() const
	{
		return m_attachedRigidbody;
	}

	void FindRigidbody();
	void SetRigidbody(const std::shared_ptr<RigidBody>& rb);
	void OnEnabled() override;
	void OnDisabled() override;
	virtual void SetDefaultSize() {}
	void RemoveReferences()  override;
	virtual void CreateCollision(bool forceCreation) = 0;
	virtual void OnTransformScaled() {};
	virtual void OnTransformUpdated() {};

	std::weak_ptr<RigidBody> m_attachedRigidbody;
	btCollisionObject* m_bulletCollisionObject = nullptr;
	btCollisionShape* m_bulletCollisionShape = nullptr;
	bool m_isTrigger = false;
	bool m_generateCollisionEvents = false;
};

