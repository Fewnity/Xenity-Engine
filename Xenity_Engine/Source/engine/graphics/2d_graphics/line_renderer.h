// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include <engine/graphics/iDrawable.h>
#include <engine/vectors/vector3.h>
#include <engine/graphics/color/color.h>

class Texture;
class Material;
class MeshData;

class API LineRenderer : public IDrawable
{
public:
	LineRenderer();
	~LineRenderer();

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	void SetOrderInLayer(int orderInLayer);

	inline int GetOrderInLayer() const
	{
		return orderInLayer;
	}

	/**
	* @brief Set the color of the sprite
	*/
	inline void SetColor(const Color& color)
	{
		this->color = color;
	}

	inline const Color& GetColor() const
	{
		return color;
	}

	inline const Vector3& GetStartPosition()
	{
		return startPosition;
	}

	inline const Vector3& GetEndPosition()
	{
		return endPosition;
	}

	inline void SetStartPosition(const Vector3& newStartPosition)
	{
		startPosition = newStartPosition;
	}

	inline void SetEndPosition(const Vector3& newEndPosition)
	{
		endPosition = newEndPosition;
	}

private:
	Vector3 startPosition = Vector3(0, 0, 0);
	Vector3 endPosition = Vector3(0, 0, 0);

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
	std::shared_ptr <MeshData> meshData = nullptr;
	std::shared_ptr <Material> material = nullptr;
	int orderInLayer = 0;

public:
	float width = 1;
};

