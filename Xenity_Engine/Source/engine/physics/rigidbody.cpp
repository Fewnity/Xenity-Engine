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
	if (attachedcollider.lock())
	{
		int side = NoSide;
		int colliderCount = PhysicsManager::rigidBodies.size();
		for (int i = 0; i < colliderCount; i++)
		{
			std::shared_ptr<RigidBody> other = PhysicsManager::rigidBodies[i].lock();
			if (other != shared_from_this() && other->GetIsEnabled() && other->GetGameObject()->GetLocalActive())
			{
				std::shared_ptr<BoxCollider> otherCollider = other->attachedcollider.lock();
				if (otherCollider && otherCollider->GetIsEnabled() && otherCollider->GetGameObject()->GetLocalActive())
				{
					if (attachedcollider.lock()->isTrigger && isStatic)
					{
						bool trigger = BoxCollider::CheckTrigger(attachedcollider.lock(), otherCollider);
						if (trigger)
							inTrigger.push_back(otherCollider);
					}
					else if (!otherCollider->isTrigger && !isStatic)
					{
						int tempSide = BoxCollider::CheckCollision(attachedcollider.lock(), otherCollider, velocity * Time::GetDeltaTime());
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
}

RigidBody::~RigidBody()
{
	AssetManager::RemoveReflection(this);
}