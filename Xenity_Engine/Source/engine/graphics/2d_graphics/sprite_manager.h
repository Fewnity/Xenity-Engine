#pragma once

class Vector3;

class Vector4;
class Texture;
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct Vertex
{
    float u, v;
    unsigned int color;
    float x, y, z;
};

class MeshData
{
public:
    MeshData() = delete;
    MeshData(unsigned int vcount, unsigned int index_count);
    ~MeshData();

    void AddVertice(float u, float v, unsigned int color, float x, float y, float z, int indice);

    Vertex *data = nullptr;

    unsigned int *indices = nullptr;
    unsigned int index_count = 0;
};

class SpriteManager
{
public:
    static void StartDraw();
    static void EndDraw();
    static void DrawSprite(Vector3 position, Vector3 rotation, Vector3 scale, Texture *texture);
    static void Init();
};

struct Character
{
    Texture *texture;     // Glyph texture
    glm::ivec2 Size;      // Size of glyph
    glm::ivec2 Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance; // Offset to advance to next glyph
};

class Font
{
public:
    ~Font();
    Character Characters[256] = {};
    float maxCharHeight = 0;

private:
};

class UiManager
{
public:
    static int Init();
    static Font *CreateFont(std::string filePath);
    static std::vector<Font *> fonts;

private:
    static void DeleteFont(Font *font);
    static void DeleteFont(int index);
    static std::vector<Vector4> GetTextLenght(std::string &text, int textLen, Font *font, float scale);
};