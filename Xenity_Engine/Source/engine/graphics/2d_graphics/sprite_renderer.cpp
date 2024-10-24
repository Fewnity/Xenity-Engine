// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "sprite_renderer.h"

#include <engine/graphics/graphics.h>
#include <engine/graphics/material.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>
#include <engine/debug/stack_debug_object.h>

#include "sprite_manager.h"

#pragma region Constructors / Destructor

SpriteRenderer::SpriteRenderer()
{
	m_componentName = "SpriteRenderer";
	AssetManager::AddReflection(this);
}

ReflectiveData SpriteRenderer::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, m_color, "color", true);
	Reflective::AddVariable(reflectedVariables, m_texture, "texture", true);
	Reflective::AddVariable(reflectedVariables, m_material, "material", true);
	return reflectedVariables;
}

void SpriteRenderer::OnReflectionUpdated()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	Graphics::isRenderingBatchDirty = true;
}

SpriteRenderer::~SpriteRenderer()
{
	AssetManager::RemoveReflection(this);
}

void SpriteRenderer::SetOrderInLayer(int orderInLayer)
{
	m_orderInLayer = orderInLayer;
	Graphics::SetDrawOrderListAsDirty();
}

#pragma endregion

void SpriteRenderer::CreateRenderCommands(RenderBatch& renderBatch)
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

void SpriteRenderer::SetMaterial(const std::shared_ptr<Material>& material)
{
	m_material = material;
	Graphics::isRenderingBatchDirty = true;
}

void SpriteRenderer::SetTexture(const std::shared_ptr<Texture>& texture)
{
	m_texture = texture;
	Graphics::isRenderingBatchDirty = true;
}

void SpriteRenderer::OnDisabled()
{
	Graphics::isRenderingBatchDirty = true;
}

void SpriteRenderer::OnEnabled()
{
	Graphics::isRenderingBatchDirty = true;
}

void SpriteRenderer::DrawCommand([[maybe_unused]] const RenderCommand& renderCommand)
{
	SpriteManager::DrawSprite(*GetTransformRaw(), m_color, *m_material, m_texture.get());
}
