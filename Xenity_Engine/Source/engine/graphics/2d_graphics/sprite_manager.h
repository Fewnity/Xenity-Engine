#pragma once

class Vector3;
class MeshData;
class Texture;
class Color;
#include <memory>
class Transform;

class SpriteManager
{
public:
    static void DrawSprite(Vector3 position, Vector3 rotation, Vector3 scale, Texture *texture, Color color, std::weak_ptr<Transform> transform);
    static void Init();

private:
    static MeshData *spriteMeshData;
};