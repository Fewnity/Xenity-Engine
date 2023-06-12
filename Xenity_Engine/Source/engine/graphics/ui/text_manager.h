#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "TextAlignments.h"
#include "../../vectors/vector2.h"

class Texture;
class Vector4;
class Transform;
class MeshData;
class Vector3;
class Texture;
class Color;

class Character
{
public:
    glm::ivec2 Size;          // Size of glyph
    glm::ivec2 Bearing;       // Offset from baseline to left/top of glyph
    unsigned int Advance = 0; // Offset to advance to next glyph

    Vector2 rightSize;
    Vector2 rightBearing;

    float rightAdvance;

    Vector2 uv;
    Vector2 uvOffet;
};

class Font
{
public:
    ~Font();
    Character *Characters[256] = {};
    float maxCharHeight = 0;
    Texture *fontAtlas = nullptr;

private:
};

class LineInfo
{
public:
    float lenght = 0;
    float y1 = 0;
};

class TextInfo
{
public:
    std::vector<LineInfo> linesInfo;
    float maxLineHeight = 0;
    int lineCount = 0;
};

class TextManager
{
public:
    static void Init();
    static Font *CreateFont(std::string filePath);
    static void DeleteFont(Font *font);
    static void DeleteFont(int index);

    static void DrawText(std::string text, HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment, Transform *transform, Color color, bool canvas);
    static void ClearTexts();

    static std::vector<Font *> fonts;

private:
    static void SetTextPosition(Transform *transform, bool canvas);
    static void DrawTextMesh(MeshData *mesh, bool for3D, bool invertFaces);
    static void AddCharToMesh(MeshData *mesh, Character *ch, float x, float y, int letterIndex);
    static TextInfo GetTextInfomations(std::string &text, int textLen, Font *font, float scale);

    static std::vector<MeshData *> meshes;
};