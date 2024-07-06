// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "sprite_renderer.h"

#include <engine/graphics/graphics.h>
#include <engine/graphics/material.h>
#include "sprite_manager.h"

#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>


#pragma region Constructors / Destructor

SpriteRenderer::SpriteRenderer()
{
	componentName = "SpriteRenderer";
	AssetManager::AddReflection(this);
	//material = AssetManager::unlitMaterial;
}

ReflectiveData SpriteRenderer::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, color, "color", true);
	Reflective::AddVariable(reflectedVariables, texture, "texture", true);
	Reflective::AddVariable(reflectedVariables, material, "material", true);
	return reflectedVariables;
}

void SpriteRenderer::OnReflectionUpdated()
{
	Graphics::isRenderingBatchDirty = true;
}

SpriteRenderer::~SpriteRenderer()
{
	AssetManager::RemoveReflection(this);
}

void SpriteRenderer::SetOrderInLayer(int orderInLayer)
{
	this->orderInLayer = orderInLayer;
	Graphics::SetDrawOrderListAsDirty();
}

#pragma endregion

void SpriteRenderer::CreateRenderCommands(RenderBatch& renderBatch)
{
	if (!material || !texture)
		return;

	RenderCommand command = RenderCommand();
	command.material = material.get();
	command.drawable = this;
	command.subMesh = nullptr;
	command.transform = GetTransform().get();
	command.isEnabled = IsEnabled() && GetGameObject()->IsLocalActive();

	renderBatch.spriteCommands.push_back(command);
	renderBatch.spriteCommandIndex++;
}

void SpriteRenderer::SetMaterial(const std::shared_ptr<Material>& material)
{
	this->material = material;
	Graphics::isRenderingBatchDirty = true;
}

void SpriteRenderer::SetTexture(const std::shared_ptr<Texture>& texture)
{
	this->texture = texture;
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

void SpriteRenderer::DrawCommand(const RenderCommand& renderCommand)
{
	SpriteManager::DrawSprite(*GetTransform(), color, *material, texture);
}
