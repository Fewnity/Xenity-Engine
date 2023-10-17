#include "box_collider.h"
#include "../asset_management/asset_manager.h"


BoxCollider::BoxCollider()
{
	componentName = "BoxCollider";
	AssetManager::AddReflection(this);
}

std::unordered_map<std::string, ReflectionEntry> BoxCollider::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	return reflectedVariables;
}

BoxCollider::~BoxCollider()
{
	AssetManager::RemoveReflection(this);
}