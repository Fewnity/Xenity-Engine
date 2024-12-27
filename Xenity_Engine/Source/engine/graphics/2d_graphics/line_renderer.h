// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(ENABLE_EXPERIMENTAL_FEATURES)

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
		return m_orderInLayer;
	}

	/**
	* @brief Set the color of the sprite
	*/
	inline void SetColor(const Color& color)
	{
		m_color = color;
	}

	inline const Color& GetColor() const
	{
		return m_color;
	}

	inline const Vector3& GetStartPosition()
	{
		return m_startPosition;
	}

	inline const Vector3& GetEndPosition()
	{
		return m_endPosition;
	}

	inline void SetStartPosition(const Vector3& newStartPosition)
	{
		m_startPosition = newStartPosition;
	}

	inline void SetEndPosition(const Vector3& newEndPosition)
	{
		m_endPosition = newEndPosition;
	}

protected:
	Vector3 m_startPosition = Vector3(0, 0, 0);
	Vector3 m_endPosition = Vector3(0, 0, 0);

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
	std::shared_ptr <MeshData> m_meshData = nullptr;
	std::shared_ptr <Material> m_material = nullptr;
	int m_orderInLayer = 0;

public:
	float width = 1;
};

#endif // ENABLE_EXPERIMENTAL_FEATURES