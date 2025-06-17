// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "sprite_manager.h"

#include <glm/ext/matrix_transform.hpp>
#if defined(__PSP__)
#include <pspkernel.h>
#endif

#include <engine/graphics/graphics.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/graphics/renderer/renderer.h>
#include <engine/graphics/material.h>
#include <engine/graphics/texture/texture.h>
#include <engine/engine.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/transform.h>
#include <engine/graphics/camera.h>
#include <engine/debug/debug.h>
#include <engine/tools/profiler_benchmark.h>
#include <engine/math/quaternion.h>
#include <engine/tools/internal_math.h>

std::shared_ptr <MeshData> SpriteManager::s_spriteMeshData = nullptr;
std::shared_ptr <MeshData> SpriteManager::s_spriteMeshDataWithNormals = nullptr;

/**
 * @brief Init the Sprite Manager
 *
 */
void SpriteManager::Init()
{
	// Create sprite mesh
	s_spriteMeshData = MeshData::MakeMeshDataForFile();
	VertexDescriptor spriteDescriptor;
	spriteDescriptor.AddVertexElement(VertexElement::UV_32_BITS);
	spriteDescriptor.AddVertexElement(VertexElement::POSITION_32_BITS);
	s_spriteMeshData->CreateSubMesh(4, 6, spriteDescriptor);

	const std::unique_ptr<MeshData::SubMesh>& subMesh = s_spriteMeshData->m_subMeshes[0];
	subMesh->SetVertex(1.0f, 1.0f, -0.5f, -0.5f, 0.0f, 0);
	subMesh->SetVertex(0.0f, 1.0f, 0.5f, -0.5f, 0.0f, 1);
	subMesh->SetVertex(0.0f, 0.0f, 0.5f, 0.5f, 0.0f, 2);
	subMesh->SetVertex(1.0f, 0.0f, -0.5f, 0.5f, 0.0f, 3);

	subMesh->SetIndex(0, 0);
	subMesh->SetIndex(1, 2);
	subMesh->SetIndex(2, 1);
	subMesh->SetIndex(3, 2);
	subMesh->SetIndex(4, 0);
	subMesh->SetIndex(5, 3);

	s_spriteMeshData->OnLoadFileReferenceFinished();

	// Create sprite mesh with normals
	s_spriteMeshDataWithNormals = MeshData::MakeMeshDataForFile();
	VertexDescriptor withNormalsDescriptor;
	withNormalsDescriptor.AddVertexElement(VertexElement::UV_32_BITS);
	withNormalsDescriptor.AddVertexElement(VertexElement::NORMAL_32_BITS);
	withNormalsDescriptor.AddVertexElement(VertexElement::POSITION_32_BITS);
	s_spriteMeshDataWithNormals->CreateSubMesh(4, 6, withNormalsDescriptor);

	const std::unique_ptr<MeshData::SubMesh>& subMeshWithNormals = s_spriteMeshDataWithNormals->m_subMeshes[0];
	subMeshWithNormals->SetVertex(1.0f, 1.0f, 0, 0, -1, -0.5f, -0.5f, 0.0f, 0);
	subMeshWithNormals->SetVertex(0.0f, 1.0f, 0, 0, -1, 0.5f, -0.5f, 0.0f, 1);
	subMeshWithNormals->SetVertex(0.0f, 0.0f, 0, 0, -1, 0.5f, 0.5f, 0.0f, 2);
	subMeshWithNormals->SetVertex(1.0f, 0.0f, 0, 0, -1, -0.5f, 0.5f, 0.0f, 3);

	subMeshWithNormals->SetIndex(0, 0);
	subMeshWithNormals->SetIndex(1, 2);
	subMeshWithNormals->SetIndex(2, 1);
	subMeshWithNormals->SetIndex(3, 2);
	subMeshWithNormals->SetIndex(4, 0);
	subMeshWithNormals->SetIndex(5, 3);
	s_spriteMeshDataWithNormals->OnLoadFileReferenceFinished();

#if defined(__PSP__)
	sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif

	Debug::Print("-------- Sprite Manager initiated --------", true);
}

void SpriteManager::Close()
{
	s_spriteMeshData.reset();
	s_spriteMeshDataWithNormals.reset();
}

/**
 * @brief Draw a sprite
 *
 * @param position Sprite position (center)
 * @param rotation Sprite rotation
 * @param scale Sprite scale
 * @param texture Texture
 */
void SpriteManager::DrawSprite(Transform& transform, const Color& color, Material& material, Texture* texture)
{
	s_spriteMeshData->unifiedColor = color;

	const Vector3& scale = transform.GetScale();
	RenderingSettings renderSettings = RenderingSettings();

	if (scale.x * scale.y < 0)
		renderSettings.invertFaces = true;
	else
		renderSettings.invertFaces = false;

	renderSettings.renderingMode = MaterialRenderingMode::Transparent;
	renderSettings.useDepth = false;
	renderSettings.useTexture = true;
	renderSettings.useLighting = false;

	const float scaleCoef = (1.0f / texture->GetPixelPerUnit());
	const float w = texture->GetWidth() * scaleCoef;
	const float h = texture->GetHeight() * scaleCoef;

	const glm::mat4 matCopy = glm::scale(transform.GetTransformationMatrix(), glm::vec3(w, h, 1));
	glm::mat4 mvp;
	if constexpr (!s_UseOpenGLFixedFunctions)
	{
		mvp = Graphics::usedCamera->m_viewProjectionMatrix * matCopy;
	}

	Graphics::DrawSubMesh(*s_spriteMeshData->m_subMeshes[0], material, texture, renderSettings, matCopy, transform.GetInverseNormalMatrix(), mvp, false);
}

void SpriteManager::DrawSprite(const Vector3& position, const Quaternion& rotation, const Vector3& scale, const Color& color, Material& material, Texture* texture)
{
	s_spriteMeshData->unifiedColor = color;

	RenderingSettings renderSettings = RenderingSettings();

	if (scale.x * scale.y < 0)
		renderSettings.invertFaces = true;
	else
		renderSettings.invertFaces = false;

	renderSettings.renderingMode = MaterialRenderingMode::Transparent;
	renderSettings.useDepth = false;
	renderSettings.useTexture = true;
	renderSettings.useLighting = false;

	const float scaleCoef = (1.0f / texture->GetPixelPerUnit());
	const float w = texture->GetWidth() * scaleCoef;
	const float h = texture->GetHeight() * scaleCoef;

	const glm::mat4 matrix = glm::scale(InternalMath::CreateModelMatrix(position, rotation, scale), glm::vec3(w, h, 1));
	glm::mat4 mvp;
	if constexpr (!s_UseOpenGLFixedFunctions)
	{
		mvp = Graphics::usedCamera->m_viewProjectionMatrix * matrix;
	}

	Graphics::DrawSubMesh(*s_spriteMeshData->m_subMeshes[0], material, texture, renderSettings, matrix, matrix, mvp, false);
}

void SpriteManager::Render2DLine(const std::shared_ptr<MeshData>& meshData)
{
	XASSERT(meshData != nullptr, "[SpriteManager::Render2DLine] meshData is nullptr");

#if defined(__PSP__)
	if (Graphics::needUpdateCamera)
	{
		Graphics::usedCamera->UpdateProjection();
		Engine::GetRenderer().SetCameraPosition(*Graphics::usedCamera);
		Graphics::needUpdateCamera = false;
	}
#else
	Engine::GetRenderer().SetCameraPosition(*Graphics::usedCamera);
#endif

	const Vector3 zero = Vector3(0);
	const Vector3 one = Vector3(1);

	Engine::GetRenderer().SetTransform(zero, zero, one, true);

	// Set draw settings
	RenderingSettings renderSettings = RenderingSettings();

	renderSettings.invertFaces = false;
	renderSettings.renderingMode = MaterialRenderingMode::Transparent;
	renderSettings.useDepth = false;
	renderSettings.useTexture = true;
	renderSettings.useLighting = false;

	Engine::GetRenderer().DrawSubMesh(*meshData->m_subMeshes[0], *AssetManager::standardMaterial, *AssetManager::defaultTexture, renderSettings);
}