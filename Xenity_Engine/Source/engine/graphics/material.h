// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include <engine/math/vector2.h>
#include <engine/math/vector3.h>
#include <engine/math/vector4.h>
#include <engine/graphics/color/color.h>
#include <engine/file_system/file_reference.h>
#include <engine/reflection/reflection.h>
#include <engine/graphics/material_rendering_mode.h>
#include "iDrawableTypes.h"

class Texture;
class Shader;
class Camera;

class Material : public FileReference
{
public:
	Material();
	~Material();

	/**
	* @brief Set attribute of the material
	* @param attribute The attribute to set
	* @param value The value to set
	*/
	void SetAttribute(const char* attribute, const Vector2& value);
	void SetAttribute(const char* attribute, const Vector3& value);
	void SetAttribute(const char* attribute, const Vector4& value);
	void SetAttribute(const char* attribute, const float value);
	void SetAttribute(const char* attribute, const int value);

	/**
	* @brief Set the material shader
	*/
	void SetShader(const std::shared_ptr<Shader>& shader)
	{
		m_shader = shader;
		m_updated = false;
	}

	/**
	* @brief Set the material texture
	*/
	void SetTexture(const std::shared_ptr<Texture>& texture)
	{
		m_texture = texture;
	}

	/**
	* @brief Set the if the material uses lighting
	* @brief Disable this only on Unlit shaders
	*/
	void SetUseLighting(const bool useLighting)
	{
		m_useLighting = useLighting;
	}

	/**
	* @brief Set the texture offset
	*/
	void SetOffset(const Vector2& offset)
	{
		t_offset = offset;
	}

	/**
	* @brief Set the texture tiling
	*/
	void SetTiling(const Vector2& tiling)
	{
		t_tiling = tiling;
	}

	/**
	* @brief Get the shader of the material
	*/
	[[nodiscard]] const std::shared_ptr<Shader>& GetShader() const
	{
		return m_shader;
	}

	/**
	* @brief Get the texture of the material
	*/
	[[nodiscard]] const std::shared_ptr<Texture>& GetTexture() const
	{
		return m_texture;
	}

	/**
	* @brief Get if the material uses lighting
	*/
	[[nodiscard]] bool GetUseLighting() const
	{
		return m_useLighting;
	}

	/**
	* @brief Get the rendering mode of the material
	*/
	[[nodiscard]] MaterialRenderingMode GetRenderingMode() const
	{
		return m_renderingMode;
	}

	/**
	* @brief Get the texture offset of the material
	*/
	[[nodiscard]] const Vector2& GetOffset() const
	{
		return t_offset;
	}

	/**
	* @brief Get the texture tiling of the material
	*/
	[[nodiscard]] const Vector2& GetTiling() const
	{
		return t_tiling;
	}

	/**
	* @brief Get the color of the material
	*/
	[[nodiscard]] const Color& GetColor() const
	{
		return m_color;
	}

	/**
	* @brief Set the color of the material
	*/
	void SetColor(const Color& color)
	{
		m_color = color;
	}

	/**
	* @brief Set the alpha cutoff of the material
	*/
	void SetAlphaCutoff(float alphaCutoff)
	{
		m_alphaCutoff = alphaCutoff;
		if (m_alphaCutoff < 0.0f)
		{
			m_alphaCutoff = 0.0f;
		}
		else if (m_alphaCutoff > 1.0f)
		{
			m_alphaCutoff = 1.0f;
		}
	}

	/**
	* @brief Get the alpha cutoff of the material
	*/
	[[nodiscard]] float GetAlphaCutoff() const
	{
		return m_alphaCutoff;
	}

protected:
	friend class AssetManager;
	friend class ProjectManager;
	friend class Graphics;

	// [Internal]
	void Use();

	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData(AssetPlatform platform) override;
	void OnReflectionUpdated() override;

	void LoadFileReference(const LoadOptions& loadOptions) override;
	[[nodiscard]] static std::shared_ptr<Material> MakeMaterial();
	[[nodiscard]] std::vector<uint64_t> GetUsedFilesIds();

	/**
	* @brief Update the material
	*/
	void Update();

	Camera* m_lastUsedCamera = nullptr;
	std::unordered_map <const char*, Vector2> m_uniformsVector2;
	std::unordered_map <const char*, Vector3> m_uniformsVector3;
	std::unordered_map <const char*, Vector4> m_uniformsVector4;
	std::unordered_map <const char*, int> m_uniformsInt;
	std::unordered_map <const char*, float> m_uniformsFloat;

	std::shared_ptr<Shader> m_shader = nullptr;
	std::shared_ptr<Texture> m_texture;
	Color m_color;
	Vector2 t_offset = Vector2(0,0);
	Vector2 t_tiling = Vector2(1, 1);
	IDrawableTypes m_lastUpdatedType = IDrawableTypes::Draw_3D;
	MaterialRenderingMode m_renderingMode = MaterialRenderingMode::Opaque;
	float m_alphaCutoff = 0.5f;
	bool m_updated = false;
	bool m_useLighting = false; // Defines if the material uses lighting or not in fixed pipeline mode (PSP)

	static constexpr int s_version = 1;
};

