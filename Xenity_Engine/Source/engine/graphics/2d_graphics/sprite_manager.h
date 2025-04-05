// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

#include <memory>

#include <engine/api.h>

class Vector3;
class MeshData;
class Texture;
class Color;
class Transform;
class Material;
class Quaternion;

class API SpriteManager
{
public:

	/**
	* @brief Init sprite manager
	*/
	static void Init();

	static void Close();

	/**
	* @brief Draw a sprite
	* @param position Sprite's position
	* @param rotation Sprite's roation
	* @param scale Sprite's scale
	* @param color Sprite's color
	* @param material Sprite's material
	* @param texture Sprite's texture
	*/
	static void DrawSprite(const Vector3& position, const Quaternion& rotation, const Vector3& scale, const Color& color, Material& material, Texture* texture);
	
	/**
	* @brief Draw a sprite
	* @param transform Sprite's transform
	* @param color Sprite's color
	* @param material Sprite's material
	* @param texture Sprite's texture
	*/
	static void DrawSprite(Transform& transform, const Color& color, Material& material, Texture* texture);

	/**
	* @brief Render a 2D line
	* @param meshData Mesh data
	*/
	static void Render2DLine(const std::shared_ptr<MeshData>& meshData);

	[[nodiscard]] static const std::shared_ptr <MeshData>& GetBasicSpriteMeshData()
	{
		return s_spriteMeshData;
	}

	[[nodiscard]] static const std::shared_ptr <MeshData>& GetBasicSpriteMeshDataWithNormals()
	{
		return s_spriteMeshDataWithNormals;
	}

private:
	static std::shared_ptr <MeshData> s_spriteMeshData;
	static std::shared_ptr <MeshData> s_spriteMeshDataWithNormals;
};