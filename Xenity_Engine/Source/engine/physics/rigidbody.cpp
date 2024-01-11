#include "rigidbody.h"

#include "physics_manager.h"
#include "box_collider.h"

#include <engine/asset_management/asset_manager.h>
#include <engine/time/time.h>

#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>

RigidBody::RigidBody()
{
	componentName = "RigidBody";
	AssetManager::AddReflection(this);
}

ReflectiveData RigidBody::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	AddVariable(reflectedVariables, attachedcollider, "attachedcollider", true);
	AddVariable(reflectedVariables, isStatic, "isStatic", true);
	AddVariable(reflectedVariables, gravityMultiplier, "gravityMultiplier", true);
	AddVariable(reflectedVariables, velocity, "velocity", true);
	AddVariable(reflectedVariables, drag, "drag", true);
	AddVariable(reflectedVariables, bounce, "bounce", true);
	return reflectedVariables;
}

void RigidBody::Update()
{
}

void RigidBody::Tick()
{
	inTrigger.clear();
	if (GetGameObject()->GetLocalActive() && GetIsEnabled()) 
	{
		if (auto attachedColliderLock = attachedcollider.lock())
		{
			int side = NoSide;
			const size_t colliderCount = PhysicsManager::rigidBodies.size();
			std::shared_ptr<RigidBody> other = nullptr;

			// Check if the rigidbody is colliding with a collider or is in a trigger
			for (int i = 0; i < colliderCount; i++)
			{
				other = PhysicsManager::rigidBodies[i].lock();
				if (other != shared_from_this() && other->GetIsEnabled() && other->GetGameObject()->GetLocalActive())
				{
					std::shared_ptr<BoxCollider> otherCollider = other->attachedcollider.lock();
					if (otherCollider && otherCollider->GetIsEnabled() && otherCollider->GetGameObject()->GetLocalActive())
					{
						if (attachedColliderLock->isTrigger && isStatic && !other->isStatic) // Check trigger
						{
							const bool trigger = BoxCollider::CheckTrigger(attachedColliderLock, otherCollider);
							if (trigger)
								inTrigger.push_back(otherCollider);
						}
						else if (!otherCollider->isTrigger && !isStatic) // Check collision
						{
							const int tempSide = BoxCollider::CheckCollision(attachedColliderLock, otherCollider, velocity * Time::GetDeltaTime());
							if (tempSide != NoSide)
							{
								if ((side & tempSide) == 0)
									side |= tempSide;
							}
						}
					}
				}
			}

			if (!isStatic)
			{
				Vector3 newVelocity = velocity;
				// Make the rigidbody bounce if there is a wall in the opposite direction
				if ((side & SideX) != 0)
					newVelocity.x = -velocity.x * bounce;
				if ((side & SideY) != 0)
					newVelocity.y = -velocity.y * bounce;
				if ((side & SideZ) != 0)
					newVelocity.z = -velocity.z * bounce;

				// Move the rigidbody
				if (newVelocity.Magnitude() != 0)
					GetTransform()->SetPosition(GetTransform()->GetPosition() + newVelocity * Time::GetDeltaTime());

				// Apply gravity
				if ((side & SideY) == 0)
				{
					newVelocity.y -= 9.81f * gravityMultiplier * Time::GetDeltaTime();
					if (newVelocity.y <= -10)
					{
						newVelocity.y = -10;
					}
				}
				velocity = newVelocity;
			}
		}
	}
}

RigidBody::~RigidBody()
{
	AssetManager::RemoveReflection(this);
}