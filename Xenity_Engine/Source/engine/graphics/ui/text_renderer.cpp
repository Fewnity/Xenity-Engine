// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "text_renderer.h"

#include <engine/graphics/graphics.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>
#include <engine/debug/stack_debug_object.h>
#include <engine/graphics/material.h>
#include <engine/graphics/ui/font.h>

#include "text_manager.h"

#pragma region Constructors / Destructor

TextRenderer::TextRenderer()
{
	AssetManager::AddReflection(this);
	m_material = AssetManager::unlitMaterial;
}

ReflectiveData TextRenderer::GetReflectiveData()
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

void TextRenderer::OnReflectionUpdated()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	m_isTextInfoDirty = true;
	Graphics::s_isRenderingBatchDirty = true;
}

TextRenderer::~TextRenderer()
{
	AssetManager::RemoveReflection(this);
}

#pragma endregion

//void TextRenderer::SetOrderInLayer(int orderInLayer)
//{
//	m_orderInLayer = orderInLayer;
//	Graphics::SetDrawOrderListAsDirty();
//}

void TextRenderer::SetText(const std::string& text)
{
	if (m_text != text)
	{
		m_text = text;
		m_isTextInfoDirty = true;
	}
}

void TextRenderer::SetFont(const std::shared_ptr<Font>& font)
{
	if (m_font != font)
	{
		m_font = font;
		m_isTextInfoDirty = true;
		Graphics::s_isRenderingBatchDirty = true;
	}
}

void TextRenderer::SetMaterial(std::shared_ptr<Material> material)
{
	m_material = material;
	Graphics::s_isRenderingBatchDirty = true;
}

void TextRenderer::OnDisabled()
{
	Graphics::s_isRenderingBatchDirty = true;
}

void TextRenderer::OnEnabled()
{
	Graphics::s_isRenderingBatchDirty = true;
}

void TextRenderer::CreateRenderCommands(RenderBatch& renderBatch)
{
	if (!m_material || !m_font)
		return;

	RenderCommand command = RenderCommand();
	command.material = m_material.get();
	command.drawable = this;
	command.subMesh = nullptr;
	command.transform = GetTransformRaw();
	command.isEnabled = IsEnabled() && GetGameObjectRaw()->IsLocalActive();

	renderBatch.uiCommands.push_back(command);
	renderBatch.uiCommandIndex++;
}

/// <summary>
/// Draw text
/// </summary>
void TextRenderer::DrawCommand(const RenderCommand& renderCommand)
{
	XASSERT(renderCommand.material, "[TextRenderer::DrawCommand] Material is nullptr");
	XASSERT(m_font, "[TextRenderer::DrawCommand] Font is nullptr");

	if (m_isTextInfoDirty)
	{
		if (m_mesh)
		{
			delete m_textInfo;
			m_mesh.reset();
		}
		if(!m_text.empty())
		{
			m_textInfo = TextManager::GetTextInfomations(m_text, (int)m_text.size(), m_font, 1);
			m_mesh = TextManager::CreateMesh(m_text, m_textInfo, m_horizontalAlignment, m_verticalAlignment, m_color, m_font, m_fontSize);
		}
		m_isTextInfoDirty = false;
	}
	if (m_mesh)
	{
		TextManager::DrawText(m_text, m_textInfo, m_horizontalAlignment, m_verticalAlignment, *GetTransformRaw(), m_color, true, *m_mesh, *m_font, *m_material);
	}
}

void TextRenderer::SetFontSize(float fontSize)
{
	m_fontSize = fontSize;
	m_isTextInfoDirty = true;
}

void TextRenderer::SetLineSpacing(float lineSpacing)
{
	m_lineSpacing = lineSpacing;
	m_isTextInfoDirty = true;
}

void TextRenderer::SetCharacterSpacing(float characterSpacing)
{
	m_characterSpacing = characterSpacing;
	m_isTextInfoDirty = true;
}

void TextRenderer::SetVerticalAlignment(VerticalAlignment verticalAlignment)
{
	m_verticalAlignment = verticalAlignment;
	m_isTextInfoDirty = true;
}

void TextRenderer::SetHorizontalAlignment(HorizontalAlignment horizontalAlignment)
{
	m_horizontalAlignment = horizontalAlignment;
	m_isTextInfoDirty = true;
}
