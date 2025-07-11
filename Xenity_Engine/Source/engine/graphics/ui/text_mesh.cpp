// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "text_mesh.h"

#include <engine/graphics/graphics.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>
#include <engine/debug/stack_debug_object.h>
#include <engine/graphics/material.h>
#include <engine/graphics/ui/font.h>
#include "text_manager.h"

#pragma region Constructors / Destructor

TextMesh::TextMesh()
{
}

ReflectiveData TextMesh::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, m_text, "text");
	Reflective::AddVariable(reflectedVariables, m_font, "font");
	Reflective::AddVariable(reflectedVariables, m_color, "color");
	Reflective::AddVariable(reflectedVariables, m_horizontalAlignment, "horizontalAlignment");
	Reflective::AddVariable(reflectedVariables, m_verticalAlignment, "verticalAlignment");
	Reflective::AddVariable(reflectedVariables, m_fontSize, "fontSize");
	return reflectedVariables;
}

void TextMesh::OnReflectionUpdated()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	m_isTextInfoDirty = true;
	Graphics::s_isRenderingBatchDirty = true;
}

#pragma endregion

void TextMesh::SetText(const std::string& text)
{
	if (m_text != text)
	{
		m_text = text;
		m_isTextInfoDirty = true;
	}
}

void TextMesh::SetFont(const std::shared_ptr<Font>& font)
{
	if (m_font != font)
	{
		m_font = font;
		m_isTextInfoDirty = true;
	}
}

void TextMesh::OnDisabled()
{
	Graphics::s_isRenderingBatchDirty = true;
}

void TextMesh::OnEnabled()
{
	Graphics::s_isRenderingBatchDirty = true;
}

void TextMesh::CreateRenderCommands(RenderBatch& renderBatch)
{
	if (!m_font)
		return;

	RenderCommand command = RenderCommand();
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
	XASSERT(m_font, "[TextMesh::DrawCommand] Font is nullptr");

	if (m_isTextInfoDirty)
	{
		if (m_mesh)
		{
			delete m_textInfo;
			m_mesh.reset();
		}
		if (!m_text.empty())
		{
			m_textInfo = TextManager::GetTextInfomations(m_text, (int)m_text.size(), m_font, 1);
			m_mesh = TextManager::CreateMesh(m_text, m_textInfo, m_horizontalAlignment, m_verticalAlignment, m_color, m_font, m_fontSize);
		}
		m_isTextInfoDirty = false;
	}
	if (m_mesh)
	{
		TextManager::DrawText(m_text, m_textInfo, m_horizontalAlignment, m_verticalAlignment, *GetTransformRaw(), m_color, false, *m_mesh, *m_font, *AssetManager::unlitMaterial);
	}
}

void TextMesh::SetFontSize(float fontSize)
{
	m_fontSize = fontSize;
	m_isTextInfoDirty = true;
}

void TextMesh::SetLineSpacing(float lineSpacing)
{
	m_lineSpacing = lineSpacing;
	m_isTextInfoDirty = true;
}

void TextMesh::SetCharacterSpacing(float characterSpacing)
{
	m_characterSpacing = characterSpacing;
	m_isTextInfoDirty = true;
}

void TextMesh::SetVerticalAlignment(VerticalAlignment verticalAlignment)
{
	m_verticalAlignment = verticalAlignment;
	m_isTextInfoDirty = true;
}

void TextMesh::SetHorizontalAlignment(HorizontalAlignment horizontalAlignment)
{
	m_horizontalAlignment = horizontalAlignment;
	m_isTextInfoDirty = true;
}
