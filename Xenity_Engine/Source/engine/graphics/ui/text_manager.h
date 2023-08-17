#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "TextAlignments.h"
#include "../../vectors/vector2.h"
#include <memory>

class Texture;
class Vector4;
class Transform;
class MeshData;
class Vector3;
class Texture;
class Color;

class API Character
{
public:
    glm::ivec2 Size = glm::ivec2(0,0);          // Size of glyph
    glm::ivec2 Bearing = glm::ivec2(0, 0);       // Offset from baseline to left/top of glyph
    unsigned int Advance = 0; // Offset to advance to next glyph

    Vector2 rightSize = Vector2(0);
    Vector2 rightBearing = Vector2(0);

    float rightAdvance = 0;

    Vector2 uv = Vector2(0);
    Vector2 uvOffet = Vector2(0);
};

class API Font
{
public:
    ~Font();
    Character *Characters[256] = {};
    float maxCharHeight = 0;
    Texture *fontAtlas = nullptr;

private:
};

class API LineInfo
{
public:
    float lenght = 0;
    float y1 = 0;
};

class API TextInfo
{
public:
    std::vector<LineInfo> linesInfo;
    float maxLineHeight = 0;
    int lineCount = 0;
};

class API TextManager
{
public:
    static void Init();
    static Font *CreateFont(std::string filePath);
    //static void DeleteFont(Font *font);
    //static void DeleteFont(int index);

    static void DrawText(std::string &text, TextInfo *textInfo, HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment, std::weak_ptr<Transform> transform, Color color, bool canvas, MeshData *mesh, Font* font);

    static std::vector<Font *> fonts;
    static TextInfo *GetTextInfomations(std::string &text, int textLen, Font *font, float scale);
    static MeshData *CreateMesh(std::string &text, TextInfo *textInfo, HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment, Color color, Font* font);

private:
    static void SetTextPosition(std::weak_ptr<Transform> transform, bool canvas);
    static void DrawTextMesh(MeshData *mesh, bool for3D, bool invertFaces, Texture* texture);
    static void AddCharToMesh(MeshData *mesh, Character *ch, float x, float y, int letterIndex);

    static std::vector<MeshData *> meshes;
};