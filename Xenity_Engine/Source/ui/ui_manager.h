#pragma once

#include "../graphics/shader.h"
#include <glm/glm.hpp>
#include <map>
#include <vector>

class Character;

class Font {
public:
	~Font();
	std::map<char, Character> Characters;
private:
};

class UiManager
{
public:
	enum Aligment {
		Left,
		Center,
		Right
	};
	static int Init();
	static void RenderText(Shader& s, std::string text, float x, float y, float angle, float scale, float lineSpacing, Vector3 color, Font* font);
	static void RenderTextCanvas(std::string text, float x, float y, float angle, float scale, float lineSpacing, Vector3 color, Font* font, Aligment aligment, Shader& s);
	//static void RenderText(Shader& s, std::string text, float x, float y, float z, float scale, glm::vec3 color, Font* font);
	static Font* CreateFont(std::string filePath);
	static std::vector<Font*> fonts;
private:
	static void CreateTextBuffer();
	static void DeleteFont(Font * font);
	static void DeleteFont(int index);
	static unsigned int textVAO, textVBO;
};

