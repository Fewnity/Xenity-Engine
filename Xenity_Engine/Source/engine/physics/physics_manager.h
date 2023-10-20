#pragma once

#include <vector>
#include <memory>

class RigidBody;

class PhysicsManager
{
	void Init();
	void Update();
	static std::vector<std::weak_ptr<RigidBody>> rigidBodies;
};

