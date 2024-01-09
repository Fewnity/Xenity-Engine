#include "box_collider.h"

#include <engine/engine.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/graphics/renderer/renderer.h>

#include <engine/graphics/graphics.h>

#include <engine/game_elements/transform.h>

#if defined(EDITOR)
#include <editor/editor.h>
#include <editor/gizmo.h>
#endif

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
	// AddReflectionVariable(reflectedVariables, min, "min", true);
	// AddReflectionVariable(reflectedVariables, max, "max", true);
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
	Vector3 aPos = a->GetTransform()->GetPosition() + a->offset;
	Vector3 bPos = b->GetTransform()->GetPosition() + b->offset;

	Vector3 aMinPos = a->min + aPos;
	Vector3 aMaxPos = a->max + aPos;
	Vector3 bMinPos = b->min + bPos;
	Vector3 bMaxPos = b->max + bPos;

	bool xColl = aMinPos.x <= bMaxPos.x && aMaxPos.x >= bMinPos.x;
	bool yColl = aMinPos.y <= bMaxPos.y && aMaxPos.y >= bMinPos.y;
	bool zColl = aMinPos.z <= bMaxPos.z && aMaxPos.z >= bMinPos.z;

	if (xColl && yColl && zColl)
	{
		return true;
	}

	return false;
}

CollisionSide BoxCollider::CheckCollision(const std::shared_ptr<BoxCollider> &a, const std::shared_ptr<BoxCollider> &b, const Vector3 &aVelocity)
{
	Vector3 aPosition = a->GetTransform()->GetPosition();
	Vector3 bPosition = b->GetTransform()->GetPosition();

	Vector3 aMinPos = a->min + aPosition + a->offset + aVelocity;
	Vector3 aMaxPos = a->max + aPosition + a->offset + aVelocity;
	Vector3 bMinPos = b->min + bPosition + b->offset;
	Vector3 bMaxPos = b->max + bPosition + b->offset;

	bool xColl = aMinPos.x <= bMaxPos.x && aMaxPos.x >= bMinPos.x;
	bool yColl = aMinPos.y <= bMaxPos.y && aMaxPos.y >= bMinPos.y;
	bool zColl = aMinPos.z <= bMaxPos.z && aMaxPos.z >= bMinPos.z;
	int result = NoSide;

	if (xColl && yColl && zColl)
	{
		Vector3 aMinPosBef = a->min + aPosition + a->offset;
		Vector3 aMaxPosBef = a->max + aPosition + a->offset;
		bool xCollBefore = aMinPosBef.x <= bMaxPos.x && aMaxPosBef.x >= bMinPos.x;
		bool yCollBefore = aMinPosBef.y <= bMaxPos.y && aMaxPosBef.y >= bMinPos.y;
		bool zCollBefore = aMinPosBef.z <= bMaxPos.z && aMaxPosBef.z >= bMinPos.z;

		if (!xCollBefore)
			result |= SideX;
		if (!yCollBefore)
			result |= SideY;
		if (!zCollBefore)
			result |= SideZ;
	}

	return (CollisionSide)result;
}

BoxCollider::~BoxCollider()
{
	AssetManager::RemoveReflection(this);
}

void BoxCollider::OnDrawGizmosSelected()
{
#if defined(EDITOR)
	Color lineColor = Color::CreateFromRGBAFloat(0, 1, 0, 1);
	Gizmo::SetColor(lineColor);

	Vector3 pos = GetTransform()->GetPosition();
	pos.x = -pos.x;
	pos += offset;

	// Bottom vertex
	Vector3 v1 = pos + Vector3(min.x, min.y, min.z);
	Vector3 v2 = pos + Vector3(min.x, min.y, max.z);
	Vector3 v3 = pos + Vector3(max.x, min.y, min.z);
	Vector3 v4 = pos + Vector3(max.x, min.y, max.z);

	// Top vertex
	Vector3 v5 = pos + Vector3(min.x, max.y, min.z);
	Vector3 v6 = pos + Vector3(min.x, max.y, max.z);
	Vector3 v7 = pos + Vector3(max.x, max.y, min.z);
	Vector3 v8 = pos + Vector3(max.x, max.y, max.z);

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