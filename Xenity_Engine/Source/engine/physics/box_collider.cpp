#include "box_collider.h"
#include "../asset_management/asset_manager.h"
#include "../game_elements/transform.h"

BoxCollider::BoxCollider()
{
	componentName = "BoxCollider";
	AssetManager::AddReflection(this);
	min = -size / 2.0f;
	max = size / 2.0f;
}

std::unordered_map<std::string, ReflectionEntry> BoxCollider::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	AddReflectionVariable(reflectedVariables, size, "size", true);
	AddReflectionVariable(reflectedVariables, offset, "offset", true);
	AddReflectionVariable(reflectedVariables, min, "min", true);
	AddReflectionVariable(reflectedVariables, max, "max", true);
	return reflectedVariables;
}

void BoxCollider::OnReflectionUpdated()
{
	min = -size / 2.0f;
	max = size / 2.0f;
}

CollisionSide BoxCollider::CheckCollision(std::shared_ptr<BoxCollider> a, std::shared_ptr <BoxCollider> b, Vector3 aVelocity)
{
	Vector3 aMinPosBef = a->min + a->GetTransform()->GetPosition() + a->offset;
	Vector3 aMaxPosBef = a->max + a->GetTransform()->GetPosition() + a->offset;
	Vector3 aMinPos = a->min + a->GetTransform()->GetPosition() + a->offset + aVelocity;
	Vector3 aMaxPos = a->max + a->GetTransform()->GetPosition() + a->offset + aVelocity;
	Vector3 bMinPos = b->min + b->GetTransform()->GetPosition() + b->offset;
	Vector3 bMaxPos = b->max + b->GetTransform()->GetPosition() + b->offset;

	bool xCollBefore = aMinPosBef.x <= bMaxPos.x && aMaxPosBef.x >= bMinPos.x;
	bool yCollBefore = aMinPosBef.y <= bMaxPos.y && aMaxPosBef.y >= bMinPos.y;
	bool zCollBefore = aMinPosBef.z <= bMaxPos.z && aMaxPosBef.z >= bMinPos.z;
	bool xColl = aMinPos.x <= bMaxPos.x && aMaxPos.x >= bMinPos.x;
	bool yColl = aMinPos.y <= bMaxPos.y && aMaxPos.y >= bMinPos.y;
	bool zColl = aMinPos.z <= bMaxPos.z && aMaxPos.z >= bMinPos.z;
	int result = NoSide;

	if (xColl && yColl && zColl) 
	{
		if (!xCollBefore)
			result |= SideX;
		if (!yCollBefore)
			result |= SideY;
		if (!zCollBefore)
			result |= SideZ;
	}

	/*if (xColl)
		result |= SideX;
	if (yColl)
		result |= SideY;
	if (zColl)
		result |= SideZ;*/

	/*if (aMinPos.x <= bMaxPos.x &&
		aMaxPos.x >= bMinPos.x &&
		aMinPos.y <= bMaxPos.y &&
		aMaxPos.y >= bMinPos.y &&
		aMinPos.z <= bMaxPos.z &&
		aMaxPos.z >= bMinPos.z)
	{
		return SideX;
	}*/
	return (CollisionSide)result;
}

BoxCollider::~BoxCollider()
{
	AssetManager::RemoveReflection(this);
}