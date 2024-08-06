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
#include <engine/debug/debug.h>

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
	AddVariable(reflectedVariables, angularDrag, "angularDrag", true);
	AddVariable(reflectedVariables, bounce, "bounce", true);
	AddVariable(reflectedVariables, mass, "mass", true);
	AddVariable(reflectedVariables, lockedMovementAxis, "lockedMovementAxis", true);
	AddVariable(reflectedVariables, lockedRotationAxis, "lockedRotationAxis", true);
	AddVariable(reflectedVariables, generatesEvents, "generatesEvents", true);
	return reflectedVariables;
}

void RigidBody::OnReflectionUpdated()
{
	// Call setters to make sure the values are correct
	SetDrag(drag);
	SetBounce(bounce);
	SetMass(mass);
	UpdateLockedAxis();
}

void RigidBody::SetVelocity(const Vector3& _velocity)
{
	if (!bulletRigidbody)
		return;

	bulletRigidbody->activate();
	bulletRigidbody->setLinearVelocity(btVector3(_velocity.x, _velocity.y, _velocity.z));
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
	UpdateRigidBodyDrag();
}

void RigidBody::SetAngularDrag(float _angularDrag)
{
	if (_angularDrag < 0)
	{
		angularDrag = 0;
	}
	else
	{
		angularDrag = _angularDrag;
	}
	UpdateRigidBodyDrag();
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
	UpdateRigidBodyBounce();
}

void RigidBody::SetGravityMultiplier(float _gravityMultiplier)
{
	gravityMultiplier = _gravityMultiplier;
}

void RigidBody::SetIsStatic(float _isStatic)
{
	isStatic = _isStatic;
}

void RigidBody::SetMass(float _mass)
{
	if (_mass < 0)
	{
		mass = 0;
	}
	else
	{
		mass = _mass;
	}
	UpdateRigidBodyMass();
}

void RigidBody::Activate()
{
	if (bulletRigidbody)
	{
		bulletRigidbody->activate();
	}
}

void RigidBody::UpdateGeneratesEvents()
{
	bool hasEvents = false;
	for (auto& collider : colliders)
	{
		if (collider->generateCollisionEvents)
		{
			hasEvents = true;
			break;
		}
	}
	generatesEvents = hasEvents;
}

void RigidBody::UpdateRigidBodyMass()
{
	if (!bulletRigidbody)
		return;

	float tempMass = mass;
	if (isStatic)
	{
		tempMass = 0;
	}
	btVector3 inertia(0, 0, 0);
	bulletCompoundShape->calculateLocalInertia(tempMass, inertia);

	bulletRigidbody->setMassProps(tempMass, inertia);

	if (tempMass != 0)
		bulletRigidbody->activate();
}

void RigidBody::UpdateRigidBodyDrag()
{
	if (!bulletRigidbody)
		return;

	bulletRigidbody->setDamping(drag, angularDrag);
}

void RigidBody::UpdateRigidBodyBounce()
{
	if (!bulletRigidbody)
		return;

	bulletRigidbody->setRestitution(bounce);
}

void RigidBody::UpdateLockedAxis()
{
	if (!bulletRigidbody)
		return;

	btVector3 lockRotAxis = btVector3(1, 1, 1);
	if (lockedRotationAxis.x)
		lockRotAxis.setX(0);

	if (lockedRotationAxis.y)
		lockRotAxis.setY(0);

	if (lockedRotationAxis.z)
		lockRotAxis.setZ(0);

	bulletRigidbody->setAngularFactor(lockRotAxis);

	btVector3 lockMovAxis = btVector3(1, 1, 1);
	if (lockedMovementAxis.x)
		lockMovAxis.setX(0);

	if (lockedMovementAxis.y)
		lockMovAxis.setY(0);

	if (lockedMovementAxis.z)
		lockMovAxis.setZ(0);

	bulletRigidbody->setLinearFactor(lockMovAxis);
}

void RigidBody::OnEnabled()
{
	if(bulletRigidbody)
	{
		PhysicsManager::physDynamicsWorld->addRigidBody(bulletRigidbody);
		PhysicsManager::physDynamicsWorld->addRigidBody(bulletTriggerRigidbody);
	}
}

void RigidBody::OnDisabled()
{
	if(bulletRigidbody)
	{
		PhysicsManager::physDynamicsWorld->removeRigidBody(bulletRigidbody);
		PhysicsManager::physDynamicsWorld->removeRigidBody(bulletTriggerRigidbody);
	}
}

void RigidBody::OnTransformUpdated()
{
	if (disableEvent)
		return;

	const Transform& transform = *GetTransform();
	bulletRigidbody->setWorldTransform(btTransform(
		btQuaternion(transform.GetRotation().x, transform.GetRotation().y, transform.GetRotation().z, transform.GetRotation().w),
		btVector3(transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z)));

	bulletTriggerRigidbody->setWorldTransform(bulletRigidbody->getWorldTransform());

	bulletRigidbody->activate();
}

void RigidBody::Tick()
{
	if (GetGameObject()->IsLocalActive() && bulletTriggerRigidbody)
	{
		Transform& transform = *GetTransform();
		disableEvent = true;
		bulletTriggerRigidbody->setWorldTransform(bulletRigidbody->getWorldTransform());

		const btVector3& p = bulletRigidbody->getCenterOfMassPosition();
		transform.SetPosition(Vector3(p.x(), p.y(), p.z()));

		const btQuaternion q = bulletRigidbody->getOrientation();
		transform.SetRotation(Quaternion(q.x(), q.y(), q.z(), q.w()));

		const btVector3& vel = bulletRigidbody->getLinearVelocity();
		velocity = Vector3(vel.x(), vel.y(), vel.z());
		disableEvent = false;
	}
}

RigidBody::~RigidBody()
{
	GetTransform()->GetOnTransformUpdated().Unbind(&RigidBody::OnTransformUpdated, this);

	AssetManager::RemoveReflection(this);
	for (Collider* c : colliders)
	{
		c->SetRigidbody(nullptr);
	}

	if (bulletRigidbody)
	{
		PhysicsManager::physDynamicsWorld->removeRigidBody(bulletRigidbody);
		PhysicsManager::physDynamicsWorld->removeRigidBody(bulletTriggerRigidbody);

		delete bulletCompoundShape;
		bulletCompoundShape = nullptr;

		delete bulletTriggerCompoundShape;
		bulletTriggerCompoundShape = nullptr;

		delete bulletRigidbody->getMotionState();
		delete bulletRigidbody;
		bulletRigidbody = nullptr;

		delete bulletTriggerRigidbody->getMotionState();
		delete bulletTriggerRigidbody;
		bulletTriggerRigidbody = nullptr;
	}
}

void RigidBody::Awake()
{
	if (bulletCompoundShape)
		return;

	GetTransform()->GetOnTransformUpdated().Bind(&RigidBody::OnTransformUpdated, this);

	btTransform startTransform;
	startTransform.setIdentity();
	const Vector3& pos = GetTransform()->GetPosition();
	const Quaternion& rot = GetTransform()->GetRotation();

	startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
	startTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

	// Create MotionState and RigidBody object
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	bulletRigidbody = new btRigidBody(1, myMotionState, nullptr, btVector3(0, 0, 0));
	bulletRigidbody->setUserPointer(this);

	btDefaultMotionState* myMotionStateTrigger = new btDefaultMotionState(startTransform);
	bulletTriggerRigidbody = new btRigidBody(1, myMotionStateTrigger, nullptr, btVector3(0, 0, 0));
	bulletTriggerRigidbody->setUserPointer(this);

	UpdateLockedAxis();
	UpdateRigidBodyDrag();
	UpdateRigidBodyBounce();

	PhysicsManager::physDynamicsWorld->addRigidBody(bulletRigidbody);
	PhysicsManager::physDynamicsWorld->addRigidBody(bulletTriggerRigidbody);
	bulletCompoundShape = new btCompoundShape();
	bulletRigidbody->setCollisionShape(bulletCompoundShape);

	bulletTriggerCompoundShape = new btCompoundShape();
	bulletTriggerRigidbody->setCollisionShape(bulletTriggerCompoundShape);
	bulletTriggerRigidbody->setCollisionFlags(bulletTriggerRigidbody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	bulletRigidbody->activate();
	bulletTriggerRigidbody->activate();

	// Add an empty shape to enable gravity with an empty rigidbody
	btEmptyShape* emptyShape = new btEmptyShape();
	AddShape(emptyShape, Vector3(0, 0, 0));

	std::vector<std::shared_ptr<Collider>> col = GetGameObject()->GetComponents<Collider>();
	for(std::shared_ptr<Collider>& c : col)
	{
		c->SetRigidbody(std::dynamic_pointer_cast<RigidBody>(shared_from_this()));
		c->CreateCollision(true);
	}
}

void RigidBody::AddShape(btCollisionShape* shape, const Vector3& offset)
{
	shapes.push_back(shape);

	PhysicsManager::physDynamicsWorld->removeRigidBody(bulletRigidbody);
	{
		btTransform offsetTransform;
		offsetTransform.setIdentity();
		offsetTransform.setOrigin(btVector3(offset.x, offset.y, offset.z));

		bulletCompoundShape->addChildShape(offsetTransform, shape);

		UpdateRigidBodyMass();
	}
	PhysicsManager::physDynamicsWorld->addRigidBody(bulletRigidbody);
}

void RigidBody::AddTriggerShape(btCollisionShape* shape, const Vector3& offset)
{
	triggerShapes.push_back(shape);

	PhysicsManager::physDynamicsWorld->removeRigidBody(bulletTriggerRigidbody);
	{
		btTransform offsetTransform;
		offsetTransform.setIdentity();
		offsetTransform.setOrigin(btVector3(offset.x, offset.y, offset.z));

		bulletTriggerCompoundShape->addChildShape(offsetTransform, shape);
	}
	PhysicsManager::physDynamicsWorld->addRigidBody(bulletTriggerRigidbody);
}

void RigidBody::RemoveShape(btCollisionShape* shape)
{
	PhysicsManager::physDynamicsWorld->removeRigidBody(bulletRigidbody);
	{
		bulletCompoundShape->removeChildShape(shape);

		UpdateRigidBodyMass();
	}
	PhysicsManager::physDynamicsWorld->addRigidBody(bulletRigidbody);
}

void RigidBody::RemoveTriggerShape(btCollisionShape* shape)
{
	PhysicsManager::physDynamicsWorld->removeRigidBody(bulletTriggerRigidbody);
	{
		bulletTriggerCompoundShape->removeChildShape(shape);
	}
	PhysicsManager::physDynamicsWorld->addRigidBody(bulletTriggerRigidbody);
}

ReflectiveData LockedAxis::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	AddVariable(reflectedVariables, x, "x", true);
	AddVariable(reflectedVariables, y, "y", true);
	AddVariable(reflectedVariables, z, "z", true);
	return reflectedVariables;
}
