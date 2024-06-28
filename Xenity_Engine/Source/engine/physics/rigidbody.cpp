// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "rigidbody.h"

#include "physics_manager.h"
#include "box_collider.h"

#include <engine/asset_management/asset_manager.h>
#include <engine/time/time.h>

#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>

RigidBody::RigidBody()
{
	componentName = "RigidBody";
	AssetManager::AddReflection(this);
}

ReflectiveData RigidBody::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	AddVariable(reflectedVariables, isStatic, "isStatic", true);
	AddVariable(reflectedVariables, gravityMultiplier, "gravityMultiplier", true);
	AddVariable(reflectedVariables, drag, "drag", true);
	AddVariable(reflectedVariables, bounce, "bounce", true);
	return reflectedVariables;
}

void RigidBody::OnReflectionUpdated()
{
	// Call setters to make sure the values are correct
	SetDrag(drag);
	SetBounce(bounce);
}

void RigidBody::SetVelocity(Vector3 _velocity)
{
	velocity = _velocity;
}

void RigidBody::SetDrag(float _drag)
{
	if(_drag < 0)
	{
		drag = 0;
	}
	else
	{
		drag = _drag;
	}
}

void RigidBody::SetBounce(float _bounce)
{
	if (_bounce < 0)
	{
		bounce = 0;
	}
	else
	{
		bounce = _bounce;
	}
}

void RigidBody::SetGravityMultiplier(float _gravityMultiplier)
{
	gravityMultiplier = _gravityMultiplier;
}

void RigidBody::SetIsStatic(float _isStatic)
{
	isStatic = _isStatic;
}

void RigidBody::Tick()
{
	if (GetGameObject()->GetLocalActive() && GetIsEnabled())
	{
		int side = (int)CollisionSide::NoSide;
		std::shared_ptr<BoxCollider> rbBoxCollider = GetGameObject()->GetComponent<BoxCollider>();
		if (rbBoxCollider)
		{
			std::shared_ptr<BoxCollider> other = nullptr;
			const Vector3 deltaTimeVelocity = velocity * Time::GetDeltaTime();

			const size_t colliderCount = PhysicsManager::boxColliders.size();
			for (int i = 0; i < colliderCount; i++)
			{
				other = PhysicsManager::boxColliders[i].lock();
				if (other != rbBoxCollider && other->GetIsEnabled() && other->GetGameObject()->GetLocalActive())
				{
					const std::shared_ptr<RigidBody> otherRigidbody = other->GetAttachedRigidbody().lock();
					if (rbBoxCollider->isTrigger && isStatic && otherRigidbody && !otherRigidbody->isStatic) // Check trigger
					{
						const bool trigger = BoxCollider::CheckTrigger(*rbBoxCollider, *other);
						if (trigger)
						{
							rbBoxCollider->onTriggerEvent.Trigger(other);
						}
					}
					else if (!other->isTrigger && !isStatic) // Check collision
					{
						const int tempSide = (int)BoxCollider::CheckCollision(*rbBoxCollider, *other, deltaTimeVelocity);
						if (tempSide != (int)CollisionSide::NoSide)
						{
							if ((side & tempSide) == 0)
								side |= tempSide;
						}
					}

				}
			}
		}

		if (!isStatic)
		{
			Vector3 newVelocity = velocity;

			// Make the rigidbody bounce if there is a wall in the opposite direction
			if ((side & (int)CollisionSide::SideX) != 0)
				newVelocity.x = -velocity.x * bounce;
			if ((side & (int)CollisionSide::SideY) != 0)
				newVelocity.y = -velocity.y * bounce;
			if ((side & (int)CollisionSide::SideZ) != 0)
				newVelocity.z = -velocity.z * bounce;

			// Move the rigidbody
			if (newVelocity.Magnitude() != 0)
				GetTransform()->SetPosition(GetTransform()->GetPosition() + newVelocity * Time::GetDeltaTime());

			// Apply gravity
			if ((side & (int)CollisionSide::SideY) == 0)
			{
				newVelocity.y -= 9.81f * gravityMultiplier * Time::GetDeltaTime();
				if (newVelocity.y <= -10)
				{
					newVelocity.y = -10;
				}
			}
			velocity = newVelocity;
		}
	}
}

RigidBody::~RigidBody()
{
	AssetManager::RemoveReflection(this);
}