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
    static void DrawSprite(std::shared_ptr<Transform> transform, std::shared_ptr <Texture> texture, Color color);
    static void DrawSprite(Vector3 position, Vector3 rotation, Vector3 scale, std::shared_ptr <Texture> texture, Color color);
    /**
    * Render a 2D line
    * @param meshData Mesh data
    */
    static void Render2DLine(std::shared_ptr <MeshData> meshData);

private:
    static std::shared_ptr <MeshData> spriteMeshData;
};