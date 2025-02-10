// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "billboard_renderer.h"

#include <engine/graphics/graphics.h>
#include <engine/graphics/material.h>
#include <engine/graphics/texture/texture.h>
#include <engine/graphics/camera.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/debug/stack_debug_object.h>
#include "sprite_manager.h"

#pragma region Constructors / Destructor

BillboardRenderer::BillboardRenderer()
{
	AssetManager::AddReflection(this);
	//material = AssetManager::standardMaterial;
}

ReflectiveData BillboardRenderer::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, m_color, "color", true);
	Reflective::AddVariable(reflectedVariables, m_texture, "texture", true);
	Reflective::AddVariable(reflectedVariables, m_material, "material", true);
	return reflectedVariables;
}

void BillboardRenderer::OnReflectionUpdated()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	Graphics::s_isRenderingBatchDirty = true;
}

BillboardRenderer::~BillboardRenderer()
{
	AssetManager::RemoveReflection(this);
}

void BillboardRenderer::SetOrderInLayer(int orderInLayer)
{
	m_orderInLayer = orderInLayer;
	Graphics::SetDrawOrderListAsDirty();
}

#pragma endregion

void BillboardRenderer::CreateRenderCommands(RenderBatch& renderBatch)
{
	if (!m_material || !m_texture)
		return;

	RenderCommand command = RenderCommand();
	command.material = m_material.get();
	command.drawable = this;
	command.transform = GetTransformRaw();
	command.isEnabled = IsEnabled() && GetGameObjectRaw()->IsLocalActive();

	renderBatch.spriteCommands.push_back(command);
	renderBatch.spriteCommandIndex++;
}

void BillboardRenderer::SetMaterial(const std::shared_ptr<Material>& material)
{
	m_material = material;
	Graphics::s_isRenderingBatchDirty = true;
}

void BillboardRenderer::SetTexture(const std::shared_ptr<Texture>& texture)
{
	m_texture = texture;
	Graphics::s_isRenderingBatchDirty = true;
}

void BillboardRenderer::OnDisabled()
{
	Graphics::s_isRenderingBatchDirty = true;
}

void BillboardRenderer::OnEnabled()
{
	Graphics::s_isRenderingBatchDirty = true;
}

/// <summary>
/// Draw sprite
/// </summary>
void BillboardRenderer::DrawCommand([[maybe_unused]] const RenderCommand& renderCommand)
{
	const Transform* transform = GetTransformRaw();
	SpriteManager::DrawSprite(transform->GetPosition(), Graphics::usedCamera->GetTransformRaw()->GetRotation() * transform->GetRotation(), transform->GetScale(), m_color, *m_material, m_texture.get());
}