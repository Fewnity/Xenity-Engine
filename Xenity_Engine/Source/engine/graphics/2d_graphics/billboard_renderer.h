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

class API BillboardRenderer : public IDrawable
{
public:
	BillboardRenderer();
	~BillboardRenderer();

	void SetOrderInLayer(int orderInLayer);

	/**
	* @brief Set the color of the sprite
	*/
	inline void SetColor(const Color& color)
	{
		m_color = color;
	}

	/**
	* @brief Get the material of the sprite
	*/
	inline std::shared_ptr <Material> GetMaterial()
	{
		return m_material;
	}

	/**
	* @brief Get the texture of the sprite
	*/
	inline std::shared_ptr <Texture> GetTexture()
	{
		return m_texture;
	}

	/**
	* @brief Set the material of the sprite
	*/
	void SetMaterial(const std::shared_ptr <Material>& material);
	
	/**
	* @brief Set the texture of the sprite
	*/
	void SetTexture(const std::shared_ptr <Texture>& texture);

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
	std::shared_ptr <Material> m_material = nullptr;
	std::shared_ptr <Texture> m_texture = nullptr;
	int m_orderInLayer = 0;
};
