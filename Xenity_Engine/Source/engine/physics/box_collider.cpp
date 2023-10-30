#include "box_collider.h"
#include "../asset_management/asset_manager.h"
#include "../../xenity.h"
#include "../../xenity_editor.h"
#include "../graphics/renderer/renderer.h"

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
	AddVariable(reflectedVariables, size, "size", true);
	AddVariable(reflectedVariables, offset, "offset", true);
	//AddReflectionVariable(reflectedVariables, min, "min", true);
	//AddReflectionVariable(reflectedVariables, max, "max", true);
	AddVariable(reflectedVariables, isTrigger, "isTrigger", true);
	return reflectedVariables;
}

void BoxCollider::OnReflectionUpdated()
{
	min = -size / 2.0f;
	max = size / 2.0f;
}

bool BoxCollider::CheckTrigger(std::shared_ptr<BoxCollider> a, std::shared_ptr <BoxCollider> b)
{
	Vector3 aMinPos = a->min + a->GetTransform()->GetPosition() + a->offset;
	Vector3 aMaxPos = a->max + a->GetTransform()->GetPosition() + a->offset;
	Vector3 bMinPos = b->min + b->GetTransform()->GetPosition() + b->offset;
	Vector3 bMaxPos = b->max + b->GetTransform()->GetPosition() + b->offset;

	bool xColl = aMinPos.x <= bMaxPos.x && aMaxPos.x >= bMinPos.x;
	bool yColl = aMinPos.y <= bMaxPos.y && aMaxPos.y >= bMinPos.y;
	bool zColl = aMinPos.z <= bMaxPos.z && aMaxPos.z >= bMinPos.z;

	if (xColl && yColl && zColl)
	{
		return true;
	}

	return false;
}

CollisionSide BoxCollider::CheckCollision(std::shared_ptr<BoxCollider> a, std::shared_ptr <BoxCollider> b, Vector3 aVelocity)
{
	Vector3 aMinPos = a->min + a->GetTransform()->GetPosition() + a->offset + aVelocity;
	Vector3 aMaxPos = a->max + a->GetTransform()->GetPosition() + a->offset + aVelocity;
	Vector3 bMinPos = b->min + b->GetTransform()->GetPosition() + b->offset;
	Vector3 bMaxPos = b->max + b->GetTransform()->GetPosition() + b->offset;

	bool xColl = aMinPos.x <= bMaxPos.x && aMaxPos.x >= bMinPos.x;
	bool yColl = aMinPos.y <= bMaxPos.y && aMaxPos.y >= bMinPos.y;
	bool zColl = aMinPos.z <= bMaxPos.z && aMaxPos.z >= bMinPos.z;
	int result = NoSide;

	if (xColl && yColl && zColl)
	{
		Vector3 aMinPosBef = a->min + a->GetTransform()->GetPosition() + a->offset;
		Vector3 aMaxPosBef = a->max + a->GetTransform()->GetPosition() + a->offset;
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

void BoxCollider::OnDrawGizmos()
{
#if defined(EDITOR)
	if (Editor::GetSelectedGameObject().lock() != GetGameObject())
	{
		return;
	}
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