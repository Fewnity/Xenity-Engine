// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <memory>

#include <engine/api.h>
#include <engine/graphics/iDrawable.h>
#include <engine/graphics/color/color.h>

class Texture;

/**
* @brief Component to render a billboard sprite (2D sprite that always faces the camera)
*/
class API BillboardRenderer : public IDrawable
{
public:

	///**
	//* @brief Set the order in layer of the sprite
	//*/
	//void SetOrderInLayer(int orderInLayer);

	/**
	* @brief Get the color of the sprite
	*/
	const Color& GetColor() const
	{
		return m_color;
	}

	/**
	* @brief Set the color of the sprite
	*/
	void SetColor(const Color& color)
	{
		m_color = color;
	}
	
	/**
	* @brief Get the texture of the sprite
	*/
	[[nodiscard]] const std::shared_ptr<Texture>& GetTexture()
	{
		return m_texture;
	}

	/**
	* @brief Set the texture of the sprite
	*/
	void SetTexture(const std::shared_ptr<Texture>& texture);

protected:
	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

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

	Color m_color = Color();
	std::shared_ptr<Texture> m_texture = nullptr;
};
