#include "physics_manager.h"
#include "collider.h"
#include "box_collider.h"
#include "rigidbody.h"

std::vector<std::weak_ptr<RigidBody>> PhysicsManager::rigidBodies;

void PhysicsManager::Init()
{
}

void PhysicsManager::Update()
{
	int colliderCount = rigidBodies.size();
	for (int i = 0; i < colliderCount; i++)
	{
		if (rigidBodies[i].lock()) 
		{
			rigidBodies[i].lock()->Tick();
		}
		else 
		{
			rigidBodies.erase(rigidBodies.begin() + i);
			i--;
			colliderCount--;
		}
	}
}
