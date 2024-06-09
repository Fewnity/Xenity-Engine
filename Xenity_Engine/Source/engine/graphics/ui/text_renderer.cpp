// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "text_renderer.h"

#include "text_manager.h"

#include <engine/graphics/graphics.h>
#include <engine/graphics/3d_graphics/mesh_data.h>

#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>


#pragma region Constructors / Destructor

TextRenderer::TextRenderer()
{
	componentName = "TextRenderer";

	AssetManager::AddReflection(this);
	material = AssetManager::standardMaterial;
}

ReflectiveData TextRenderer::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, text, "text", true);
	Reflective::AddVariable(reflectedVariables, font, "font", true);
	Reflective::AddVariable(reflectedVariables, material, "material", true);
	Reflective::AddVariable(reflectedVariables, horizontalAlignment, "horizontalAlignment", true);
	Reflective::AddVariable(reflectedVariables, verticalAlignment, "verticalAlignment", true);
	Reflective::AddVariable(reflectedVariables, fontSize, "fontSize", true);
	return reflectedVariables;
}

void TextRenderer::OnReflectionUpdated()
{
	isTextInfoDirty = true;
	Graphics::isRenderingBatchDirty = true;
}

TextRenderer::~TextRenderer()
{
	AssetManager::RemoveReflection(this);
}

void TextRenderer::SetOrderInLayer(int orderInLayer)
{
	this->orderInLayer = orderInLayer;
	Graphics::SetDrawOrderListAsDirty();
}

#pragma endregion

void TextRenderer::SetText(const std::string& text)
{
	if (this->text != text)
	{
		this->text = text;
		isTextInfoDirty = true;
	}
}

void TextRenderer::SetFont(const std::shared_ptr<Font>& font)
{
	if (this->font != font)
	{
		this->font = font;
		isTextInfoDirty = true;
	}
}

std::shared_ptr<Material> TextRenderer::GetMaterial()
{
	return material;
}

void TextRenderer::SetMaterial(std::shared_ptr<Material> _material)
{
	material = _material;
	Graphics::isRenderingBatchDirty = true;
}

void TextRenderer::OnDisabled()
{
	Graphics::isRenderingBatchDirty = true;
}

void TextRenderer::OnEnabled()
{
	Graphics::isRenderingBatchDirty = true;
}

void TextRenderer::CreateRenderCommands(RenderBatch& renderBatch)
{
	if (!material || !font)
		return;

	RenderCommand command = RenderCommand();
	command.material = material;
	command.drawable = this;
	command.subMesh = nullptr;
	command.transform = GetTransform();
	command.isEnabled = GetIsEnabled() && GetGameObject()->GetLocalActive();

	renderBatch.transparentMeshCommands.push_back(command);
	renderBatch.transparentMeshCommandIndex++;
}

/// <summary>
/// Draw text
/// </summary>
void TextRenderer::DrawCommand(const RenderCommand& renderCommand)
{
	if (isTextInfoDirty)
	{
		delete textInfo;
		if (mesh)
			mesh.reset();
		textInfo = TextManager::GetTextInfomations(text, (int)text.size(), font, 1);
		mesh = TextManager::CreateMesh(text, textInfo, horizontalAlignment, verticalAlignment, color, font, fontSize);
		isTextInfoDirty = false;
	}
	TextManager::DrawText(text, textInfo, horizontalAlignment, verticalAlignment, GetTransform(), color, false, mesh, font, material);
}

void TextRenderer::SetFontSize(float fontSize)
{
	this->fontSize = fontSize;
	isTextInfoDirty = true;
}

void TextRenderer::SetLineSpacing(float lineSpacing)
{
	this->lineSpacing = lineSpacing;
	isTextInfoDirty = true;
}

void TextRenderer::SetCharacterSpacing(float characterSpacing)
{
	this->characterSpacing = characterSpacing;
	isTextInfoDirty = true;

}

void TextRenderer::SetVerticalAlignment(VerticalAlignment verticalAlignment)
{
	this->verticalAlignment = verticalAlignment;
	isTextInfoDirty = true;
}

void TextRenderer::SetHorizontalAlignment(HorizontalAlignment horizontalAlignment)
{
	this->horizontalAlignment = horizontalAlignment;
	isTextInfoDirty = true;
}
