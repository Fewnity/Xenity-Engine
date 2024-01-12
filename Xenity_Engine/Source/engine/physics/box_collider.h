#pragma once
#include <engine/api.h>

#include "collider.h"
#include <engine/vectors/vector3.h>
#include <memory>

enum CollisionSide {
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

	Vector3 size = Vector3(1);
	Vector3 offset = Vector3(0);


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
	bool isTrigger = false;

private:
	Vector3 min;
	Vector3 max;
};
