// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "sphere_collider.h"

#include <bullet/btBulletDynamicsCommon.h>

#include <engine/engine.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/graphics/renderer/renderer.h>

#include <engine/graphics/graphics.h>

#include <engine/game_elements/transform.h>
#include <engine/game_elements/gameobject.h>
#include <engine/graphics/3d_graphics/mesh_renderer.h>

#if defined(EDITOR)
#include <editor/editor.h>
#include <editor/gizmo.h>
#endif

#include "rigidbody.h"
#include "physics_manager.h"

SphereCollider::SphereCollider()
{
	componentName = "SphereCollider";
	AssetManager::AddReflection(this);
	CalculateBoundingBox();
}

ReflectiveData SphereCollider::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	AddVariable(reflectedVariables, size, "size", true);
	AddVariable(reflectedVariables, offset, "offset", true);
	AddVariable(reflectedVariables, isTrigger, "isTrigger", true);
	AddVariable(reflectedVariables, generateCollisionEvents, "generateCollisionEvents", true);
	return reflectedVariables;
}

void SphereCollider::OnReflectionUpdated()
{
	CalculateBoundingBox();
	if (std::shared_ptr<RigidBody> rb = attachedRigidbody.lock())
	{
		rb->UpdateGeneratesEvents();
	}
}

bool SphereCollider::CheckTrigger(const SphereCollider& a, const SphereCollider& b)
{
	return false;
}

SphereCollider::~SphereCollider()
{
	AssetManager::RemoveReflection(this);
}

void SphereCollider::Awake()
{
	FindRigidbody();
}

void SphereCollider::Start()
{
	CreateCollision(false);
}

void SphereCollider::OnDrawGizmosSelected()
{
#if defined(EDITOR)
	Color lineColor = Color::CreateFromRGBAFloat(0, 1, 0, 1);
	if (isTrigger)
		lineColor = Color::CreateFromRGBAFloat(0, 1, 0, 0.5f);

	Gizmo::SetColor(lineColor);

	Engine::GetRenderer().SetCameraPosition(*Graphics::usedCamera);

	const float maxScale = GetTransform()->GetScale().Max();
	const glm::mat4x4& matrix = GetTransform()->GetTransformationMatrix();
	Vector3 newPos = matrix * glm::vec4(-offset.x, offset.y, offset.z, 1);

	Gizmo::DrawSphere(Vector3(-newPos.x, newPos.y, newPos.z), size / 2 * maxScale);
#endif
}

void SphereCollider::CreateCollision(bool forceCreation)
{
	if (!forceCreation && (bulletCollisionShape || bulletCollisionObject))
		return;

	if (bulletCollisionObject)
	{
		PhysicsManager::physDynamicsWorld->removeCollisionObject(bulletCollisionObject);

		delete bulletCollisionObject;
		bulletCollisionObject = nullptr;
	}

	btVector3 localInertia(0, 0, 0);
	const Vector3& scale = GetTransform()->GetScale();
	if (!bulletCollisionShape)
		bulletCollisionShape = new btSphereShape(1);

	const float maxScale = scale.Max();
	bulletCollisionShape->setLocalScaling(btVector3(size / 2.0f * maxScale, size / 2.0f * maxScale, size / 2.0f * maxScale));

	if (attachedRigidbody.lock())
	{
		if (!isTrigger)
			attachedRigidbody.lock()->AddShape(bulletCollisionShape, offset * scale);
		else
			attachedRigidbody.lock()->AddTriggerShape(bulletCollisionShape, offset * scale);
	}
	else
	{
		const glm::mat4x4& matrix = GetTransform()->GetTransformationMatrix();
		Vector3 newPos = matrix * glm::vec4(-offset.x, offset.y, -offset.z, 1);

		const Quaternion& rot = GetTransform()->GetRotation();

		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(btVector3(-newPos.x, newPos.y, newPos.z));
		startTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

		bulletCollisionObject = new btCollisionObject();
		bulletCollisionObject->setCollisionShape(bulletCollisionShape);
		bulletCollisionObject->setWorldTransform(startTransform);
		bulletCollisionObject->setUserPointer(this);

		if (isTrigger)
		{
			bulletCollisionObject->setCollisionFlags(bulletCollisionObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		}

		PhysicsManager::physDynamicsWorld->addCollisionObject(bulletCollisionObject);
	}
}

void SphereCollider::SetDefaultSize()
{
	/*std::shared_ptr<MeshRenderer> mesh = GetGameObject()->GetComponent<MeshRenderer>();
	if (mesh && mesh->GetMeshData())
	{
		std::shared_ptr<MeshData> meshData = mesh->GetMeshData();
		const Vector3& scale = GetTransform()->GetLocalScale();
		size = (meshData->GetMaxBoundingBox() - meshData->GetMinBoundingBox()) * scale;
		offset = ((meshData->GetMaxBoundingBox() + meshData->GetMinBoundingBox()) / 2.0f) * scale;
		CalculateBoundingBox();
	}*/
}

void SphereCollider::CalculateBoundingBox()
{
	/*min = -size / 2.0f + offset * 2;
	max = size / 2.0f + offset * 2;*/
}

void SphereCollider::SetSize(const float& size)
{
	this->size = size;
	CalculateBoundingBox();
}


void SphereCollider::SetOffset(const Vector3& offset)
{
	this->offset = offset;
}