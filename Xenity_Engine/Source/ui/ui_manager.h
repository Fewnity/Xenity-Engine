#pragma once

#include "../graphics/shader.h"
#include <glm/glm.hpp>

class UiManager
{
public:
	static int Init();
	static void RenderText(Shader& s, std::string text, float x, float y, float scale, glm::vec3 color);
private:
	static void CreateTextBuffer();
	static unsigned int textVAO, textVBO;
};

