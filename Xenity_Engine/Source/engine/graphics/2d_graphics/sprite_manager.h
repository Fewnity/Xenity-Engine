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
    static void DrawSprite(Vector3 position, Vector3 rotation, Vector3 scale, Texture *texture, Color color, std::weak_ptr<Transform> transform);
    static void Render2DLine(MeshData *meshData);
    static void Init();

private:
    static MeshData *spriteMeshData;
};