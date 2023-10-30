#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once
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
	* @param texture Sprite's texture
	* @param color Sprite's color
	* @param transform Sprite's transform
	*/
	//static void DrawSprite(std::shared_ptr<Texture> texture, Color color, std::shared_ptr<Transform> transform);
	static void DrawSprite(const std::shared_ptr<Transform>& transform, const std::shared_ptr<Texture>& texture, const Color& color, const std::shared_ptr<Material>& material);
	static void DrawSprite(const Vector3& position, const Vector3& rotation, const Vector3& scale, const std::shared_ptr<Texture>& texture, const Color& color, const std::shared_ptr<Material>& material);

	/**
	* Render a 2D line
	* @param meshData Mesh data
	*/
	static void Render2DLine(const std::shared_ptr<MeshData>& meshData);

private:
	static std::shared_ptr <MeshData> spriteMeshData;
};