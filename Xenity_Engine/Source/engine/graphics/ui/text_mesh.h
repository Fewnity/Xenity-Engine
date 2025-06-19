// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <string>

#include <engine/api.h>
#include <engine/graphics/iDrawable.h>
#include <engine/graphics/color/color.h>
#include "text_alignments.h"

class Font;
struct TextInfo;
class MeshData;

class API TextMesh : public IDrawable
{
public:
	TextMesh();
	~TextMesh();

	/**
	* @brief Set text color
	* @param color Color
	*/
	void SetColor(const Color& color)
	{
		m_color = color;
	}

	/**
	* @brief Set text
	* @param text Text
	*/
	void SetText(const std::string& text);

	/**
	* @brief Set text font
	* @param font Font
	*/
	void SetFont(const std::shared_ptr<Font>& font);

	[[nodiscard]] std::shared_ptr<Material> GetMaterial() const
	{
		return m_material;
	}

	void SetMaterial(std::shared_ptr<Material> _material);

	void SetFontSize(float fontSize);
	[[nodiscard]] float GetFontSize() const
	{
		return m_fontSize;
	}

	void SetLineSpacing(float lineSpacing);
	[[nodiscard]] float GetLineSpacing() const
	{
		return m_lineSpacing;
	}

	void SetCharacterSpacing(float characterSpacing);
	[[nodiscard]] float GetCharacterSpacing() const
	{
		return m_characterSpacing;
	}

	void SetVerticalAlignment(VerticalAlignment verticalAlignment);
	[[nodiscard]] VerticalAlignment GetVerticalAlignment() const
	{
		return m_verticalAlignment;
	}

	void SetHorizontalAlignment(HorizontalAlignment horizontalAlignment);
	[[nodiscard]] HorizontalAlignment GetHorizontalAlignment() const
	{
		return m_horizontalAlignment;
	}

protected:

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	std::shared_ptr<Material> m_material = nullptr;

	/**
	* @brief Called when the component is disabled
	*/
	void OnDisabled() override;

	/**
	* @brief Called when the component is enabled
	*/
	void OnEnabled() override;

	/**
	* @brief Create the render commands
	* @param renderBatch Render batch
	*/
	void CreateRenderCommands(RenderBatch& renderBatch) override;

	/**
	* Draw the command
	* @param renderCommand Render command
	*/
	void DrawCommand(const RenderCommand& renderCommand) override;

	TextInfo* m_textInfo = nullptr;
	std::shared_ptr <MeshData> m_mesh = nullptr;
	std::shared_ptr<Font> m_font;
	std::string m_text;
	Color m_color = Color();
	int m_orderInLayer = 0;
	float m_fontSize = 1;

	float m_lineSpacing = 0;
	float m_characterSpacing = 0;
	HorizontalAlignment m_horizontalAlignment = HorizontalAlignment::Center;
	VerticalAlignment m_verticalAlignment = VerticalAlignment::Center;

	bool m_isTextInfoDirty = true;
};
