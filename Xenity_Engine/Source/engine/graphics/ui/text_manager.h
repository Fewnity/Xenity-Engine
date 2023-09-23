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
#include "../../file_system/file_reference.h"
#include "../../reflection/reflection.h"

class Texture;
class Vector4;
class Transform;
class MeshData;
class Vector3;
class Color;
class Font;
class Material;

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

    /**
    * [Internal] Init text manager
    */
    static void Init();

    /**
    * Create font
    * @filePath Font file path
    */
    //static bool CreateFont(std::shared_ptr<Font>, std::string filePath);

    /**
    * Draw a text
    * @param text Text
    * @param textInfo Text info
    * @param horizontalAlignment Horizontal alignment
    * @param verticalAlignment Vertical alignment
    * @param transform Transform
    * @param color Color
    * @param canvas Is for canvas
    * @param mesh Mesh
    * @param font Font
    */
    static void DrawText(std::string &text, TextInfo *textInfo, HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment, std::weak_ptr<Transform> transform, Color color, bool canvas, std::shared_ptr <MeshData> mesh, std::shared_ptr<Font> font, std::shared_ptr <Material> material);

    /**
    * Get informations about a text
    * @param text Text
    * @param textLen Text lenght
    * @param font Font
    * @param scale Test scale
    */
    static TextInfo *GetTextInfomations(std::string &text, int textLen, std::shared_ptr<Font> font, float scale);
    
    /**
    * Create a mesh from a text
    * @param text Text
    * @param textInfo Text informations
    * @param horizontalAlignment Horizontal alignment
    * @param verticalAlignment Vertical alignment
    * @param color Color
    * @param font Font
    */
    static std::shared_ptr <MeshData> CreateMesh(std::string &text, TextInfo *textInfo, HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment, Color color, std::shared_ptr<Font> font);

private:

    /**
    * @brief Set the text mesh position
    *
    * @param transform
    * @param canvas
    */
    static void SetTextPosition(std::weak_ptr<Transform> transform, bool canvas);

    /**
    * Draw mesh
    *
    * @param mesh Mesh data
    * @param for3D 3D text mode
    */
    static void DrawTextMesh(std::shared_ptr <MeshData> mesh, bool for3D, bool invertFaces, std::shared_ptr <Texture> texture);

    /**
    * @brief Add a char to the mesh
    *
    * @param mesh Mesh to modify
    * @param ch Char to add
    * @param x Char X position
    * @param y Char Y position
    * @param letterIndex Letter index in the string
    */
    static void AddCharToMesh(std::shared_ptr<MeshData> mesh, Character *ch, float x, float y, int letterIndex);

    static std::vector<std::shared_ptr<MeshData>> meshes;
};