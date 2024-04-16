#pragma once

/**
 * [Internal]
 */

#include <engine/api.h>

#include <memory>

class Vector3;
class MeshData;
class Texture;
class Color;
class Transform;
class Material;

class API SpriteManager
{
public:

	/**
	* Init sprite manager
	*/
	static void Init();

	/**
	* Draw a sprite
	* @param position Sprite's position
	* @param rotation Sprite's roation
	* @param scale Sprite's scale
	* @param color Sprite's color
	* @param material Sprite's material
	* @param texture Sprite's texture
	*/
	static void DrawSprite(const Vector3& position, const Vector3& rotation, const Vector3& scale, const Color& color, const std::shared_ptr<Material>& material, const std::shared_ptr<Texture>& texture);
	
	/**
	* Draw a sprite
	* @param transform Sprite's transform
	* @param color Sprite's color
	* @param material Sprite's material
	* @param texture Sprite's texture
	*/
	static void DrawSprite(const std::shared_ptr<Transform>& transform, const Color& color, const std::shared_ptr<Material>& material, const std::shared_ptr<Texture>& texture);

	/**
	* Render a 2D line
	* @param meshData Mesh data
	*/
	static void Render2DLine(const std::shared_ptr<MeshData>& meshData);

private:
	static std::shared_ptr <MeshData> spriteMeshData;
};