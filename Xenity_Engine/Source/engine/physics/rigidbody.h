// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
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
class btEmptyShape;

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

	inline const Vector3& GetVelocity() const { return m_velocity; }
	void SetVelocity(const Vector3& _velocity);

	void ApplyTorque(const Vector3& torque);
	Vector3 GetTorque() const;

	void SetAngularVelocity(const Vector3& torque);
	void AddAngularVelocity(const Vector3& torque);
	Vector3 GetAngularVelocity() const;

	inline float GetDrag() const { return m_drag; }
	void SetDrag(float _drag);

	inline float GetAngularDrag() const { return m_angularDrag; }
	void SetAngularDrag(float _angularDrag);

	inline float GetBounce() const { return m_bounce; }
	void SetBounce(float _bounce);

	inline float GetGravityMultiplier() const  { return m_gravityMultiplier; }
	void SetGravityMultiplier(float _gravityMultiplier);

	inline float IsStatic() const  { return m_isStatic; }
	void SetIsStatic(float _isStatic);

	inline float GetMass() const { return m_mass; }
	void SetMass(float _mass);

	inline float GetFriction() const { return m_friction; }
	void SetFriction(float _friction);

	LockedAxis lockedMovementAxis;
	LockedAxis lockedRotationAxis;

	void Activate();

protected:
	void RemoveReferences()  override;

	bool m_disableEvent = false;
	std::vector<Collider*> m_colliders;
	bool m_generatesEvents = false;
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


	Vector3 m_velocity = Vector3(0, 0, 0);

	btRigidBody* m_bulletRigidbody = nullptr;
	btCompoundShape* m_bulletCompoundShape = nullptr;

	btRigidBody* m_bulletTriggerRigidbody = nullptr;
	btCompoundShape* m_bulletTriggerCompoundShape = nullptr;

	btEmptyShape* m_emptyShape = nullptr;

	float m_drag = 0.1f;
	float m_angularDrag = 0.1f;
	float m_bounce = 0.0f;
	float m_mass = 1;
	float m_gravityMultiplier = 1.0f;
	float m_friction = 0.1f;
	bool m_isStatic = false;
	bool m_isEmpty = false;
	bool m_isTriggerEmpty = false;

	/**
	 * @brief [Internal]
	 */
	void Tick();
};
