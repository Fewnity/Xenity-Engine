// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "box_collider.h"

#include <bullet/btBulletDynamicsCommon.h>

#include <engine/engine.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/graphics/renderer/renderer.h>

#include <engine/graphics/graphics.h>

#include <engine/game_elements/transform.h>
#include <engine/game_elements/gameobject.h>
#include <engine/graphics/3d_graphics/mesh_renderer.h>
#include <engine/tools/math.h>

#if defined(EDITOR)
#include <editor/editor.h>
#include <editor/gizmo.h>
#endif
#include "rigidbody.h"
#include "physics_manager.h"

BoxCollider::BoxCollider()
{
	componentName = "BoxCollider";
	AssetManager::AddReflection(this);
	CalculateBoundingBox();
}

ReflectiveData BoxCollider::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	AddVariable(reflectedVariables, size, "size", true);
	AddVariable(reflectedVariables, offset, "offset", true);
	AddVariable(reflectedVariables, isTrigger, "isTrigger", true);
	AddVariable(reflectedVariables, generateCollisionEvents, "generateCollisionEvents", true);
	return reflectedVariables;
}

void BoxCollider::OnReflectionUpdated()
{
	CalculateBoundingBox();
	if (std::shared_ptr<RigidBody> rb = attachedRigidbody.lock())
	{
		rb->UpdateGeneratesEvents();
	}
	OnTransformScaled();
	OnTransformUpdated();
}

void BoxCollider::OnTransformScaled()
{
	if (bulletCollisionShape)
	{
		const Vector3& scale = GetTransform()->GetScale();
		bulletCollisionShape->setLocalScaling(btVector3(size.x / 2.0f * scale.x, size.y / 2.0f * scale.y, size.z / 2.0f * scale.z));

		if (std::shared_ptr<RigidBody> rb = attachedRigidbody.lock())
		{
			rb->RemoveShape(bulletCollisionShape);
			rb->AddShape(bulletCollisionShape, offset * scale);
			rb->Activate();
		}
	}
}

void BoxCollider::OnTransformUpdated()
{
	if (bulletCollisionObject)
	{
		const Transform& transform = *GetTransform();

		const glm::mat4x4& matrix = transform.GetTransformationMatrix();
		Vector3 newPos = matrix * glm::vec4(-offset.x, offset.y, offset.z, 1);

		bulletCollisionObject->setWorldTransform(btTransform(
			btQuaternion(transform.GetRotation().x, transform.GetRotation().y, transform.GetRotation().z, transform.GetRotation().w),
			btVector3(-newPos.x, newPos.y, newPos.z)));
	}
}

bool BoxCollider::CheckTrigger(const BoxCollider& a, const BoxCollider& b)
{
	const Vector3& aPos = a.GetTransform()->GetPosition();
	const Vector3& bPos = b.GetTransform()->GetPosition();

	const Vector3 aMinPos = a.min + aPos;
	const Vector3 aMaxPos = a.max + aPos;
	const Vector3 bMinPos = b.min + bPos;
	const Vector3 bMaxPos = b.max + bPos;

	const bool xColl = aMinPos.x <= bMaxPos.x && aMaxPos.x >= bMinPos.x;
	const bool yColl = aMinPos.y <= bMaxPos.y && aMaxPos.y >= bMinPos.y;
	const bool zColl = aMinPos.z <= bMaxPos.z && aMaxPos.z >= bMinPos.z;

	if (xColl && yColl && zColl)
	{
		return true;
	}

	return false;
}

CollisionSide BoxCollider::CheckCollision(const BoxCollider& a, const BoxCollider& b, const Vector3& aVelocity)
{
	const Vector3& aPosition = a.GetTransform()->GetPosition();
	const Vector3& bPosition = b.GetTransform()->GetPosition();

	const Vector3 aMinPos = a.min + aPosition + aVelocity;
	const Vector3 aMaxPos = a.max + aPosition + aVelocity;
	const Vector3 bMinPos = b.min + bPosition;
	const Vector3 bMaxPos = b.max + bPosition;

	const bool xColl = aMinPos.x <= bMaxPos.x && aMaxPos.x >= bMinPos.x;
	const bool yColl = aMinPos.y <= bMaxPos.y && aMaxPos.y >= bMinPos.y;
	const bool zColl = aMinPos.z <= bMaxPos.z && aMaxPos.z >= bMinPos.z;
	int result = (int)CollisionSide::NoSide;

	if (xColl && yColl && zColl)
	{
		const Vector3 aMinPosBef = a.min + aPosition;
		const Vector3 aMaxPosBef = a.max + aPosition;
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
	GetTransform()->GetOnTransformScaled().Unbind(&BoxCollider::OnTransformScaled, this);
	GetTransform()->GetOnTransformUpdated().Unbind(&BoxCollider::OnTransformUpdated, this);

	AssetManager::RemoveReflection(this);
}

void BoxCollider::Awake()
{
	GetTransform()->GetOnTransformScaled().Bind(&BoxCollider::OnTransformScaled, this);
	GetTransform()->GetOnTransformUpdated().Bind(&BoxCollider::OnTransformUpdated, this);
	FindRigidbody();
}

void BoxCollider::Start()
{
	CreateCollision(false);
}

void BoxCollider::CreateCollision(bool forceCreation)
{
	if (!forceCreation && (bulletCollisionShape || bulletCollisionObject))
		return;

	if (bulletCollisionObject)
	{
		PhysicsManager::physDynamicsWorld->removeCollisionObject(bulletCollisionObject);

		delete bulletCollisionObject;
		bulletCollisionObject = nullptr;
	}

	const Vector3& scale = GetTransform()->GetScale();

	if (!bulletCollisionShape)
		bulletCollisionShape = new btBoxShape(btVector3(1, 1, 1));
	bulletCollisionShape->setLocalScaling(btVector3(size.x / 2.0f * scale.x, size.y / 2.0f * scale.y, size.z / 2.0f * scale.z));

	if (std::shared_ptr<RigidBody> rb = attachedRigidbody.lock())
	{
		if (!isTrigger)
			rb->AddShape(bulletCollisionShape, offset * scale);
		else
			rb->AddTriggerShape(bulletCollisionShape, offset * scale);
	}
	else
	{
		const glm::mat4x4& matrix = GetTransform()->GetTransformationMatrix();
		Vector3 newPos = matrix * glm::vec4(-offset.x, offset.y, offset.z, 1);

		const Quaternion& rot = GetTransform()->GetRotation();

		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(btVector3(-newPos.x, newPos.y, newPos.z));
		startTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

		bulletCollisionObject = new btCollisionObject();
		bulletCollisionObject->setCollisionShape(bulletCollisionShape);
		bulletCollisionObject->setWorldTransform(startTransform);
		bulletCollisionObject->setUserPointer(this);
		bulletCollisionObject->setRestitution(1);

		if (isTrigger)
		{
			bulletCollisionObject->setCollisionFlags(bulletCollisionObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		}

		PhysicsManager::physDynamicsWorld->addCollisionObject(bulletCollisionObject);
	}
}

void BoxCollider::OnDrawGizmosSelected()
{
#if defined(EDITOR)
	Color lineColor = Color::CreateFromRGBAFloat(0, 1, 0, 1);
	if (isTrigger)
		lineColor = Color::CreateFromRGBAFloat(0, 1, 0, 0.5f);

	Gizmo::SetColor(lineColor);

	const glm::mat4x4& matrix = GetTransform()->GetTransformationMatrix();
	Vector3 bottom0 = matrix * glm::vec4(-min.x, min.y, min.z, 1);
	Vector3 bottom1 = matrix * glm::vec4(-min.x, min.y, max.z, 1);
	Vector3 bottom2 = matrix * glm::vec4(-max.x, min.y, min.z, 1);
	Vector3 bottom3 = matrix * glm::vec4(-max.x, min.y, max.z, 1);

	Vector3 top0 = matrix * glm::vec4(-min.x, max.y, min.z, 1);
	Vector3 top1 = matrix * glm::vec4(-min.x, max.y, max.z, 1);
	Vector3 top2 = matrix * glm::vec4(-max.x, max.y, min.z, 1);
	Vector3 top3 = matrix * glm::vec4(-max.x, max.y, max.z, 1);

	bottom0.x = -bottom0.x;
	bottom1.x = -bottom1.x;
	bottom2.x = -bottom2.x;
	bottom3.x = -bottom3.x;

	top0.x = -top0.x;
	top1.x = -top1.x;
	top2.x = -top2.x;
	top3.x = -top3.x;


	Engine::GetRenderer().SetCameraPosition(*Graphics::usedCamera);

	// Bottom
	Gizmo::DrawLine(bottom0, bottom1);
	Gizmo::DrawLine(bottom0, bottom2);
	Gizmo::DrawLine(bottom3, bottom2);
	Gizmo::DrawLine(bottom3, bottom1);

	// Top
	Gizmo::DrawLine(top0, top1);
	Gizmo::DrawLine(top0, top2);
	Gizmo::DrawLine(top3, top2);
	Gizmo::DrawLine(top3, top1);

	// Bottom to top
	Gizmo::DrawLine(bottom0, top0);
	Gizmo::DrawLine(bottom1, top1);
	Gizmo::DrawLine(bottom2, top2);
	Gizmo::DrawLine(bottom3, top3);
#endif
}

void BoxCollider::SetDefaultSize()
{
	std::shared_ptr<MeshRenderer> mesh = GetGameObject()->GetComponent<MeshRenderer>();
	if (mesh && mesh->GetMeshData())
	{
		const std::shared_ptr<MeshData>& meshData = mesh->GetMeshData();
		size = ((meshData->GetMaxBoundingBox() - meshData->GetMinBoundingBox()));
		offset = ((meshData->GetMaxBoundingBox() + meshData->GetMinBoundingBox()) / 2.0f);
		CalculateBoundingBox();
	}
}

void BoxCollider::CalculateBoundingBox()
{
	min = -size / 2.0f + offset;
	max = size / 2.0f + offset;
}

void BoxCollider::SetSize(const Vector3& size)
{
	this->size = size;
	CalculateBoundingBox();
}


void BoxCollider::SetOffset(const Vector3& offset)
{
	this->offset = offset;
}