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
	static bool CheckTrigger(std::shared_ptr<BoxCollider> a, std::shared_ptr <BoxCollider> b);
	static CollisionSide CheckCollision(std::shared_ptr <BoxCollider> a, std::shared_ptr <BoxCollider> b, Vector3 aVelocity);
	void OnDrawGizmos() override;
	bool isTrigger = false;
private:
};
