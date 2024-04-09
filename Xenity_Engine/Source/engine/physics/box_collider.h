#pragma once
#include <engine/api.h>

#include "collider.h"
#include <engine/vectors/vector3.h>
#include <memory>

class RigidBody;

enum class CollisionSide 
{
	NoSide = 0,
	SideX = 1,
	SideY = 2,
	SideZ = 4,
};

class API BoxCollider : public Collider
{
public:
	BoxCollider();
	~BoxCollider();

	void Awake() override;

	Vector3 size = Vector3(1);
	Vector3 offset = Vector3(0);
	std::weak_ptr<RigidBody> currentRigidbody;

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;
	
	/**
	 * [Internal]
	 */
	static bool CheckTrigger(const std::shared_ptr<BoxCollider>& a, const std::shared_ptr <BoxCollider>& b);
	
	/**
	 * [Internal]
	 */
	static CollisionSide CheckCollision(const std::shared_ptr <BoxCollider>& a, const std::shared_ptr <BoxCollider>& b, const Vector3& aVelocity);
	
	void OnDrawGizmosSelected() override;
	void SetDefaultSize();
	void CalculateBoundingBox();

	bool isTrigger = false;

	Vector3 min;
	Vector3 max;
private:
};
