// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include <engine/vectors/vector2.h>
#include <engine/vectors/vector3.h>
#include <engine/vectors/vector4.h>
#include <engine/graphics/color/color.h>
#include <engine/file_system/file_reference.h>
#include <engine/reflection/reflection.h>
#include <engine/graphics/material_rendering_modes.h>
#include "iDrawableTypes.h"

class Texture;
class Shader;
class Camera;

class Material : public FileReference, public Reflective
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
	//void SetAttribute(const char* attribut, Texture* value);

	inline void SetShader(const std::shared_ptr<Shader>& _shader)
	{
		m_shader = _shader;
	}

	inline void SetTexture(const std::shared_ptr<Texture>& _texture)
	{
		m_texture = _texture;
	}

	inline void SetUseLighting(const bool _useLighting)
	{
		m_useLighting = _useLighting;
	}

	inline void SetOffset(const Vector2& _offset)
	{
		t_offset = _offset;
	}

	inline void SetTiling(const Vector2& _tiling)
	{
		t_tiling = _tiling;
	}

	inline const std::shared_ptr<Shader>& GetShader() const
	{
		return m_shader;
	}

	inline const std::shared_ptr<Texture>& GetTexture() const
	{
		return m_texture;
	}

	inline bool GetUseLighting() const
	{
		return m_useLighting;
	}

	MaterialRenderingModes GetRenderingMode() const
	{
		return m_renderingMode;
	}

	inline const Vector2& GetOffset() const
	{
		return t_offset;
	}

	inline const Vector2& GetTiling() const
	{
		return t_tiling;
	}

	inline const Color& GetColor() const
	{
		return m_color;
	}

	inline void SetColor(const Color& _color)
	{
		m_color = _color;
	}

	void SetAlphaCutoff(float _alphaCutoff)
	{
		m_alphaCutoff = _alphaCutoff;
		if (m_alphaCutoff < 0.0f)
		{
			m_alphaCutoff = 0.0f;
		}
		else if (m_alphaCutoff > 1.0f)
		{
			m_alphaCutoff = 1.0f;
		}
	}

	float GetAlphaCutoff() const
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

	void LoadFileReference() override;
	static std::shared_ptr<Material> MakeMaterial();

	/**
	* @brief Update the material
	*/
	void Update();

	std::weak_ptr<Camera> m_lastUsedCamera;
	//std::unordered_map <const char *, Texture*> uniformsTextures;
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
	MaterialRenderingModes m_renderingMode = MaterialRenderingModes::Opaque;
	float m_alphaCutoff = 0.5f;
	bool m_updated = false;
	bool m_useLighting = false; // Defines if the material uses lighting or not in fixed pipeline mode (PSP)

	static constexpr int s_version = 1;
};

