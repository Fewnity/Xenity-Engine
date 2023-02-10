#pragma once

#include "../graphics/shader.h"
#include <glm/glm.hpp>
#include <map>
#include <vector>

class Character;

class Font {
public:
private:
};

class UiManager
{
public:
	static int Init();
	static void RenderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color);
	static Font* CreateFont(std::string filePath);
private:
	static void CreateTextBuffer();
	static unsigned int textVAO, textVBO;
	static std::vector<Font*> fonts;
	static std::map<char, Character> Characters;
};

