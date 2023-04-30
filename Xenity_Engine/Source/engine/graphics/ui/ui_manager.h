#pragma once

#include "../shader.h"
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include "TextAlignments.h"

class Vector4;

struct Character
{
	unsigned int TextureID;  // ID handle of the glyph texture
	glm::ivec2   Size;       // Size of glyph
	glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
};

class Font {
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
	static void RenderText(std::string text, float x, float y, float angle, float scale, float lineSpacing, Vector3 color, Font* font, HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment, Shader& s);
	static void RenderTextCanvas(std::string text, float x, float y, float angle, float scale, float lineSpacing, Vector3 color, Font* font, HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment, Shader& s);
	//static void RenderText(Shader& s, std::string text, float x, float y, float z, float scale, glm::vec3 color, Font* font);
	static Font* CreateFont(std::string filePath);
	static std::vector<Font*> fonts;
private:
	static void CreateTextBuffer();
	static void DeleteFont(Font * font);
	static void DeleteFont(int index);
	static std::vector<Vector4> GetTextLenght(std::string &text, int textLen, Font* font, float scale);
	static unsigned int textVAO, textVBO;
	static unsigned int textVAO2, textVBO2;
};

