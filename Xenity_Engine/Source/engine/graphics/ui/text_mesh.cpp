// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "text_mesh.h"

#include <engine/graphics/graphics.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>
#include "text_manager.h"

#pragma region Constructors / Destructor

TextMesh::TextMesh()
{
	m_componentName = "TextMesh";

	AssetManager::AddReflection(this);
	m_material = AssetManager::standardMaterial;
}

ReflectiveData TextMesh::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, m_text, "text", true);
	Reflective::AddVariable(reflectedVariables, m_font, "font", true);
	Reflective::AddVariable(reflectedVariables, m_material, "material", true);
	Reflective::AddVariable(reflectedVariables, m_horizontalAlignment, "horizontalAlignment", true);
	Reflective::AddVariable(reflectedVariables, m_verticalAlignment, "verticalAlignment", true);
	Reflective::AddVariable(reflectedVariables, m_fontSize, "fontSize", true);
	return reflectedVariables;
}

void TextMesh::OnReflectionUpdated()
{
	m_isTextInfoDirty = true;
	Graphics::isRenderingBatchDirty = true;
}

TextMesh::~TextMesh()
{
	AssetManager::RemoveReflection(this);
}

void TextMesh::SetOrderInLayer(int orderInLayer)
{
	this->m_orderInLayer = orderInLayer;
	Graphics::SetDrawOrderListAsDirty();
}

#pragma endregion

void TextMesh::SetText(const std::string& text)
{
	if (this->m_text != text)
	{
		this->m_text = text;
		m_isTextInfoDirty = true;
	}
}

void TextMesh::SetFont(const std::shared_ptr<Font>& font)
{
	if (this->m_font != font)
	{
		this->m_font = font;
		m_isTextInfoDirty = true;
	}
}

void TextMesh::SetMaterial(std::shared_ptr<Material> _material)
{
	m_material = _material;
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
	if (!m_material || !m_font)
		return;

	RenderCommand command = RenderCommand();
	command.material = m_material.get();
	command.drawable = this;
	command.transform = GetTransformRaw();
	command.isEnabled = IsEnabled() && GetGameObject()->IsLocalActive();

	renderBatch.transparentMeshCommands.push_back(command);
	renderBatch.transparentMeshCommandIndex++;
}

/// <summary>
/// Draw text
/// </summary>
void TextMesh::DrawCommand(const RenderCommand& renderCommand)
{
	if (m_isTextInfoDirty)
	{
		delete m_textInfo;
		if (m_mesh)
			m_mesh.reset();
		m_textInfo = TextManager::GetTextInfomations(m_text, (int)m_text.size(), m_font, 1);
		m_mesh = TextManager::CreateMesh(m_text, m_textInfo, m_horizontalAlignment, m_verticalAlignment, m_color, m_font, m_fontSize);
		m_isTextInfoDirty = false;
	}
	TextManager::DrawText(m_text, m_textInfo, m_horizontalAlignment, m_verticalAlignment, *GetTransformRaw(), m_color, false, *m_mesh, *m_font, *m_material);
}

void TextMesh::SetFontSize(float fontSize)
{
	this->m_fontSize = fontSize;
	m_isTextInfoDirty = true;
}

void TextMesh::SetLineSpacing(float lineSpacing)
{
	this->m_lineSpacing = lineSpacing;
	m_isTextInfoDirty = true;
}

void TextMesh::SetCharacterSpacing(float characterSpacing)
{
	this->m_characterSpacing = characterSpacing;
	m_isTextInfoDirty = true;

}

void TextMesh::SetVerticalAlignment(VerticalAlignment verticalAlignment)
{
	this->m_verticalAlignment = verticalAlignment;
	m_isTextInfoDirty = true;
}

void TextMesh::SetHorizontalAlignment(HorizontalAlignment horizontalAlignment)
{
	this->m_horizontalAlignment = horizontalAlignment;
	m_isTextInfoDirty = true;
}
