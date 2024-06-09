// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <unordered_map>
#include <string>

class Texture;
class Shader;
class Camera;

// Vectors includes needed for other platforms
#include <engine/vectors/vector2.h>
#include <engine/vectors/vector3.h>
#include <engine/vectors/vector4.h>
//

#include <engine/file_system/file_reference.h>
#include <engine/reflection/reflection.h>
#include "iDrawableTypes.h"
#include <memory>

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
		shader = _shader;
	}

	inline void SetTexture(const std::shared_ptr<Texture>& _texture)
	{
		texture = _texture;
	}

	inline void SetUseLighting(const bool _useLighting)
	{
		useLighting = _useLighting;
	}

	inline void SetUseTransparency(const bool _useTransparency)
	{
		useTransparency = _useTransparency;
	}

	inline void SetOffset(const Vector2& _offset)
	{
		offset = _offset;
	}

	inline void SetTiling(const Vector2& _tiling)
	{
		tiling = _tiling;
	}

	inline std::shared_ptr<Shader> GetShader() const
	{
		return shader;
	}

	inline std::shared_ptr<Texture> GetTexture() const
	{
		return texture;
	}

	inline bool GetUseLighting() const
	{
		return useLighting;
	}

	inline bool GetUseTransparency() const
	{
		return useTransparency;
	}

	inline Vector2 GetOffset() const
	{
		return offset;
	}

	inline Vector2 GetTiling() const
	{
		return tiling;
	}

protected:
	friend class AssetManager;
	friend class ProjectManager;
	friend class Graphics;

	// [Internal]
	void Use();

	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData() override;
	void OnReflectionUpdated() override;

	void LoadFileReference() override;
	static std::shared_ptr<Material> MakeMaterial();

	/**
	* @brief Update the material
	*/
	void Update();

	std::weak_ptr<Camera> lastUsedCamera;
	//std::unordered_map <const char *, Texture*> uniformsTextures;
	std::unordered_map <const char*, Vector2> uniformsVector2;
	std::unordered_map <const char*, Vector3> uniformsVector3;
	std::unordered_map <const char*, Vector4> uniformsVector4;
	std::unordered_map <const char*, int> uniformsInt;
	std::unordered_map <const char*, float> uniformsFloat;

	std::shared_ptr<Shader> shader = nullptr;
	std::shared_ptr<Texture> texture;
	IDrawableTypes lastUpdatedType = IDrawableTypes::Draw_3D;
	bool updated = false;
	bool useLighting = false;
	bool useTransparency = false;
	Vector2 offset = Vector2(0,0);
	Vector2 tiling = Vector2(1, 1);

	static constexpr int version = 1;
};

