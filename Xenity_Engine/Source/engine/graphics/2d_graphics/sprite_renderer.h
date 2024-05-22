// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

class Texture;

#include <engine/component.h>
#include <engine/graphics/iDrawable.h>
#include <engine/graphics/color/color.h>
#include <engine/engine.h>
#include <engine/game_elements/gameplay_manager.h>
#include <memory>

class API SpriteRenderer : public IDrawable
{
public:
	SpriteRenderer();
	~SpriteRenderer();

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	/**
	 * @brief [Internal]
	 */
	int GetDrawPriority() const override;

	void SetOrderInLayer(int orderInLayer);

	int GetOrderInLayer() const
	{
		return orderInLayer;
	}

	/**
	* @brief Set the color of the sprite
	*/
	void SetColor(const Color& color)
	{
		this->color = color;
	}

	/**
	* @brief Set the material of the sprite
	*/
	void SetMaterial(std::shared_ptr <Material> material);

	/**
	* @brief Set the texture of the sprite
	*/
	void SetTexture(std::shared_ptr <Texture> texture);

private:

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

	Color color = Color();
	std::shared_ptr <Material> material = nullptr;
	std::shared_ptr <Texture> texture = nullptr;
	int orderInLayer = 0;
};
