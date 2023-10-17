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
	return reflectedVariables;
}

void RigidBody::Update()
{
	GetTransform()->SetPosition(GetTransform()->GetPosition() + velocity);
}

RigidBody::~RigidBody()
{
	AssetManager::RemoveReflection(this);
}