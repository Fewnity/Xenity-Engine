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
#include <bullet/btBulletDynamicsCommon.h>
#include "physics_manager.h"
#include <engine/tools/math.h>
#include <glm/gtx/euler_angles.hpp>

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

void RigidBody::SetVelocity(const Vector3& _velocity)
{
	velocity = _velocity;
}

void RigidBody::SetDrag(float _drag)
{
	if (_drag < 0)
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
	btVector3 pos;
	PhysicsManager::GetPosition(bulletRigidbody, pos);
	GetTransform() ->SetPosition(Vector3(pos.x(), pos.y(), pos.z()));

	btVector3 rot;
	PhysicsManager::GetRotation(bulletRigidbody, rot); //ZYX
	//GetTransform()->SetRotation(Vector3(rot.x(), rot.y(), rot.z()));

	btMatrix3x3 m = bulletRigidbody->getWorldTransform().getBasis();
	glm::mat4x4 mgl;
	bulletRigidbody->getWorldTransform().getBasis().getOpenGLSubMatrix((btScalar*)(&mgl));
	
	//const glm::mat4 matChildRelative = glm::mat4_cast(mgl);

	float x, y, z;
	glm::extractEulerAngleXYZ(mgl, x, y, z);
	x = glm::degrees(x);
	y = glm::degrees(y);
	z = glm::degrees(z);
	GetTransform()->SetRotation(Vector3(x, y, z));
	//GetTransform()->transformationMatrix = mgl;
	//GetTransform()->SetRotation(Vector3(rot.x(), rot.y(), rot.z()));
	/*GetTransform()->SetRotation(Vector3(rot.x() / Math::PI * 180.f, rot.y() / Math::PI * 180.f, rot.z() / Math::PI * 180.f));*/

	/*GetTransform()->SetRotation(Vector3(rot.y() / Math::PI * 180.f, rot.x() / Math::PI * 180.f, rot.z() / Math::PI * 180.f));*/

	//GetTransform()->SetRotation(Vector3(0, 0, rot.z() / Math::PI * 180.f));
	//GetTransform()->SetRotation(Vector3(rot.x() / Math::PI * 180.f, 0, 0));
	//if (GetGameObject()->IsLocalActive() && IsEnabled())
	//{
	//	int side = (int)CollisionSide::NoSide;
	//	std::shared_ptr<BoxCollider> rbBoxCollider = GetGameObject()->GetComponent<BoxCollider>();
	//	if (rbBoxCollider)
	//	{
	//		std::shared_ptr<BoxCollider> other = nullptr;
	//		const Vector3 deltaTimeVelocity = velocity * Time::GetDeltaTime();

	//		const size_t colliderCount = PhysicsManager::boxColliders.size();
	//		for (int i = 0; i < colliderCount; i++)
	//		{
	//			other = PhysicsManager::boxColliders[i].lock();
	//			if (other != rbBoxCollider && other->IsEnabled() && other->GetGameObject()->IsLocalActive())
	//			{
	//				const std::shared_ptr<RigidBody> otherRigidbody = other->GetAttachedRigidbody().lock();
	//				if (rbBoxCollider->isTrigger && isStatic && otherRigidbody && !otherRigidbody->isStatic) // Check trigger
	//				{
	//					const bool trigger = BoxCollider::CheckTrigger(*rbBoxCollider, *other);
	//					if (trigger)
	//					{
	//						rbBoxCollider->onTriggerEvent.Trigger(other);
	//					}
	//				}
	//				else if (!other->isTrigger && !isStatic) // Check collision
	//				{
	//					const int tempSide = (int)BoxCollider::CheckCollision(*rbBoxCollider, *other, deltaTimeVelocity);
	//					if (tempSide != (int)CollisionSide::NoSide)
	//					{
	//						if ((side & tempSide) == 0)
	//							side |= tempSide;
	//					}
	//				}

	//			}
	//		}
	//	}

	//	if (!isStatic)
	//	{
	//		Vector3 newVelocity = velocity;

	//		// Make the rigidbody bounce if there is a wall in the opposite direction
	//		if ((side & (int)CollisionSide::SideX) != 0)
	//			newVelocity.x = -velocity.x * bounce;
	//		if ((side & (int)CollisionSide::SideY) != 0)
	//			newVelocity.y = -velocity.y * bounce;
	//		if ((side & (int)CollisionSide::SideZ) != 0)
	//			newVelocity.z = -velocity.z * bounce;

	//		// Move the rigidbody
	//		if (newVelocity.Magnitude() != 0)
	//			GetTransform()->SetPosition(GetTransform()->GetPosition() + newVelocity * Time::GetDeltaTime());

	//		// Apply gravity
	//		if ((side & (int)CollisionSide::SideY) == 0)
	//		{
	//			newVelocity.y -= 9.81f * gravityMultiplier * Time::GetDeltaTime();
	//			if (newVelocity.y <= -10)
	//			{
	//				newVelocity.y = -10;
	//			}
	//		}
	//		velocity = newVelocity;
	//	}
	//}
}

RigidBody::~RigidBody()
{
	AssetManager::RemoveReflection(this);
}

void RigidBody::Awake()
{
	{
		btCollisionShape* physBoxShape = new btBoxShape(btVector3(1, 1, 1));

		float mass = 1.0f;
		btVector3 localInertia(0, 0, 0);

		btTransform offsetTransform;

		//btCompoundShape* compoundShape = new btCompoundShape();
		offsetTransform.setIdentity();
		//offsetTransform.setOrigin(btVector3(-10, 1, 0)); // Par exemple, un offset de (1, 0, 0)
		offsetTransform.setOrigin(btVector3(0, 0, 0)); // Par exemple, un offset de (1, 0, 0)
		/*compoundShape->addChildShape(offsetTransform, physBoxShape);
		compoundShape->calculateLocalInertia(mass, localInertia);*/

		/*bulletRigidbody->setCollisionShape(compoundShape);
		bulletRigidbody->activate();*/
		physBoxShape->calculateLocalInertia(mass, localInertia);
		btTransform startTransform;
		startTransform.setIdentity();
		Vector3 pos = GetTransform()->GetPosition();
		startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
		startTransform.setRotation(btQuaternion(btVector3(-0.2389781, 0.0578619, 0.9692995), 4.4765805));
		//startTransform.setRotation(btQuaternion(btVector3(0.3, 1, 0.2), 0.8141592653));

		// Create MotionState and RigidBody object for the box shape
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

		bulletRigidbody = new btRigidBody(1, myMotionState, physBoxShape, localInertia);
		//bulletRigidbody->applyTorque(btVector3(100, 0, 5000));
		PhysicsManager::physDynamicsWorld->addRigidBody(bulletRigidbody);
		bulletRigidbody->activate();
	}

	//{
	//}
}
