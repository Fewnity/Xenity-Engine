#include "rigidbody.h"
#include "../asset_management/asset_manager.h"
#include "../../xenity.h"

RigidBody::RigidBody()
{
	componentName = "RigidBody";
	AssetManager::AddReflection(this);
}

std::unordered_map<std::string, ReflectionEntry> RigidBody::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	AddReflectionVariable(reflectedVariables, attachedcollider, "attachedcollider", true);
	return reflectedVariables;
}

void RigidBody::Update()
{
	GetTransform()->SetPosition(GetTransform()->GetPosition() + velocity * Time::GetDeltaTime());
	velocity.y -= 9.81f * gravityMultiplier * Time::GetDeltaTime();

}

void RigidBody::Tick()
{
}

RigidBody::~RigidBody()
{
	AssetManager::RemoveReflection(this);
}