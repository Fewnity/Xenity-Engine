// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gr�gory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include <string>

#include "TextAlignments.h"
#include <engine/graphics/iDrawable.h>
#include <engine/graphics/color/color.h>
#include <engine/vectors/vector3.h>

class Font;
class TextInfo;
class MeshData;

class API TextRendererCanvas : public IDrawable
{
public:
	TextRendererCanvas();
	~TextRendererCanvas();

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	/**
	 * @brief [Internal]
	 */
	int GetDrawPriority() const override;



	/**
	* @brief Set text color
	* @param color Color
	*/
	void SetColor(const Color& color)
	{
		this->color = color;
	}

	/**
	* @brief Set order in layer
	* @param orderInLayer Order in layer
	*/
	void SetOrderInLayer(int orderInLayer);

	/**
	* @brief Get order in layer
	*/
	int GetOrderInLayer() const
	{
		return orderInLayer;
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

	std::shared_ptr<Material> GetMaterial();

	void SetMaterial(std::shared_ptr<Material> _material);

private:
	std::shared_ptr<Material> material = nullptr;

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

	std::shared_ptr<MeshData> mesh = nullptr;
	std::shared_ptr<Font> font;
	TextInfo* textInfo = nullptr;
	std::string text;
	Color color = Color();
	int orderInLayer = 0;
	float fontSize = 1;

public:
	HorizontalAlignment horizontalAlignment = HorizontalAlignment::Center;
	VerticalAlignment verticalAlignment = VerticalAlignment::Center;
	float size = 16;
	float lineSpacing = 0;
	float characterSpacing = 0;

private:
	bool isTextInfoDirty = true;
};
