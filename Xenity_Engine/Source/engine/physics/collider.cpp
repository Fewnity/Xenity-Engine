// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "collider.h"

#include <bullet/btBulletDynamicsCommon.h>

#include <engine/physics/rigidbody.h>
#include <engine/game_elements/gameobject.h>
#include "physics_manager.h"

Collider::~Collider()
{
	if (std::shared_ptr<RigidBody> rb = attachedRigidbody.lock())
	{
		//Remove from the rigidbody
		const size_t colliderCount = rb->colliders.size();
		for (size_t i = 0; i < colliderCount; i++)
		{
			if (rb->colliders[i] == this)
			{
				rb->colliders.erase(rb->colliders.begin() + i);
				rb->UpdateGeneratesEvents();
				break;
			}
		}
	}

	if (bulletCollisionObject)
	{
		PhysicsManager::physDynamicsWorld->removeCollisionObject(bulletCollisionObject);

		delete bulletCollisionObject;
		bulletCollisionObject = nullptr;
	}

	if (bulletCollisionShape)
	{
		if (attachedRigidbody.lock())
		{
			if (!isTrigger)
				attachedRigidbody.lock()->RemoveShape(bulletCollisionShape);
			else
				attachedRigidbody.lock()->RemoveTriggerShape(bulletCollisionShape);
		}

		delete bulletCollisionShape;
		bulletCollisionShape = nullptr;
	}

}

void Collider::FindRigidbody()
{
	bool isAttached = attachedRigidbody.lock() != nullptr;
	attachedRigidbody = GetGameObject()->GetComponent<RigidBody>();
	if (std::shared_ptr<RigidBody> rb = attachedRigidbody.lock())
	{
		if (!isAttached)
		{
			rb->colliders.push_back(this);
			rb->UpdateGeneratesEvents();
		}
	}
}

void Collider::SetRigidbody(const std::shared_ptr<RigidBody>& rb)
{
	bool isAttached = attachedRigidbody.lock() != nullptr;
	attachedRigidbody = rb;
	if (std::shared_ptr<RigidBody> rb = attachedRigidbody.lock())
	{
		if (!isAttached)
		{
			rb->colliders.push_back(this);
			rb->UpdateGeneratesEvents();
		}
	}
	else 
	{
		CreateCollision(true);
	}
}

void Collider::OnEnabled()
{
	if (bulletCollisionObject)
		PhysicsManager::physDynamicsWorld->addCollisionObject(bulletCollisionObject);
}

void Collider::OnDisabled()
{
	if(bulletCollisionObject)
		PhysicsManager::physDynamicsWorld->removeCollisionObject(bulletCollisionObject);
}

void Collider::RemoveReferences()
{
	PhysicsManager::RemoveCollider(this);
}
