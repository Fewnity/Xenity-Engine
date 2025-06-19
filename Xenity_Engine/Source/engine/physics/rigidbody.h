// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <vector>

#include <engine/api.h>
#include <engine/component.h>
#include <engine/math/vector3.h>

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

	/**
	* @brief Get the velocity
	*/
	[[nodiscard]] const Vector3& GetVelocity() const { return m_velocity; }

	/**
	* @brief Set the velocity
	*/
	void SetVelocity(const Vector3& velocity);

	/**
	* @brief Get the torque applied to the rigidbody
	*/
	[[nodiscard]] Vector3 GetTorque() const;

	/**
	* @brief Apply torque
	*/
	void ApplyTorque(const Vector3& torque);

	/**
	* @brief Get the angular velocity
	*/
	[[nodiscard]] Vector3 GetAngularVelocity() const;

	/**
	* @brief Set the angular velocity
	*/
	void SetAngularVelocity(const Vector3& angularVelocity);

	/**
	* @brief Add angular velocity
	*/
	void AddAngularVelocity(const Vector3& angularVelocity);

	/**
	* @brief Get the drag value
	*/
	[[nodiscard]] float GetDrag() const { return m_drag; }
	
	/**
	* @brief Set the drag value
	*/
	void SetDrag(float drag);

	/**
	* @brief Get the angular drag value
	*/
	[[nodiscard]] float GetAngularDrag() const { return m_angularDrag; }

	/**
	* @brief Set the angular drag value
	*/
	void SetAngularDrag(float angularDrag);
	
	/**
	* @brief Get the bounce value
	*/
	[[nodiscard]] float GetBounce() const { return m_bounce; }
	
	/**
	* @brief Set the bounce value
	*/
	void SetBounce(float bounce);

	/**
	* @brief Get the gravity multiplier
	*/
	[[nodiscard]] float GetGravityMultiplier() const { return m_gravityMultiplier; }
	
	/**
	* @brief Set the gravity multiplier
	*/
	void SetGravityMultiplier(float gravityMultiplier);

	/**
	* @brief Get if the rigidbody is static
	*/
	[[nodiscard]] float IsStatic() const { return m_isStatic; }
	
	/**
	* @brief Set if the rigidbody is static
	*/
	void SetIsStatic(float isStatic);

	/**
	* @brief Get the mass of the rigidbody
	*/
	[[nodiscard]] float GetMass() const { return m_mass; }

	/**
	* @brief Set the mass of the rigidbody
	*/
	void SetMass(float mass);

	/**
	* @brief Get the friction value
	*/
	[[nodiscard]] float GetFriction() const { return m_friction; }

	/**
	* @brief Set the friction value
	*/
	void SetFriction(float friction);

	/**
	* @brief Activate the rigidbody (used to wake up the rigidbody if it was sleeping)
	*/
	void Activate();

	/**
	* @brief Get locked movement axis
	*/
	[[nodiscard]] const LockedAxis& GetLockedMovementAxis() const
	{
		return lockedMovementAxis;
	}

	/**
	* @brief Set locked movement axis
	*/
	void SetLockedMovementAxis(LockedAxis axis);

	/**
	* @brief Get locked rotation axis
	*/
	[[nodiscard]] const LockedAxis& GetLockedRotationAxis() const
	{
		return lockedRotationAxis;
	}

	/**
	* @brief Set locked rotation axis
	*/
	void SetLockedRotationAxis(LockedAxis axis);

protected:
	void RemoveReferences()  override;
	void Awake() override;
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

	std::vector<Collider*> m_colliders;
	LockedAxis lockedMovementAxis;
	LockedAxis lockedRotationAxis;

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
	bool m_generatesEvents = false;
	bool m_disableEvent = false;

	/**
	 * @brief [Internal]
	 */
	void Tick();
};
