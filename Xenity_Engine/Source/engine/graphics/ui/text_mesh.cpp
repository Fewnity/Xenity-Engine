// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "text_mesh.h"

#include "text_manager.h"

#include <engine/graphics/graphics.h>
#include <engine/graphics/3d_graphics/mesh_data.h>

#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>


#pragma region Constructors / Destructor

TextMesh::TextMesh()
{
	componentName = "TextMesh";

	AssetManager::AddReflection(this);
	material = AssetManager::standardMaterial;
}

ReflectiveData TextMesh::GetReflectiveData()
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

void TextMesh::OnReflectionUpdated()
{
	isTextInfoDirty = true;
	Graphics::isRenderingBatchDirty = true;
}

TextMesh::~TextMesh()
{
	AssetManager::RemoveReflection(this);
}

void TextMesh::SetOrderInLayer(int orderInLayer)
{
	this->orderInLayer = orderInLayer;
	Graphics::SetDrawOrderListAsDirty();
}

#pragma endregion

void TextMesh::SetText(const std::string& text)
{
	if (this->text != text)
	{
		this->text = text;
		isTextInfoDirty = true;
	}
}

void TextMesh::SetFont(const std::shared_ptr<Font>& font)
{
	if (this->font != font)
	{
		this->font = font;
		isTextInfoDirty = true;
	}
}

std::shared_ptr<Material> TextMesh::GetMaterial()
{
	return material;
}

void TextMesh::SetMaterial(std::shared_ptr<Material> _material)
{
	material = _material;
	Graphics::isRenderingBatchDirty = true;
}

void TextMesh::OnDisabled()
{
	Graphics::isRenderingBatchDirty = true;
}

void TextMesh::OnEnabled()
{
	Graphics::isRenderingBatchDirty = true;
}

void TextMesh::CreateRenderCommands(RenderBatch& renderBatch)
{
	if (!material || !font)
		return;

	RenderCommand command = RenderCommand();
	command.material = material;
	command.drawable = this;
	command.subMesh = nullptr;
	command.transform = GetTransform();
	command.isEnabled = IsEnabled() && GetGameObject()->IsLocalActive();

	renderBatch.transparentMeshCommands.push_back(command);
	renderBatch.transparentMeshCommandIndex++;
}

/// <summary>
/// Draw text
/// </summary>
void TextMesh::DrawCommand(const RenderCommand& renderCommand)
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

void TextMesh::SetFontSize(float fontSize)
{
	this->fontSize = fontSize;
	isTextInfoDirty = true;
}

void TextMesh::SetLineSpacing(float lineSpacing)
{
	this->lineSpacing = lineSpacing;
	isTextInfoDirty = true;
}

void TextMesh::SetCharacterSpacing(float characterSpacing)
{
	this->characterSpacing = characterSpacing;
	isTextInfoDirty = true;

}

void TextMesh::SetVerticalAlignment(VerticalAlignment verticalAlignment)
{
	this->verticalAlignment = verticalAlignment;
	isTextInfoDirty = true;
}

void TextMesh::SetHorizontalAlignment(HorizontalAlignment horizontalAlignment)
{
	this->horizontalAlignment = horizontalAlignment;
	isTextInfoDirty = true;
}
