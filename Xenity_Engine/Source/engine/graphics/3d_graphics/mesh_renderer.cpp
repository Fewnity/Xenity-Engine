// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "mesh_renderer.h"

#if defined(EDITOR)
#include <editor/gizmo.h>
#endif

#include <engine/graphics/renderer/renderer.h>
#include <engine/graphics/3d_graphics/mesh_manager.h>
#include <engine/graphics/material.h>
#include <engine/game_elements/transform.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>
#include <engine/graphics/shader.h>
#include <engine/world_partitionner/world_partitionner.h>
#include <engine/engine.h>
#include <engine/debug/stack_debug_object.h>
#include <engine/debug/debug.h>

using namespace std;

// #pragma region MeshRenderer Constructors / Destructor

bool IsSphereInFrustum(const Frustum& frustum, const Sphere& sphere)
{
	for (const Plane& plane : frustum.planes)
	{
		// Distance between the center of the sphere and the plane
		const float distance = plane.A * sphere.position.x +
			plane.B * sphere.position.y +
			plane.C * sphere.position.z +
			plane.D;

		// If the distance is less than -radius, the sphere is completely out of the frustum
		if (distance < -sphere.radius)
		{
			return false;
		}
	}
	return true;
}

MeshRenderer::MeshRenderer()
{
	AssetManager::AddReflection(this);
}

void MeshRenderer::OnDrawGizmosSelected()
{
	return;

#if defined(EDITOR)
	Engine::GetRenderer().SetCameraPosition(*Graphics::usedCamera);
	Gizmo::DrawSphere(m_boundingSphere.position, m_boundingSphere.radius);

	const Color meshLineColor = Color::CreateFromRGBAFloat(0, 0, 1, 1);

	Gizmo::SetColor(meshLineColor);

	const Vector3& tPos = GetTransformRaw()->GetPosition();
	for (auto& chunk : m_worldChunkPositions)
	{
		Gizmo::DrawLine(tPos, chunk + Vector3(WORLD_CHUNK_HALF_SIZE));
	}

	const Color lightLineColor = Color::CreateFromRGBAFloat(1, 0, 0, 1);

	Gizmo::SetColor(lightLineColor);

	for (auto& light : m_affectedByLights)
	{
		Gizmo::DrawLine(tPos, light->GetTransformRaw()->GetPosition());
	}
#endif
}

Sphere MeshRenderer::ProcessBoundingSphere() const
{
	Sphere sphere;
	if (!m_meshData)
		return sphere;

	sphere = m_meshData->GetBoundingSphere();
	const glm::vec3 transformedPosition = glm::vec3(GetTransformRaw()->GetTransformationMatrix() * glm::vec4(sphere.position.x, sphere.position.y, sphere.position.z, 1.0f));
	sphere.position = Vector3(-transformedPosition.x, transformedPosition.y, transformedPosition.z);

	const Vector3& scale = GetTransformRaw()->GetScale();
	sphere.radius *= std::max({ std::abs(scale.x), std::abs(scale.y), std::abs(scale.z) });
	return sphere;
}

void MeshRenderer::OnNewRender()
{
	if (Graphics::usedCamera && GetGameObjectRaw()->IsLocalActive() && IsEnabled())
	{
		m_outOfFrustum = !IsSphereInFrustum(Graphics::usedCamera->frustum, m_boundingSphere);
	}
}

void MeshRenderer::OnComponentAttached()
{
	GetTransformRaw()->GetOnTransformUpdated().Bind(&MeshRenderer::OnTransformPositionUpdated, this);
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
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	if (m_meshData)
	{
		m_materials.resize(m_meshData->m_subMeshCount);
	}

	m_matCount = m_materials.size();
	Graphics::s_isRenderingBatchDirty = true;

	m_boundingSphere = ProcessBoundingSphere();
	WorldPartitionner::ProcessMeshRenderer(this);
}

/// <summary>
/// Destructor
/// </summary>
MeshRenderer::~MeshRenderer()
{
	GetTransformRaw()->GetOnTransformUpdated().Unbind(&MeshRenderer::OnTransformPositionUpdated, this);
	AssetManager::RemoveReflection(this);
	WorldPartitionner::RemoveMeshRenderer(this);
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
		command.subMesh = m_meshData->m_subMeshes[i].get();
		command.transform = GetTransformRaw();
		command.isEnabled = IsEnabled() && GetGameObjectRaw()->IsLocalActive();
		if (material->GetRenderingMode() == MaterialRenderingModes::Opaque || material->GetRenderingMode() == MaterialRenderingModes::Cutout)
		{
#if defined(ENABLE_OVERDRAW_OPTIMIZATION)
			renderBatch.opaqueMeshCommands.push_back(command);
			renderBatch.opaqueMeshCommandIndex++;
#else
			RenderQueue& renderQueue = renderBatch.renderQueues[material->GetFileId()];
			renderQueue.commands.push_back(command);
			renderQueue.commandIndex++;
#endif
		}
		else
		{
			renderBatch.transparentMeshCommands.push_back(command);
			renderBatch.transparentMeshCommandIndex++;
		}
	}
}

void MeshRenderer::SetMeshData(const std::shared_ptr<MeshData>& meshData)
{
	m_meshData = meshData;
	if (meshData)
	{
		m_materials.resize(meshData->m_subMeshCount);
		m_matCount = meshData->m_subMeshCount;
	}
	else
	{

	}
	Graphics::s_isRenderingBatchDirty = true;
}

void MeshRenderer::SetMaterial(const std::shared_ptr<Material>& material, int index)
{
	XASSERT(index < m_materials.size(), "[MeshRenderer::SetMaterial] Index is out of bounds");
	if (index < m_materials.size())
	{
		m_materials[index] = material;
		Graphics::s_isRenderingBatchDirty = true;
	}
}

void MeshRenderer::OnDisabled()
{
	Graphics::s_isRenderingBatchDirty = true;
}

void MeshRenderer::OnEnabled()
{
	Graphics::s_isRenderingBatchDirty = true;
}

void MeshRenderer::DrawCommand(const RenderCommand& renderCommand)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);
	if (m_culled || m_outOfFrustum)
		return;

	if (renderCommand.material->GetShader() == nullptr)
	{
		return;
	}

	if constexpr (!s_UseOpenGLFixedFunctions)
	{
		if (renderCommand.material->GetShader()->GetFileStatus() != FileStatus::FileStatus_Loaded)
		{
			return;
		}
	}

	if (renderCommand.material->GetUseLighting())
	{
		const size_t lightCount = m_affectedByLights.size();

		const size_t directionalLightCount = Graphics::s_directionalLights.size();
#if defined(ENABLE_SHADER_VARIANT_OPTIMIZATION)
		if (lightCount == 0)
		{
			if (renderCommand.material->GetShader() != AssetManager::standardShaderNoPointLight)
			{
				renderCommand.material->SetShader(AssetManager::standardShaderNoPointLight);
				Graphics::s_currentMaterial = nullptr;
			}
		}
		else 
		{
			if (renderCommand.material->GetShader() != AssetManager::standardShader)
			{
				renderCommand.material->SetShader(AssetManager::standardShader);
				Graphics::s_currentMaterial = nullptr;
			}
		}
#endif
		const std::shared_ptr<Shader>& shader = renderCommand.material->GetShader();
		// Check if the lights have changed
		bool needLightUpdate = Graphics::s_isLightUpdateNeeded;
		if (!needLightUpdate)
		{
			if (shader->m_currentLights.size() != lightCount || shader->m_currentDirectionalLights.size() != directionalLightCount)
			{
				needLightUpdate = true;
			}
			else
			{
				for (size_t i = 0; i < lightCount; i++)
				{
					bool found = false;
					for (size_t j = 0; j < lightCount; j++)
					{
						if (m_affectedByLights[i] == shader->m_currentLights[j])
						{
							found = true;
							break;
						}
					}
					if (!found)
					{
						needLightUpdate = true;
						break;
					}
				}
			}
		}

		// Update light buffer
		if (needLightUpdate)
		{
			Graphics::s_isLightUpdateNeeded = false;
			int pointLightCount = 0;
			int spotLightCount = 0;

			LightsIndices lightsIndices;
			lightsIndices.usedDirectionalLightCount = static_cast<int>(directionalLightCount);
			shader->m_currentLights = m_affectedByLights;
			shader->m_currentDirectionalLights = Graphics::s_directionalLights;

			for (size_t i = 0; i < lightCount; i++)
			{
				const Light* light = m_affectedByLights[i];
				if (light->GetType() == LightType::Point)
				{
					if constexpr (s_UseOpenGLFixedFunctions)
						lightsIndices.pointLightIndices[pointLightCount].x = light->m_indexInLightList + 1;
					else
						lightsIndices.pointLightIndices[pointLightCount].x = light->m_indexInShaderList + 1;

					pointLightCount++;
				}
				else if (light->GetType() == LightType::Spot)
				{
					if constexpr (s_UseOpenGLFixedFunctions)
						lightsIndices.spotLightIndices[spotLightCount].x = light->m_indexInLightList + 1;
					else
						lightsIndices.spotLightIndices[spotLightCount].x = light->m_indexInShaderList + 1;

					spotLightCount++;
				}
			}

			for (size_t i = 0; i < directionalLightCount; i++)
			{
				if constexpr (s_UseOpenGLFixedFunctions)
					lightsIndices.directionalLightIndices[i].x = Graphics::s_directionalLights[i]->m_indexInLightList + 1;
				else
					lightsIndices.directionalLightIndices[i].x = Graphics::s_directionalLights[i]->m_indexInShaderList + 1;
			}

			lightsIndices.usedPointLightCount = pointLightCount;
			lightsIndices.usedSpotLightCount = spotLightCount;
			if constexpr (s_UseOpenGLFixedFunctions)
			{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
				Engine::GetRenderer().SetCameraPosition(*Graphics::usedCamera);
#endif
				Engine::GetRenderer().Setlights(lightsIndices);
			}
			else
			{
				shader->SetLightIndices(lightsIndices);
			}
		}
	}

	RenderingSettings renderSettings = RenderingSettings();
	renderSettings.invertFaces = false;
	renderSettings.useDepth = true;
	renderSettings.useTexture = true;
	renderSettings.useLighting = renderCommand.material->GetUseLighting();
	renderSettings.renderingMode = renderCommand.material->GetRenderingMode();
	MeshManager::DrawMesh(*GetTransformRaw(), *renderCommand.subMesh, *renderCommand.material, renderSettings);
}

void MeshRenderer::OnTransformPositionUpdated()
{
	m_boundingSphere = ProcessBoundingSphere();
	WorldPartitionner::ProcessMeshRenderer(this);
}
