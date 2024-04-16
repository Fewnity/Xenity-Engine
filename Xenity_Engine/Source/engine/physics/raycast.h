#pragma once

#include <vector>
#include <memory>
#include <engine/vectors/vector3.h>
#include <engine/api.h>

class RigidBody;
class BoxCollider;

struct RaycastHit 
{
	std::weak_ptr<GameObject> hitGameObject;
	Vector3 hitPosition = Vector3(0,0,0);
	float distance = 0;
};

class API Raycast
{
public:
	static bool Check(const Vector3& startPosition, const Vector3& direction, const float distance, RaycastHit& raycastHit);
	static bool Check(const std::weak_ptr<BoxCollider> boxCollider, const Vector3& startPosition, const Vector3& direction, const float distance, RaycastHit& raycastHit);
private:
};

