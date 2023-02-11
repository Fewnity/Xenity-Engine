#pragma once

#include "../graphics/shader.h"
#include <glm/glm.hpp>
#include <map>
#include <vector>

class Character;

class Font {
public:
	std::map<char, Character> Characters;
private:
};

class UiManager
{
public:
	static int Init();
	static void RenderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color, Font* font);
	static Font* CreateFont(std::string filePath);
	static std::vector<Font*> fonts;
private:
	static void CreateTextBuffer();
	static unsigned int textVAO, textVBO;
};

