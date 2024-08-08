// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "sphere_collider.h"

#include <bullet/btBulletDynamicsCommon.h>

#if defined(EDITOR)
#include <editor/editor.h>
#include <editor/gizmo.h>
#endif

#include <engine/engine.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/graphics/renderer/renderer.h>
#include <engine/graphics/graphics.h>
#include <engine/game_elements/transform.h>
#include <engine/game_elements/gameobject.h>
#include <engine/graphics/3d_graphics/mesh_renderer.h>
#include <engine/tools/math.h>
#include "rigidbody.h"
#include "physics_manager.h"

SphereCollider::SphereCollider()
{
	componentName = "SphereCollider";
	AssetManager::AddReflection(this);
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
	if (std::shared_ptr<RigidBody> rb = attachedRigidbody.lock())
	{
		rb->UpdateGeneratesEvents();
	}
	OnTransformScaled();
	OnTransformUpdated();
}

bool SphereCollider::CheckTrigger(const SphereCollider& a, const SphereCollider& b)
{
	return false;
}

SphereCollider::~SphereCollider()
{
	GetTransform()->GetOnTransformScaled().Unbind(&SphereCollider::OnTransformScaled, this);
	GetTransform()->GetOnTransformUpdated().Unbind(&SphereCollider::OnTransformUpdated, this);
	AssetManager::RemoveReflection(this);
}

void SphereCollider::Awake()
{
	GetTransform()->GetOnTransformScaled().Bind(&SphereCollider::OnTransformScaled, this);
	GetTransform()->GetOnTransformUpdated().Bind(&SphereCollider::OnTransformUpdated, this);
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
	const Vector3 newPos = matrix * glm::vec4(-offset.x, offset.y, offset.z, 1);

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

	const Vector3& scale = GetTransform()->GetScale();
	if (!bulletCollisionShape)
		bulletCollisionShape = new btSphereShape(1);

	const float maxScale = scale.Max();
	bulletCollisionShape->setLocalScaling(btVector3(size / 2.0f * maxScale, size / 2.0f * maxScale, size / 2.0f * maxScale));

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
		const Vector3 newPos = matrix * glm::vec4(-offset.x, offset.y, -offset.z, 1);

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

void SphereCollider::OnTransformScaled()
{
	if (bulletCollisionShape)
	{
		const Vector3& scale = GetTransform()->GetScale();
		const float maxScale = scale.Max();
		bulletCollisionShape->setLocalScaling(btVector3(size / 2.0f * maxScale, size / 2.0f * maxScale, size / 2.0f * maxScale));

		if (std::shared_ptr<RigidBody> rb = attachedRigidbody.lock())
		{
			rb->RemoveShape(bulletCollisionShape);
			rb->AddShape(bulletCollisionShape, offset * scale);
			rb->Activate();
		}
	}
}

void SphereCollider::OnTransformUpdated()
{
	if (bulletCollisionObject)
	{
		const Transform& transform = *GetTransform();

		const glm::mat4x4& matrix = transform.GetTransformationMatrix();
		const Vector3 newPos = matrix * glm::vec4(-offset.x, offset.y, offset.z, 1);

		bulletCollisionObject->setWorldTransform(btTransform(
			btQuaternion(transform.GetRotation().x, transform.GetRotation().y, transform.GetRotation().z, transform.GetRotation().w),
			btVector3(-newPos.x, newPos.y, newPos.z)));
	}
}

void SphereCollider::SetDefaultSize()
{
	std::shared_ptr<MeshRenderer> mesh = GetGameObject()->GetComponent<MeshRenderer>();
	if (mesh && mesh->GetMeshData())
	{
		const std::shared_ptr<MeshData>& meshData = mesh->GetMeshData();
		size = ((meshData->GetMaxBoundingBox() - meshData->GetMinBoundingBox())).Max();
		offset = ((meshData->GetMaxBoundingBox() + meshData->GetMinBoundingBox()) / 2.0f);
	}
}

void SphereCollider::SetSize(const float& size)
{
	this->size = size;
}


void SphereCollider::SetOffset(const Vector3& offset)
{
	this->offset = offset;
}