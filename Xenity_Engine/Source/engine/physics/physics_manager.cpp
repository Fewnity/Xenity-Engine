#include "physics_manager.h"
#include "collider.h"
#include "box_collider.h"
#include "rigidbody.h"
#include "../game_elements/gameobject.h"

std::vector<std::weak_ptr<RigidBody>> PhysicsManager::rigidBodies;

void PhysicsManager::Init()
{
}

void PhysicsManager::Update()
{
	size_t colliderCount = rigidBodies.size();
	for (int i = 0; i < colliderCount; i++)
	{
		if (!rigidBodies[i].lock())
		{
			rigidBodies.erase(rigidBodies.begin() + i);
			i--;
			colliderCount--;
		}
	}

	for (int i = 0; i < colliderCount; i++)
	{
		std::shared_ptr<RigidBody> rb = rigidBodies[i].lock();
		rb->Tick();
	}
}
