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

/**
* @brief Component for rendering text in a canvas
*/
class API TextRenderer : public IDrawable
{
public:
	TextRenderer();
	~TextRenderer();

	/**
	* @brief Get text color
	*/
	const Color& GetColor() const
	{
		return m_color;
	}

	/**
	* @brief Set text color
	* @param color Color
	*/
	void SetColor(const Color& color)
	{
		m_color = color;
	}

	///**
	//* @brief Set order in layer
	//* @param orderInLayer Order in layer
	//*/
	//void SetOrderInLayer(int orderInLayer);

	///**
	//* @brief Get order in layer
	//*/
	//[[nodiscard]] int GetOrderInLayer() const
	//{
	//	return m_orderInLayer;
	//}

	/**
	* @brief Get text
	*/
	const std::string& GetText() const
	{
		return m_text;
	}

	/**
	* @brief Set text
	* @param text Text
	*/
	void SetText(const std::string& text);

	/**
	* @brief Get text font
	*/
	const std::shared_ptr<Font>& GetFont() const
	{
		return m_font;
	}

	/**
	* @brief Set text font
	* @param font Font
	*/
	void SetFont(const std::shared_ptr<Font>& font);

	/**
	* @brief Get the material
	*/
	[[nodiscard]] const std::shared_ptr<Material>& GetMaterial()
	{
		return m_material;
	}

	/**
	* @brief Set the material
	*/
	void SetMaterial(std::shared_ptr<Material> material);

	/**
	* @brief Get the font size
	*/
	[[nodiscard]] float GetFontSize() const
	{
		return m_fontSize;
	}

	/**
	* @brief Set the font size
	*/
	void SetFontSize(float fontSize);

	/**
	* @brief Get the line spacing
	*/
	[[nodiscard]] float GetLineSpacing() const
	{
		return m_lineSpacing;
	}

	/**
	* @brief Set the line spacing
	*/
	void SetLineSpacing(float lineSpacing);

	/**
	* @brief Get the character spacing
	*/
	[[nodiscard]] float GetCharacterSpacing() const
	{
		return m_characterSpacing;
	}

	/**
	* @brief Set the character spacing
	*/
	void SetCharacterSpacing(float characterSpacing);

	/**
	* @brief Get the vertical alignment
	*/
	[[nodiscard]] VerticalAlignment GetVerticalAlignment() const
	{
		return m_verticalAlignment;
	}

	/**
	* @brief Set the vertical alignment
	*/
	void SetVerticalAlignment(VerticalAlignment verticalAlignment);

	/**
	* @brief Get the horizontal alignment
	*/
	[[nodiscard]] HorizontalAlignment GetHorizontalAlignment() const
	{
		return m_horizontalAlignment;
	}

	/**
	* @brief Set the horizontal alignment
	*/
	void SetHorizontalAlignment(HorizontalAlignment horizontalAlignment);

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
	*/
	void CreateRenderCommands(RenderBatch& renderBatch) override;

	/**
	* @brief Draw the command
	*/
	void DrawCommand(const RenderCommand& renderCommand) override;

	std::shared_ptr<MeshData> m_mesh = nullptr;
	std::shared_ptr<Font> m_font;
	TextInfo* m_textInfo = nullptr;
	std::string m_text;
	Color m_color = Color();
	int m_orderInLayer = 0;
	float m_fontSize = 1;

	HorizontalAlignment m_horizontalAlignment = HorizontalAlignment::Center;
	VerticalAlignment m_verticalAlignment = VerticalAlignment::Center;
	float m_lineSpacing = 0;
	float m_characterSpacing = 0;

	bool m_isTextInfoDirty = true;
};
