#pragma once
#include <string>

#include "../component.h"
#include "iDrawable.h"
#include "../ui/TextAlignments.h"

class Font;
class Shader;
class Vector3;

class TextRenderer : public IDrawable
{
public:
	TextRenderer();
	TextRenderer(Font* font, float size, Shader* shader);
	~TextRenderer();

	std::string text;
	float size = 16;
	float lineSpacing = 0;
	float characterSpacing = 0;
	int GetDrawPriority();

	Font* font = nullptr;
	Vector3 color = Vector3(1,1,1); // [0;1]
	Shader* shader = nullptr;
	HorizontalAlignment horizontalAligment = H_Center;

private:
	void Draw();

};

