#include "box_collider.h"

#include <engine/engine.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/graphics/renderer/renderer.h>

#include <engine/graphics/graphics.h>

#include <engine/game_elements/transform.h>
#include <engine/game_elements/gameobject.h>

#if defined(EDITOR)
#include <editor/editor.h>
#include <editor/gizmo.h>
#endif
#include "rigidbody.h"

BoxCollider::BoxCollider()
{
	componentName = "BoxCollider";
	AssetManager::AddReflection(this);
	min = -size / 2.0f;
	max = size / 2.0f;
}

ReflectiveData BoxCollider::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	AddVariable(reflectedVariables, size, "size", true);
	AddVariable(reflectedVariables, offset, "offset", true);
	AddVariable(reflectedVariables, isTrigger, "isTrigger", true);
	return reflectedVariables;
}

void BoxCollider::OnReflectionUpdated()
{
	min = -size / 2.0f;
	max = size / 2.0f;
}

bool BoxCollider::CheckTrigger(const std::shared_ptr<BoxCollider> &a, const std::shared_ptr<BoxCollider> &b)
{
	const Vector3 aPos = a->GetTransform()->GetPosition() + a->offset;
	const Vector3 bPos = b->GetTransform()->GetPosition() + b->offset;

	const Vector3 aMinPos = a->min + aPos;
	const Vector3 aMaxPos = a->max + aPos;
	const Vector3 bMinPos = b->min + bPos;
	const Vector3 bMaxPos = b->max + bPos;

	const bool xColl = aMinPos.x <= bMaxPos.x && aMaxPos.x >= bMinPos.x;
	const bool yColl = aMinPos.y <= bMaxPos.y && aMaxPos.y >= bMinPos.y;
	const bool zColl = aMinPos.z <= bMaxPos.z && aMaxPos.z >= bMinPos.z;

	if (xColl && yColl && zColl)
	{
		return true;
	}

	return false;
}

CollisionSide BoxCollider::CheckCollision(const std::shared_ptr<BoxCollider> &a, const std::shared_ptr<BoxCollider> &b, const Vector3 &aVelocity)
{
	const Vector3& aPosition = a->GetTransform()->GetPosition();
	const Vector3& bPosition = b->GetTransform()->GetPosition();

	const Vector3 aMinPos = a->min + aPosition + a->offset + aVelocity;
	const Vector3 aMaxPos = a->max + aPosition + a->offset + aVelocity;
	const Vector3 bMinPos = b->min + bPosition + b->offset;
	const Vector3 bMaxPos = b->max + bPosition + b->offset;

	const bool xColl = aMinPos.x <= bMaxPos.x && aMaxPos.x >= bMinPos.x;
	const bool yColl = aMinPos.y <= bMaxPos.y && aMaxPos.y >= bMinPos.y;
	const bool zColl = aMinPos.z <= bMaxPos.z && aMaxPos.z >= bMinPos.z;
	int result = (int)CollisionSide::NoSide;

	if (xColl && yColl && zColl)
	{
		const Vector3 aMinPosBef = a->min + aPosition + a->offset;
		const Vector3 aMaxPosBef = a->max + aPosition + a->offset;
		const bool xCollBefore = aMinPosBef.x <= bMaxPos.x && aMaxPosBef.x >= bMinPos.x;
		const bool yCollBefore = aMinPosBef.y <= bMaxPos.y && aMaxPosBef.y >= bMinPos.y;
		const bool zCollBefore = aMinPosBef.z <= bMaxPos.z && aMaxPosBef.z >= bMinPos.z;

		if (!xCollBefore)
			result |= (int)CollisionSide::SideX;
		if (!yCollBefore)
			result |= (int)CollisionSide::SideY;
		if (!zCollBefore)
			result |= (int)CollisionSide::SideZ;
	}

	return (CollisionSide)result;
}

BoxCollider::~BoxCollider()
{
	AssetManager::RemoveReflection(this);
}

void BoxCollider::Awake()
{
	currentRigidbody = GetGameObject()->GetComponent<RigidBody>();
}

void BoxCollider::OnDrawGizmosSelected()
{
#if defined(EDITOR)
	Color lineColor = Color::CreateFromRGBAFloat(0, 1, 0, 1);
	if(isTrigger)
		lineColor = Color::CreateFromRGBAFloat(0, 1, 0, 0.5f);

	Gizmo::SetColor(lineColor);

	Vector3 pos = GetTransform()->GetPosition();
	pos.x = -pos.x;
	pos += offset;

	// Bottom vertex
	const Vector3 v1 = pos + Vector3(min.x, min.y, min.z);
	const Vector3 v2 = pos + Vector3(min.x, min.y, max.z);
	const Vector3 v3 = pos + Vector3(max.x, min.y, min.z);
	const Vector3 v4 = pos + Vector3(max.x, min.y, max.z);

	// Top vertex
	const Vector3 v5 = pos + Vector3(min.x, max.y, min.z);
	const Vector3 v6 = pos + Vector3(min.x, max.y, max.z);
	const Vector3 v7 = pos + Vector3(max.x, max.y, min.z);
	const Vector3 v8 = pos + Vector3(max.x, max.y, max.z);

	Engine::GetRenderer().SetCameraPosition(Graphics::usedCamera.lock());

	// Bottom
	Gizmo::DrawLine(v1, v2);
	Gizmo::DrawLine(v1, v3);
	Gizmo::DrawLine(v4, v3);
	Gizmo::DrawLine(v4, v2);

	// Top
	Gizmo::DrawLine(v5, v6);
	Gizmo::DrawLine(v5, v7);
	Gizmo::DrawLine(v8, v7);
	Gizmo::DrawLine(v8, v6);

	// Bottom to top
	Gizmo::DrawLine(v1, v5);
	Gizmo::DrawLine(v2, v6);
	Gizmo::DrawLine(v3, v7);
	Gizmo::DrawLine(v4, v8);
#endif
}