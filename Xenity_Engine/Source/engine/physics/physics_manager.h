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
	/**
	* @brief Initialize the physics manager
	*/
	static void Init();

	/**
	* @brief Update the physics manager
	*/
	static void Update();

	static std::vector<std::weak_ptr<RigidBody>> rigidBodies;
	static std::vector<std::weak_ptr<BoxCollider>> boxColliders;
};

