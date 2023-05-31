#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "TextAlignments.h"

class Texture;
class Vector4;
class Transform;
class MeshData;
class Vector3;
class Texture;

struct Character
{
    Texture *texture;     // Glyph texture
    glm::ivec2 Size;      // Size of glyph
    glm::ivec2 Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance; // Offset to advance to next glyph
    MeshData *mesh;
};

class Font
{
public:
    ~Font();
    Character Characters[256] = {};
    float maxCharHeight = 0;

private:
};

class TextManager
{
public:
    static void Init();
    static Font *CreateFont(std::string filePath);
    static void DrawText(std::string text, HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment, Transform *transform);
    static void DrawCharacter(Vector3 position, Vector3 rotation, Vector3 scale, Texture *texture, Character *ch);
    static std::vector<Font *> fonts;
    static void CreateCharacterMesh(Character *chara);

private:
    // static MeshData *spriteMeshData;
    static void DeleteFont(Font *font);
    static void DeleteFont(int index);
    static std::vector<Vector4> GetTextLenght(std::string &text, int textLen, Font *font, float scale);
};