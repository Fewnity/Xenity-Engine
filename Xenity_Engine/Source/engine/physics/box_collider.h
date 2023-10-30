#pragma once

#include "collider.h"
#include "../vectors/vector3.h"
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

	//void SetReflection();
	Vector3 size = Vector3(1);
	Vector3 offset = Vector3(0);

	Vector3 min;
	Vector3 max;

	std::unordered_map<std::string, ReflectionEntry> GetReflection() override;
	void OnReflectionUpdated() override;
	static bool CheckTrigger(const std::shared_ptr<BoxCollider>& a, const std::shared_ptr <BoxCollider>& b);
	static CollisionSide CheckCollision(const std::shared_ptr <BoxCollider>& a, const std::shared_ptr <BoxCollider>& b, const Vector3& aVelocity);
	void OnDrawGizmos() override;
	bool isTrigger = false;
private:
};
