#pragma once

/**
 * [Internal]
 */

#include <vector>
#include <memory>

class RigidBody;
class BoxCollider;

class PhysicsManager
{
public:
	static void Init();
	static void Update();
	static std::vector<std::weak_ptr<RigidBody>> rigidBodies;
	static std::vector<std::weak_ptr<BoxCollider>> boxColliders;
};

