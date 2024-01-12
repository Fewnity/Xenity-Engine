#pragma once

/**
 * [Internal]
 */

#include <vector>
#include <memory>

class RigidBody;

class PhysicsManager
{
public:
	static void Init();
	static void Update();
	static std::vector<std::weak_ptr<RigidBody>> rigidBodies;
};

