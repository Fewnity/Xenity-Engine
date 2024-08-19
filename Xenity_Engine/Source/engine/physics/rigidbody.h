// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <vector>

#include <engine/api.h>
#include <engine/component.h>
#include <engine/vectors/vector3.h>

class BoxCollider;

class btRigidBody;
class btCollisionShape;
class btCompoundShape;

class LockedAxis : public Reflective
{
public:
	ReflectiveData GetReflectiveData() override;

	bool x = false;
	bool y = false;
	bool z = false;
};

/*
* @brief Component to add physics to the GameObject
*/
class API RigidBody : public Component
{
public:
	RigidBody();
	~RigidBody();

	void Awake() override;

	inline const Vector3& GetVelocity() const { return velocity; }
	void SetVelocity(const Vector3& _velocity);

	inline float GetDrag() const { return drag; }
	void SetDrag(float _drag);

	inline float GetAngularDrag() const { return angularDrag; }
	void SetAngularDrag(float _angularDrag);

	inline float GetBounce() const { return bounce; }
	void SetBounce(float _bounce);

	inline float GetGravityMultiplier() const  { return gravityMultiplier; }
	void SetGravityMultiplier(float _gravityMultiplier);

	inline float IsStatic() const  { return isStatic; }
	void SetIsStatic(float _isStatic);

	inline float GetMass() const { return mass; }
	void SetMass(float _mass);

	inline float GetFriction() const { return friction; }
	void SetFriction(float _friction);

	LockedAxis lockedMovementAxis;
	LockedAxis lockedRotationAxis;

	void Activate();

protected:
	void RemoveReferences()  override;

	bool disableEvent = false;
	std::vector<Collider*> colliders;
	bool generatesEvents = false;
	void UpdateGeneratesEvents();

	void UpdateRigidBodyMass();
	void UpdateRigidBodyDrag();
	void UpdateRigidBodyBounce();
	void UpdateRigidBodyGravityMultiplier();
	void UpdateRigidBodyFriction();

	void UpdateLockedAxis();

	void OnEnabled() override;
	void OnDisabled() override;
	void OnTransformUpdated();

	friend class Collider;
	friend class BoxCollider;
	friend class SphereCollider;
	friend class PhysicsManager;
	friend class MyContactResultCallback;

	void AddShape(btCollisionShape* shape, const Vector3& offset);
	void AddTriggerShape(btCollisionShape* shape, const Vector3& offset);
	void RemoveShape(btCollisionShape* shape);
	void RemoveTriggerShape(btCollisionShape* shape);

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;


	Vector3 velocity = Vector3(0, 0, 0);
	float drag = 0.1f;
	float angularDrag = 0.1f;
	float bounce = 0.0f;
	float mass = 1;
	float gravityMultiplier = 1.0f;
	float friction = 0.1f;
	bool isStatic = false;

	btRigidBody* bulletRigidbody = nullptr;
	btCompoundShape* bulletCompoundShape = nullptr;
	std::vector<btCollisionShape*> shapes;

	btRigidBody* bulletTriggerRigidbody = nullptr;
	btCompoundShape* bulletTriggerCompoundShape = nullptr;
	std::vector<btCollisionShape*> triggerShapes;

	/**
	 * @brief [Internal]
	 */
	void Tick();
};
