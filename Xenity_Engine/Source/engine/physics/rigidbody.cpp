#include "rigidbody.h"
#include "../asset_management/asset_manager.h"
#include "../../xenity.h"
#include "physics_manager.h"

RigidBody::RigidBody()
{
	componentName = "RigidBody";
	AssetManager::AddReflection(this);
}

std::unordered_map<std::string, ReflectionEntry> RigidBody::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	AddReflectionVariable(reflectedVariables, (std::weak_ptr<Component>&)attachedcollider, "attachedcollider", true);
	AddReflectionVariable(reflectedVariables, isStatic, "isStatic", true);
	AddReflectionVariable(reflectedVariables, gravityMultiplier, "gravityMultiplier", true);
	AddReflectionVariable(reflectedVariables, velocity, "velocity", true);
	AddReflectionVariable(reflectedVariables, drag, "drag", true);
	AddReflectionVariable(reflectedVariables, bounce, "bounce", true);
	return reflectedVariables;
}

void RigidBody::Update()
{
}

void RigidBody::Tick()
{
	if (attachedcollider.lock() && !isStatic)
	{
		int side = NoSide;
		int colliderCount = PhysicsManager::rigidBodies.size();
		for (int i = 0; i < colliderCount; i++)
		{
			std::shared_ptr<RigidBody> other = PhysicsManager::rigidBodies[i].lock();
			if (other != shared_from_this())
			{
				if (other->attachedcollider.lock()) 
				{
					int tempSide = BoxCollider::CheckCollision(attachedcollider.lock(), other->attachedcollider.lock(), velocity * Time::GetDeltaTime());
					if (tempSide != NoSide)
					{
						if((side & tempSide) == 0)
							side |= tempSide;
					}
				}
			}
		}

		Vector3 newVelocity = velocity;
		if ((side & SideX) != 0)
			newVelocity.x = -velocity.x * bounce;
		if ((side & SideY) != 0)
			newVelocity.y = -velocity.y * bounce;
		if ((side & SideZ) != 0)
			newVelocity.z = -velocity.z * bounce;

		if (newVelocity.Magnitude() != 0)
			GetTransform()->SetPosition(GetTransform()->GetPosition() + newVelocity * Time::GetDeltaTime());

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

RigidBody::~RigidBody()
{
	AssetManager::RemoveReflection(this);
}