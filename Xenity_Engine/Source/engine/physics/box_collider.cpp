// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "box_collider.h"

#include <iostream>

#include <bullet/btBulletDynamicsCommon.h>

#if defined(EDITOR)
#include <editor/gizmo.h>
#endif

#include <engine/engine.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/graphics/renderer/renderer.h>
#include <engine/graphics/graphics.h>
#include <engine/game_elements/transform.h>
#include <engine/game_elements/gameobject.h>
#include <engine/graphics/3d_graphics/mesh_renderer.h>
#include <engine/debug/stack_debug_object.h>
#include "rigidbody.h"
#include "physics_manager.h"

BoxCollider::BoxCollider()
{
	AssetManager::AddReflection(this);
	CalculateBoundingBox();
}

ReflectiveData BoxCollider::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	AddVariable(reflectedVariables, s_size, "size", true);
	AddVariable(reflectedVariables, m_offset, "offset", true);
	AddVariable(reflectedVariables, m_isTrigger, "isTrigger", true);
	AddVariable(reflectedVariables, m_generateCollisionEvents, "generateCollisionEvents", true);
	return reflectedVariables;
}

void BoxCollider::OnReflectionUpdated()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	CalculateBoundingBox();
	if (const std::shared_ptr<RigidBody> rb = m_attachedRigidbody.lock())
	{
		rb->UpdateGeneratesEvents();
	}
	OnTransformScaled();
	OnTransformUpdated();
}

void BoxCollider::OnTransformScaled()
{
	if (m_bulletCollisionShape)
	{
		const Vector3& scale = GetTransform()->GetScale();
		m_bulletCollisionShape->setLocalScaling(btVector3(s_size.x / 2.0f * scale.x, s_size.y / 2.0f * scale.y, s_size.z / 2.0f * scale.z));

		if (const std::shared_ptr<RigidBody> rb = m_attachedRigidbody.lock())
		{
			rb->RemoveShape(m_bulletCollisionShape);
			rb->RemoveTriggerShape(m_bulletCollisionShape);
			if (!m_isTrigger)
				rb->AddShape(m_bulletCollisionShape, m_offset * scale);
			else
				rb->AddTriggerShape(m_bulletCollisionShape, m_offset * scale);
			rb->Activate();
		}
	}
}

void BoxCollider::OnTransformUpdated()
{
	if (m_bulletCollisionObject)
	{
		const Transform& transform = *GetTransform();

		const glm::mat4x4& matrix = transform.GetTransformationMatrix();
		const Vector3 newPos = matrix * glm::vec4(-m_offset.x, m_offset.y, m_offset.z, 1);

		m_bulletCollisionObject->setWorldTransform(btTransform(
			btQuaternion(transform.GetRotation().x, transform.GetRotation().y, transform.GetRotation().z, transform.GetRotation().w),
			btVector3(-newPos.x, newPos.y, newPos.z)));
	}
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
	if (!forceCreation && (m_bulletCollisionShape || m_bulletCollisionObject))
		return;

	if (m_bulletCollisionObject)
	{
		PhysicsManager::s_physDynamicsWorld->removeCollisionObject(m_bulletCollisionObject);

		delete m_bulletCollisionObject;
		m_bulletCollisionObject = nullptr;
	}

	const Vector3& scale = GetTransform()->GetScale();

	if (!m_bulletCollisionShape) 
	{
		m_bulletCollisionShape = new btBoxShape(btVector3(1, 1, 1));
	}

	m_bulletCollisionShape->setLocalScaling(btVector3(s_size.x / 2.0f * scale.x, s_size.y / 2.0f * scale.y, s_size.z / 2.0f * scale.z));
	m_bulletCollisionShape->setUserPointer(this);

	if (const std::shared_ptr<RigidBody> rb = m_attachedRigidbody.lock())
	{
		if (!m_isTrigger)
			rb->AddShape(m_bulletCollisionShape, m_offset * scale);
		else
			rb->AddTriggerShape(m_bulletCollisionShape, m_offset * scale);
	}
	else
	{
		const glm::mat4x4& matrix = GetTransform()->GetTransformationMatrix();
		const Vector3 newPos = matrix * glm::vec4(-m_offset.x, m_offset.y, m_offset.z, 1);

		const Quaternion& rot = GetTransform()->GetRotation();

		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(btVector3(-newPos.x, newPos.y, newPos.z));
		startTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

		m_bulletCollisionObject = new btCollisionObject();
		m_bulletCollisionObject->setCollisionShape(m_bulletCollisionShape);
		m_bulletCollisionObject->setWorldTransform(startTransform);
		m_bulletCollisionObject->setUserPointer(this);
		m_bulletCollisionObject->setRestitution(1);

		if (m_isTrigger)
		{
			m_bulletCollisionObject->setCollisionFlags(m_bulletCollisionObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		}

		PhysicsManager::s_physDynamicsWorld->addCollisionObject(m_bulletCollisionObject);
	}
}

void BoxCollider::OnDrawGizmosSelected()
{
#if defined(EDITOR)
	Color lineColor = Color::CreateFromRGBAFloat(0, 1, 0, 1);
	if (m_isTrigger)
		lineColor = Color::CreateFromRGBAFloat(0, 1, 0, 0.5f);

	Gizmo::SetColor(lineColor);

	const glm::mat4x4& matrix = GetTransform()->GetTransformationMatrix();
	Vector3 bottom0 = matrix * glm::vec4(-m_min.x, m_min.y, m_min.z, 1);
	Vector3 bottom1 = matrix * glm::vec4(-m_min.x, m_min.y, m_max.z, 1);
	Vector3 bottom2 = matrix * glm::vec4(-m_max.x, m_min.y, m_min.z, 1);
	Vector3 bottom3 = matrix * glm::vec4(-m_max.x, m_min.y, m_max.z, 1);

	Vector3 top0 = matrix * glm::vec4(-m_min.x, m_max.y, m_min.z, 1);
	Vector3 top1 = matrix * glm::vec4(-m_min.x, m_max.y, m_max.z, 1);
	Vector3 top2 = matrix * glm::vec4(-m_max.x, m_max.y, m_min.z, 1);
	Vector3 top3 = matrix * glm::vec4(-m_max.x, m_max.y, m_max.z, 1);

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
	const std::shared_ptr<MeshRenderer> mesh = GetGameObject()->GetComponent<MeshRenderer>();
	if (mesh && mesh->GetMeshData())
	{
		const std::shared_ptr<MeshData>& meshData = mesh->GetMeshData();
		s_size = ((meshData->GetMaxBoundingBox() - meshData->GetMinBoundingBox()));
		m_offset = ((meshData->GetMaxBoundingBox() + meshData->GetMinBoundingBox()) / 2.0f);
		CalculateBoundingBox();
	}
}

void BoxCollider::CalculateBoundingBox()
{
	m_min = -s_size / 2.0f + m_offset;
	m_max = s_size / 2.0f + m_offset;
}

void BoxCollider::SetSize(const Vector3& size)
{
	this->s_size = size;
	CalculateBoundingBox();
}


void BoxCollider::SetOffset(const Vector3& offset)
{
	this->m_offset = offset;
}

std::string BoxCollider::ToString()
{
	return s_size.ToString();
}
