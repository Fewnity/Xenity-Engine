// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "billboard_renderer.h"

#include <engine/graphics/graphics.h>
#include "sprite_manager.h"
#include <engine/graphics/camera.h>

#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>

#pragma region Constructors / Destructor

BillboardRenderer::BillboardRenderer()
{
	componentName = "BillboardRenderer";
	AssetManager::AddReflection(this);
	//material = AssetManager::standardMaterial;
}

ReflectiveData BillboardRenderer::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, color, "color", true);
	Reflective::AddVariable(reflectedVariables, texture, "texture", true);
	Reflective::AddVariable(reflectedVariables, material, "material", true);
	return reflectedVariables;
}

void BillboardRenderer::OnReflectionUpdated()
{
	Graphics::isRenderingBatchDirty = true;
}

BillboardRenderer::~BillboardRenderer()
{
	AssetManager::RemoveReflection(this);
}

void BillboardRenderer::SetOrderInLayer(int orderInLayer)
{
	this->orderInLayer = orderInLayer;
	Graphics::SetDrawOrderListAsDirty();
}

#pragma endregion

void BillboardRenderer::CreateRenderCommands(RenderBatch& renderBatch)
{
	if (!material || !texture)
		return;

	RenderCommand command = RenderCommand();
	command.material = material.get();
	command.drawable = this;
	command.transform = GetTransform().get();
	command.isEnabled = IsEnabled() && GetGameObject()->IsLocalActive();

	renderBatch.spriteCommands.push_back(command);
	renderBatch.spriteCommandIndex++;
}

void BillboardRenderer::SetMaterial(const std::shared_ptr<Material>& material)
{
	this->material = material;
	Graphics::isRenderingBatchDirty = true;
}

void BillboardRenderer::SetTexture(const std::shared_ptr<Texture>& texture)
{
	this->texture = texture;
	Graphics::isRenderingBatchDirty = true;
}

void BillboardRenderer::OnDisabled()
{
	Graphics::isRenderingBatchDirty = true;
}

void BillboardRenderer::OnEnabled()
{
	Graphics::isRenderingBatchDirty = true;
}

/// <summary>
/// Draw sprite
/// </summary>
void BillboardRenderer::DrawCommand(const RenderCommand& renderCommand)
{
	const std::shared_ptr<Transform> transform = GetTransform();
	SpriteManager::DrawSprite(transform->GetPosition(), transform->GetEulerAngles() + Graphics::usedCamera->GetTransform()->GetEulerAngles(), transform->GetScale(), color, *material, texture);
}