#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

class Texture;
class Vector4;

struct Character
{
    Texture* texture;     // Glyph texture
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

class TextManager
{
public:
    static Font* CreateFont(std::string filePath);
    static std::vector<Font*> fonts;

private:
    static void DeleteFont(Font* font);
    static void DeleteFont(int index);
    static std::vector<Vector4> GetTextLenght(std::string& text, int textLen, Font* font, float scale);
};