#pragma once

class Vector3;

class Vector4;
class Texture;
#include <glm/glm.hpp>
#include <vector>
#include <string>

class SpriteManager
{
public:
    static void DrawSprite(Vector3 position, Vector3 rotation, Vector3 scale, Texture *texture);
    static void Init();
};