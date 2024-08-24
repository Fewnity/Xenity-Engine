// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "mesh_renderer.h"

#include <engine/graphics/renderer/renderer.h>
#include <engine/graphics/3d_graphics/mesh_manager.h>
#include <engine/graphics/material.h>
#include <engine/game_elements/transform.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>

using namespace std;

// #pragma region MeshRenderer Constructors / Destructor

bool IsSphereInFrustum(const Frustum& frustum, const Sphere& sphere)
{
	for (const Plane& plane : frustum.planes)
	{
		// Calcul de la distance du centre de la sph�re au plan
		float distance = plane.A * sphere.position.x +
			plane.B * sphere.position.y +
			plane.C * sphere.position.z +
			plane.D;

		// Si la distance est inf�rieure � -radius, la sph�re est compl�tement hors du frustum
		if (distance < -sphere.radius)
		{
			return false;
		}
	}
	return true;
}

/// <summary>
/// Instantiate an empty mesh
/// </summary>
MeshRenderer::MeshRenderer()
{
	m_componentName = "MeshRenderer";
	AssetManager::AddReflection(this);
}

void MeshRenderer::OnDrawGizmosSelected()
{
#if defined(EDITOR)
	//Engine::GetRenderer().SetCameraPosition(*Graphics::usedCamera);
	//Sphere sphere = GetBoundingSphere();
	//Gizmo::DrawSphere(sphere.position, sphere.radius);
#endif
}

Sphere MeshRenderer::GetBoundingSphere() const
{
	Sphere sphere;
	if(!m_meshData)
		return sphere;

	Vector3 spherePosition = (m_meshData->m_minBoundingBox + m_meshData->m_maxBoundingBox) / 2.0f;
	glm::vec3 transformedPosition = glm::vec3(GetTransform()->GetTransformationMatrix() * glm::vec4(spherePosition.x, spherePosition.y, spherePosition.z, 1.0f));
	spherePosition = Vector3(-transformedPosition.x, transformedPosition.y, transformedPosition.z);

	Vector3 halfDiagonal = (m_meshData->m_maxBoundingBox - m_meshData->m_minBoundingBox) / 2.0f;
	const Vector3& scale = GetTransform()->GetScale();
	float spshereRadius = sqrt(halfDiagonal.x * halfDiagonal.x + halfDiagonal.y * halfDiagonal.y + halfDiagonal.z * halfDiagonal.z) * std::max({ scale.x, scale.y, scale.z });

	sphere.position = spherePosition;
	sphere.radius = spshereRadius;
	return sphere;
}

void MeshRenderer::OnNewRender()
{
	if(Graphics::usedCamera)
		outOfFrustum = !IsSphereInFrustum(Graphics::usedCamera->frustum, boundingSphere);
}

void MeshRenderer::OnComponentAttached()
{
	GetTransform()->GetOnTransformUpdated().Bind(&MeshRenderer::OnTransformPositionUpdated, this);
}

ReflectiveData MeshRenderer::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, m_meshData, "meshData", true);
	Reflective::AddVariable(reflectedVariables, m_materials, "materials", true);
	return reflectedVariables;
}

void MeshRenderer::OnReflectionUpdated()
{
	if (m_meshData)
		m_materials.resize(m_meshData->m_subMeshCount);

	m_matCount = m_materials.size();
	Graphics::isRenderingBatchDirty = true;

	boundingSphere = GetBoundingSphere();
}

/// <summary>
/// Destructor
/// </summary>
MeshRenderer::~MeshRenderer()
{
	GetTransform()->GetOnTransformUpdated().Unbind(&MeshRenderer::OnTransformPositionUpdated, this);
	AssetManager::RemoveReflection(this);
}

void MeshRenderer::CreateRenderCommands(RenderBatch& renderBatch)
{
	if (!m_meshData)
		return;

	// Create a command for each submesh
	const int subMeshCount = m_meshData->m_subMeshCount;
	std::shared_ptr<Material> material = nullptr;
	for (int i = 0; i < subMeshCount; i++)
	{
		if (i != m_matCount)
			material = m_materials[i];
		else
			break;
		if (material == nullptr)
			continue;

		RenderCommand command = RenderCommand();
		command.material = material.get();
		command.drawable = this;
		command.subMesh = m_meshData->m_subMeshes[i];
		command.transform = GetTransform().get();
		command.isEnabled = IsEnabled() && GetGameObject()->IsLocalActive();
		if (!material->GetUseTransparency())
		{
			RenderQueue& renderQueue = renderBatch.renderQueues[material->m_fileId];
			renderQueue.commands.push_back(command);
			renderQueue.commandIndex++;
		}
		else
		{
			renderBatch.transparentMeshCommands.push_back(command);
			renderBatch.transparentMeshCommandIndex++;
		}
	}
}

void MeshRenderer::SetMeshData(std::shared_ptr<MeshData> meshData)
{
	this->m_meshData = meshData;
	if (meshData) 
	{
		m_materials.resize(meshData->m_subMeshCount);
		m_matCount = meshData->m_subMeshCount;
	}
	else 
	{

	}
	Graphics::isRenderingBatchDirty = true;
}

void MeshRenderer::SetMaterial(std::shared_ptr<Material> material, int index)
{
	XASSERT(index < m_materials.size(), "[MeshRenderer::SetMaterial] Index is out of bounds");
	if (index < m_materials.size())
	{
		m_materials[index] = material;
		Graphics::isRenderingBatchDirty = true;
	}
}

void MeshRenderer::OnDisabled()
{
	Graphics::isRenderingBatchDirty = true;
}

void MeshRenderer::OnEnabled()
{
	Graphics::isRenderingBatchDirty = true;
}

void MeshRenderer::DrawCommand(const RenderCommand& renderCommand)
{
	if (m_culled || outOfFrustum)
		return;

	RenderingSettings renderSettings = RenderingSettings();
	renderSettings.invertFaces = false;
	renderSettings.useDepth = true;
	renderSettings.useTexture = true;
	renderSettings.useLighting = renderCommand.material->GetUseLighting();
	renderSettings.useBlend = renderCommand.material->GetUseTransparency();
	MeshManager::DrawMesh(*GetTransform(), *renderCommand.subMesh, *renderCommand.material, renderSettings);
}

void MeshRenderer::OnTransformPositionUpdated()
{
	boundingSphere = GetBoundingSphere();
}
