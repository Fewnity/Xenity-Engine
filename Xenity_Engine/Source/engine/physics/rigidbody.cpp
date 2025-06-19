// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "rigidbody.h"

#include <bullet/btBulletDynamicsCommon.h>
#include <glm/gtx/euler_angles.hpp>

#include <engine/physics/collider.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/debug/stack_debug_object.h>

#include "physics_manager.h"
#include <glm/gtx/quaternion.hpp>

RigidBody::RigidBody() : Component(false, false)
{
	AssetManager::AddReflection(this);
}

ReflectiveData RigidBody::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	AddVariable(reflectedVariables, m_isStatic, "isStatic", true);
	AddVariable(reflectedVariables, m_gravityMultiplier, "gravityMultiplier", true);
	AddVariable(reflectedVariables, m_drag, "drag", true);
	AddVariable(reflectedVariables, m_angularDrag, "angularDrag", true);
	AddVariable(reflectedVariables, m_bounce, "bounce", true);
	AddVariable(reflectedVariables, m_mass, "mass", true);
	AddVariable(reflectedVariables, m_friction, "friction", true);
	AddVariable(reflectedVariables, lockedMovementAxis, "lockedMovementAxis", true);
	AddVariable(reflectedVariables, lockedRotationAxis, "lockedRotationAxis", true);
	return reflectedVariables;
}

void RigidBody::OnReflectionUpdated()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	// Call setters to make sure the values are correct and to apply them to the bullet rigidbody
	SetDrag(m_drag);
	SetBounce(m_bounce);
	SetMass(m_mass);
	SetGravityMultiplier(m_gravityMultiplier);
	SetFriction(m_friction);
	UpdateLockedAxis();
}

void RigidBody::SetVelocity(const Vector3& _velocity)
{
	if (!m_bulletRigidbody)
		return;

	m_bulletRigidbody->activate();
	m_bulletRigidbody->setLinearVelocity(btVector3(_velocity.x, _velocity.y, _velocity.z));
	m_velocity = _velocity;
}

void RigidBody::ApplyTorque(const Vector3& torque)
{
	if (!m_bulletRigidbody)
		return;

	m_bulletRigidbody->activate();
	m_bulletRigidbody->applyTorque(btVector3(torque.x, torque.y, torque.z));
}

Vector3 RigidBody::GetTorque() const
{
	if (!m_bulletRigidbody)
		return Vector3(0);

	btVector3 torque = m_bulletRigidbody->getTotalTorque();
	return Vector3(torque.x(), torque.y(), torque.z());
}

void RigidBody::AddAngularVelocity(const Vector3& velocity)
{
	if (!m_bulletRigidbody)
		return;

	m_bulletRigidbody->activate();
	btVector3 angVel = m_bulletRigidbody->getAngularVelocity();
	m_bulletRigidbody->setAngularVelocity(angVel + btVector3(velocity.x, velocity.y, velocity.z));
}

void RigidBody::SetAngularVelocity(const Vector3& torque)
{
	if (!m_bulletRigidbody)
		return;

	m_bulletRigidbody->activate();
	m_bulletRigidbody->setAngularVelocity(btVector3(torque.x, torque.y, torque.z));
}

Vector3 RigidBody::GetAngularVelocity() const
{
	if (!m_bulletRigidbody)
		return Vector3(0);

	btVector3 angularVel = m_bulletRigidbody->getAngularVelocity();
	return Vector3(angularVel.x(), angularVel.y(), angularVel.z());
}

void RigidBody::SetDrag(float _drag)
{
	if (_drag < 0)
	{
		m_drag = 0;
	}
	else
	{
		m_drag = _drag;
	}
	UpdateRigidBodyDrag();
}

void RigidBody::SetAngularDrag(float _angularDrag)
{
	if (_angularDrag < 0)
	{
		m_angularDrag = 0;
	}
	else
	{
		m_angularDrag = _angularDrag;
	}
	UpdateRigidBodyDrag();
}

void RigidBody::SetBounce(float _bounce)
{
	if (_bounce < 0)
	{
		m_bounce = 0;
	}
	else
	{
		m_bounce = _bounce;
	}
	UpdateRigidBodyBounce();
}

void RigidBody::SetGravityMultiplier(float _gravityMultiplier)
{
	m_gravityMultiplier = _gravityMultiplier;
	UpdateRigidBodyGravityMultiplier();
}

void RigidBody::SetIsStatic(float _isStatic)
{
	m_isStatic = _isStatic;
}

void RigidBody::SetMass(float _mass)
{
	if (_mass < 0)
	{
		m_mass = 0;
	}
	else
	{
		m_mass = _mass;
	}
	UpdateRigidBodyMass();
}

void RigidBody::SetFriction(float _friction)
{
	m_friction = _friction;
	if (m_friction < 0)
	{
		m_friction = 0;
	}
	UpdateRigidBodyFriction();
}

void RigidBody::Activate()
{
	if (m_bulletRigidbody)
	{
		m_bulletRigidbody->activate();
	}
}

void RigidBody::SetLockedMovementAxis(LockedAxis axis)
{
	lockedMovementAxis = axis;
	UpdateLockedAxis();
}

void RigidBody::SetLockedRotationAxis(LockedAxis axis)
{
	lockedRotationAxis = axis;
	UpdateLockedAxis();
}

void RigidBody::RemoveReferences()
{
	PhysicsManager::RemoveRigidBody(this);
}

void RigidBody::UpdateGeneratesEvents()
{
	bool hasEvents = false;
	for (auto& collider : m_colliders)
	{
		if (collider->m_generateCollisionEvents)
		{
			hasEvents = true;
			break;
		}
	}
	m_generatesEvents = hasEvents;
}

void RigidBody::UpdateRigidBodyMass()
{
	if (!m_bulletRigidbody)
	{
		return;
	}

	btVector3 inertia(0, 0, 0);
	m_bulletCompoundShape->calculateLocalInertia(m_mass, inertia);
	if (m_isStatic) 
	{
		m_bulletRigidbody->setCollisionFlags(m_bulletRigidbody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	}
	else 
	{
		m_bulletRigidbody->setCollisionFlags(0);
	}
	m_bulletRigidbody->setMassProps(m_mass, inertia);

	if (!m_isStatic)
	{
		m_bulletRigidbody->activate();
	}
}

void RigidBody::UpdateRigidBodyDrag()
{
	if (!m_bulletRigidbody)
		return;

	m_bulletRigidbody->setDamping(m_drag, m_angularDrag);
}

void RigidBody::UpdateRigidBodyBounce()
{
	if (!m_bulletRigidbody)
		return;

	m_bulletRigidbody->setRestitution(m_bounce);
}

void RigidBody::UpdateRigidBodyGravityMultiplier()
{
	if (!m_bulletRigidbody)
		return;

	// Apply gravity multiplier
	const Vector3 newGravity = PhysicsManager::s_gravity * m_gravityMultiplier;
	const btVector3 btgravity = btVector3(newGravity.x, newGravity.y, newGravity.z);
	m_bulletRigidbody->setGravity(btgravity);
}

void RigidBody::UpdateRigidBodyFriction()
{
	if (!m_bulletRigidbody)
		return;

	m_bulletRigidbody->setFriction(m_friction);
}

void RigidBody::UpdateLockedAxis()
{
	if (!m_bulletRigidbody)
		return;

	btVector3 lockRotAxis = btVector3(1, 1, 1);
	if (lockedRotationAxis.x)
		lockRotAxis.setX(0);

	if (lockedRotationAxis.y)
		lockRotAxis.setY(0);

	if (lockedRotationAxis.z)
		lockRotAxis.setZ(0);

	m_bulletRigidbody->setAngularFactor(lockRotAxis);

	btVector3 lockMovAxis = btVector3(1, 1, 1);
	if (lockedMovementAxis.x)
		lockMovAxis.setX(0);

	if (lockedMovementAxis.y)
		lockMovAxis.setY(0);

	if (lockedMovementAxis.z)
		lockMovAxis.setZ(0);

	m_bulletRigidbody->setLinearFactor(lockMovAxis);
}

void RigidBody::OnEnabled()
{
	if(m_bulletRigidbody)
	{
		PhysicsManager::s_physDynamicsWorld->addRigidBody(m_bulletRigidbody);
		PhysicsManager::s_physDynamicsWorld->addRigidBody(m_bulletTriggerRigidbody);
	}
}

void RigidBody::OnDisabled()
{
	if(m_bulletRigidbody)
	{
		PhysicsManager::s_physDynamicsWorld->removeRigidBody(m_bulletRigidbody);
		PhysicsManager::s_physDynamicsWorld->removeRigidBody(m_bulletTriggerRigidbody);
	}
}

void RigidBody::OnTransformUpdated()
{
	if (m_disableEvent)
		return;

	const Transform& transform = *GetTransformRaw();
	m_bulletRigidbody->setWorldTransform(btTransform(
		btQuaternion(transform.GetRotation().x, transform.GetRotation().y, transform.GetRotation().z, transform.GetRotation().w),
		btVector3(transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z)));

	m_bulletTriggerRigidbody->setWorldTransform(m_bulletRigidbody->getWorldTransform());

	m_bulletRigidbody->activate();
}

void RigidBody::Tick()
{
	if (GetGameObjectRaw()->IsLocalActive() && m_bulletTriggerRigidbody)
	{
		m_disableEvent = true;
		m_bulletTriggerRigidbody->setWorldTransform(m_bulletRigidbody->getWorldTransform());
		
		// Use the same matrix but with the x inverted
		glm::mat4 mat;
		m_bulletRigidbody->getCenterOfMassTransform().getOpenGLMatrix((float*)&mat);

		// Fix position
		mat[3].x = -mat[3].x;

		// Fix rotation
		mat[0].y = -mat[0].y;
		mat[0].z = -mat[0].z;
		mat[1].x = -mat[1].x;
		mat[2].x = -mat[2].x;

		Transform& transform = *GetTransformRaw();
		transform.SetTransformationMatrix(mat);

		const btVector3& vel = m_bulletRigidbody->getLinearVelocity();
		m_velocity = Vector3(vel.x(), vel.y(), vel.z());
		m_disableEvent = false;
	}
}

RigidBody::~RigidBody()
{
	if (GetTransformRaw())
	{
		GetTransformRaw()->GetOnTransformUpdated().Unbind(&RigidBody::OnTransformUpdated, this);
	}

	AssetManager::RemoveReflection(this);
	for (Collider* c : m_colliders)
	{
		c->SetRigidbody(nullptr);
	}

	if (m_bulletRigidbody)
	{
		PhysicsManager::s_physDynamicsWorld->removeRigidBody(m_bulletRigidbody);
		PhysicsManager::s_physDynamicsWorld->removeRigidBody(m_bulletTriggerRigidbody);

		delete m_bulletCompoundShape;
		m_bulletCompoundShape = nullptr;

		delete m_bulletTriggerCompoundShape;
		m_bulletTriggerCompoundShape = nullptr;

		delete m_bulletRigidbody->getMotionState();
		delete m_bulletRigidbody;
		m_bulletRigidbody = nullptr;

		delete m_bulletTriggerRigidbody->getMotionState();
		delete m_bulletTriggerRigidbody;
		m_bulletTriggerRigidbody = nullptr;
	}
}

void RigidBody::Awake()
{
	if (m_bulletCompoundShape)
		return;

	GetTransformRaw()->GetOnTransformUpdated().Bind(&RigidBody::OnTransformUpdated, this);

	btTransform startTransform;
	startTransform.setIdentity();
	const Vector3& pos = GetTransformRaw()->GetPosition();
	const Quaternion& rot = GetTransformRaw()->GetRotation();

	startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
	startTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

	// Create MotionState and RigidBody object
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	m_bulletRigidbody = new btRigidBody(1, myMotionState, nullptr, btVector3(0, 0, 0));
	m_bulletRigidbody->setUserPointer(this);

	btDefaultMotionState* myMotionStateTrigger = new btDefaultMotionState(startTransform);
	m_bulletTriggerRigidbody = new btRigidBody(1, myMotionStateTrigger, nullptr, btVector3(0, 0, 0));
	m_bulletTriggerRigidbody->setUserPointer(this);

	UpdateLockedAxis();
	UpdateRigidBodyDrag();
	UpdateRigidBodyBounce();
	UpdateRigidBodyFriction();

	PhysicsManager::s_physDynamicsWorld->addRigidBody(m_bulletRigidbody);

	// Set compound's enableDynamicAabbTree param to false, because it's not working on PS3 for some reasons...
	m_bulletCompoundShape = new btCompoundShape(false);
	m_bulletRigidbody->setCollisionShape(m_bulletCompoundShape);

	// Set compound's enableDynamicAabbTree param to false, because it's not working on PS3 for some reasons...
	m_bulletTriggerCompoundShape = new btCompoundShape(false);
	m_bulletTriggerRigidbody->setCollisionShape(m_bulletTriggerCompoundShape);
	m_bulletTriggerRigidbody->setCollisionFlags(m_bulletTriggerRigidbody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	m_bulletRigidbody->activate();
	m_bulletTriggerRigidbody->activate();

	// Add an empty shape to enable gravity with an empty rigidbody
	m_emptyShape = new btEmptyShape();
	AddShape(m_emptyShape, Vector3(0, 0, 0));
	m_isEmpty = true;
	m_isTriggerEmpty = true;

	const std::vector<std::shared_ptr<Collider>> col = GetGameObject()->GetComponents<Collider>();
	for(const std::shared_ptr<Collider>& c : col)
	{
		c->SetRigidbody(std::dynamic_pointer_cast<RigidBody>(shared_from_this()));
		c->CreateCollision(true);
	}

	UpdateRigidBodyGravityMultiplier();
}

void RigidBody::AddShape(btCollisionShape* shape, const Vector3& offset)
{
	PhysicsManager::s_physDynamicsWorld->removeRigidBody(m_bulletRigidbody);
	{
		btTransform offsetTransform;
		offsetTransform.setIdentity();
		offsetTransform.setOrigin(btVector3(offset.x, offset.y, offset.z));

		if (m_bulletCompoundShape->getNumChildShapes() == 1)
		{
			m_bulletCompoundShape->removeChildShape(m_emptyShape);
		}

		m_bulletCompoundShape->addChildShape(offsetTransform, shape);

		m_isEmpty = false;

		UpdateRigidBodyMass();
	}
	PhysicsManager::s_physDynamicsWorld->addRigidBody(m_bulletRigidbody);
}

void RigidBody::AddTriggerShape(btCollisionShape* shape, const Vector3& offset)
{
	PhysicsManager::s_physDynamicsWorld->removeRigidBody(m_bulletTriggerRigidbody);
	{
		btTransform offsetTransform;
		offsetTransform.setIdentity();
		offsetTransform.setOrigin(btVector3(offset.x, offset.y, offset.z));

		m_bulletTriggerCompoundShape->addChildShape(offsetTransform, shape);

		m_isTriggerEmpty = false;
	}
	PhysicsManager::s_physDynamicsWorld->addRigidBody(m_bulletTriggerRigidbody);
}

void RigidBody::RemoveShape(btCollisionShape* shape)
{
	PhysicsManager::s_physDynamicsWorld->removeRigidBody(m_bulletRigidbody);
	{
		m_bulletCompoundShape->removeChildShape(shape);

		if (m_bulletCompoundShape->getNumChildShapes() == 0)
		{
			btTransform offsetTransform;
			offsetTransform.setIdentity();
			offsetTransform.setOrigin(btVector3(0, 0, 0));

			m_bulletCompoundShape->addChildShape(offsetTransform, m_emptyShape);
			m_isEmpty = false;
		}
		else 
		{
			m_isEmpty = true;
		}

		UpdateRigidBodyMass();
	}
	PhysicsManager::s_physDynamicsWorld->addRigidBody(m_bulletRigidbody);
}

void RigidBody::RemoveTriggerShape(btCollisionShape* shape)
{
	PhysicsManager::s_physDynamicsWorld->removeRigidBody(m_bulletTriggerRigidbody);

	m_bulletTriggerCompoundShape->removeChildShape(shape);

	if (m_bulletTriggerCompoundShape->getNumChildShapes() != 0)
	{
		PhysicsManager::s_physDynamicsWorld->addRigidBody(m_bulletTriggerRigidbody);
		m_isTriggerEmpty = false;
	}
	else 
	{
		m_isTriggerEmpty = true;
	}
}

ReflectiveData LockedAxis::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	AddVariable(reflectedVariables, x, "x", true);
	AddVariable(reflectedVariables, y, "y", true);
	AddVariable(reflectedVariables, z, "z", true);
	return reflectedVariables;
}
