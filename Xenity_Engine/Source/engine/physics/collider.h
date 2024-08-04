// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
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
	bool isTrigger = false;
	bool generateCollisionEvents = false;
	~Collider();

protected:
	friend class PhysicsManager;
	friend class RigidBody;

	const std::weak_ptr<RigidBody>& GetAttachedRigidbody()
	{
		return attachedRigidbody;
	}

	void FindRigidbody();
	void SetRigidbody(const std::shared_ptr<RigidBody>& rb);
	void OnEnabled() override;
	void OnDisabled() override;

	virtual void CreateCollision(bool forceCreation) = 0;

	std::weak_ptr<RigidBody> attachedRigidbody;
	btCollisionObject* bulletCollisionObject = nullptr;
	btCollisionShape* bulletCollisionShape = nullptr;
};

