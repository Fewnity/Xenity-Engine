#pragma once
#include <string>

#include "../component.h"
#include "iDrawable.h"

class Font;
class Shader;
class Vector3;

class TextRenderer : public Component, public IDrawable
{
public:
	TextRenderer();
	~TextRenderer();

	void Draw();

	std::string text;
	float size = 16;
	Font* font = nullptr;
	Vector3 color = Vector3(1,1,1); // [0;1]
	Shader* shader = nullptr;

private:

};

