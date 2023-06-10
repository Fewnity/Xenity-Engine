#pragma once

class Vector3;
class MeshData;
class Texture;

class SpriteManager
{
public:
    static void DrawSprite(Vector3 position, Vector3 rotation, Vector3 scale, Texture *texture);
    static void Init();

private:
    static MeshData *spriteMeshData;
};