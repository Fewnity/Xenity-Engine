// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
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
#include <engine/graphics/texture.h>
#include <engine/engine.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/transform.h>
#include <engine/graphics/camera.h>
#include <engine/debug/debug.h>
#include <engine/tools/profiler_benchmark.h>
#include <engine/vectors/quaternion.h>
#include <engine/tools/math.h>

std::shared_ptr <MeshData> SpriteManager::s_spriteMeshData = nullptr;

std::shared_ptr<ProfilerBenchmark> spriteBenchmark = nullptr;

/**
 * @brief Init the Sprite Manager
 *
 */
void SpriteManager::Init()
{
	spriteBenchmark = std::make_shared<ProfilerBenchmark>("Sprite", "Sprite");

	// Create sprite mesh
	s_spriteMeshData = MeshData::MakeMeshData(4, 6, false, false, true);
	s_spriteMeshData->AddVertex(1.0f, 1.0f, -0.5f, -0.5f, 0.0f, 0, 0);
	s_spriteMeshData->AddVertex(0.0f, 1.0f, 0.5f, -0.5f, 0.0f, 1, 0);
	s_spriteMeshData->AddVertex(0.0f, 0.0f, 0.5f, 0.5f, 0.0f, 2, 0);
	s_spriteMeshData->AddVertex(1.0f, 0.0f, -0.5f, 0.5f, 0.0f, 3, 0);
	s_spriteMeshData->m_hasIndices = true;

	std::unique_ptr<MeshData::SubMesh>& subMesh = s_spriteMeshData->m_subMeshes[0];
	subMesh->indices[0] = 0;
	subMesh->indices[1] = 2;
	subMesh->indices[2] = 1;
	subMesh->indices[3] = 2;
	subMesh->indices[4] = 0;
	subMesh->indices[5] = 3;

	s_spriteMeshData->OnLoadFileReferenceFinished();

#if defined(__PSP__)
	sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif

	Debug::Print("-------- Sprite Manager initiated --------", true);
}

/**
 * @brief Draw a sprite
 *
 * @param position Sprite position (center)
 * @param rotation Sprite rotation
 * @param scale Sprite scale
 * @param texture Texture
 */
void SpriteManager::DrawSprite(const Transform& transform, const Color& color, Material& material, Texture* texture)
{
	spriteBenchmark->Start();

	s_spriteMeshData->unifiedColor = color;

	const Vector3& scale = transform.GetScale();
	RenderingSettings renderSettings = RenderingSettings();

	if (scale.x * scale.y < 0)
		renderSettings.invertFaces = true;
	else
		renderSettings.invertFaces = false;

	renderSettings.renderingMode = MaterialRenderingModes::Transparent;
	renderSettings.useDepth = false;
	renderSettings.useTexture = true;
	renderSettings.useLighting = false;

	const float scaleCoef = (1.0f / texture->GetPixelPerUnit());
	const float w = texture->GetWidth() * scaleCoef;
	const float h = texture->GetHeight() * scaleCoef;

	const glm::mat4 matCopy = glm::scale(transform.GetTransformationMatrix(), glm::vec3(w, h, 1));

	Graphics::DrawSubMesh(*s_spriteMeshData->m_subMeshes[0], material, texture, renderSettings, matCopy, false);

	spriteBenchmark->Stop();
}

void SpriteManager::DrawSprite(const Vector3& position, const Quaternion& rotation, const Vector3& scale, const Color& color, Material& material, Texture* texture)
{
	spriteBenchmark->Start();

	s_spriteMeshData->unifiedColor = color;

	RenderingSettings renderSettings = RenderingSettings();

	if (scale.x * scale.y < 0)
		renderSettings.invertFaces = true;
	else
		renderSettings.invertFaces = false;

	renderSettings.renderingMode = MaterialRenderingModes::Transparent;
	renderSettings.useDepth = false;
	renderSettings.useTexture = true;
	renderSettings.useLighting = false;

	const float scaleCoef = (1.0f / texture->GetPixelPerUnit());
	const float w = texture->GetWidth() * scaleCoef;
	const float h = texture->GetHeight() * scaleCoef;

	const glm::mat4 matrix = glm::scale(Math::CreateModelMatrix(position, rotation, scale), glm::vec3(w, h, 1));

	Graphics::DrawSubMesh(*s_spriteMeshData->m_subMeshes[0], material, texture, renderSettings, matrix, false);
	spriteBenchmark->Stop();
}

void SpriteManager::Render2DLine(const std::shared_ptr<MeshData>& meshData)
{
	XASSERT(meshData != nullptr, "[SpriteManager::Render2DLine] meshData is nullptr");

	spriteBenchmark->Start();

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
	renderSettings.renderingMode = MaterialRenderingModes::Transparent;
	renderSettings.useDepth = false;
	renderSettings.useTexture = true;
	renderSettings.useLighting = false;

	Engine::GetRenderer().DrawSubMesh(*meshData->m_subMeshes[0], *AssetManager::standardMaterial, *AssetManager::defaultTexture, renderSettings);

	spriteBenchmark->Stop();
}