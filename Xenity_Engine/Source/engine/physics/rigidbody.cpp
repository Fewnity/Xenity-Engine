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
#include <engine/inputs/input_system.h>

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
	AddVariable(reflectedVariables, mass, "mass", true);
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
	bulletRigidbody->activate();
	bulletRigidbody->setLinearVelocity(btVector3(_velocity.x, _velocity.y, _velocity.z));
	//velocity = _velocity;
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
	if (GetGameObject()->IsLocalActive() && IsEnabled())
	{
		btVector3 pos;
		PhysicsManager::GetPosition(bulletRigidbody, pos);
		GetTransform()->SetPosition(Vector3(pos.x(), pos.y(), pos.z()));

		btVector3 rot;
		btQuaternion q = bulletRigidbody->getOrientation();
		GetTransform()->SetRotation(Quaternion(q.x(), q.y(), q.z(), q.w()));
	}

	btVector3 vel = bulletRigidbody->getLinearVelocity();
	velocity = Vector3(vel.x(), vel.y(), vel.z());

	//PhysicsManager::GetEulerAngles(bulletRigidbody, rot); //ZYX
	////GetTransform()->SetRotation(Vector3(rot.x(), rot.y(), rot.z()));

	//btMatrix3x3 m = bulletRigidbody->getWorldTransform().getBasis();
	//glm::mat4x4 mgl;
	//bulletRigidbody->getWorldTransform().getBasis().getOpenGLSubMatrix((btScalar*)(&mgl));
	//
	////const glm::mat4 matChildRelative = glm::mat4_cast(mgl);

	//float x, y, z;
	//glm::extractEulerAngleXYZ(mgl, x, y, z);
	//x = glm::degrees(x);
	//y = glm::degrees(y);
	//z = glm::degrees(z);
	//GetTransform()->SetRotation(Vector3(x, y, z));



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
	if (bulletCompoundShape)
		return;


	btVector3 localInertia(0, 0, 0);

	btTransform offsetTransform;
	offsetTransform.setIdentity();
	offsetTransform.setOrigin(btVector3(0, 0, 0));
	btTransform startTransform;
	startTransform.setIdentity();
	const Vector3& pos = GetTransform()->GetPosition();
	const Quaternion& rot = GetTransform()->GetRotation();

	startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
	startTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

	// Create MotionState and RigidBody object for the box shape
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	bulletRigidbody = new btRigidBody(1, myMotionState, nullptr, localInertia);
	PhysicsManager::physDynamicsWorld->addRigidBody(bulletRigidbody);
	bulletRigidbody->activate();

	bulletCompoundShape = new btCompoundShape();
}

void RigidBody::AddShape(btCollisionShape* shape, const Vector3& offset)
{
	shapes.push_back(shape);
	//shapes.push_back(std::make_pair(shape, rb));
	//if (shapes.size() != 1)
	//{
	//	btTransform frameInA;
	//	frameInA.setIdentity();
	//	frameInA.setOrigin(btVector3(0, 0, 0));

	//	btTransform frameInB;
	//	frameInB.setIdentity();
	//	frameInB.setOrigin(btVector3(0, 0, 0));

	//	//btPoint2PointConstraint* p2p = new btPoint2PointConstraint(*bulletRigidbody, frameInA);
	//	btPoint2PointConstraint* fixedConstraintAB = new btPoint2PointConstraint(*shapes[0].second, *rb, btVector3(0, 0, 0), btVector3(0, 0, 0));
	//	PhysicsManager::physDynamicsWorld->addConstraint(fixedConstraintAB, true);
	//}

	PhysicsManager::physDynamicsWorld->removeRigidBody(bulletRigidbody);
	{
		float mass = 1.0f;
		btVector3 inertia(0, 0, 0);

		btTransform offsetTransform;
		offsetTransform.setIdentity();
		offsetTransform.setOrigin(btVector3(offset.x, offset.y, offset.z));

		bulletCompoundShape->addChildShape(offsetTransform, shape);
		bulletCompoundShape->calculateLocalInertia(mass, inertia);

		bulletRigidbody->setCollisionShape(bulletCompoundShape);
		bulletRigidbody->setMassProps(mass, inertia);
	}
	PhysicsManager::physDynamicsWorld->addRigidBody(bulletRigidbody);
}