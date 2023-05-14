#pragma once
#include <string>

#include "../../component.h"
#include "../iDrawable.h"
#include "TextAlignments.h"
#include "../color/color.h"
#include "../../vectors/vector3.h"

class Font;
class Shader;

class TextRendererCanvas : public IDrawable
{
public:
	TextRendererCanvas();
	TextRendererCanvas(Font* font, float size, Shader* shader);
	~TextRendererCanvas();
	Vector3 position = Vector3(0,0,0);
	std::string text;
	float size = 16;
	float lineSpacing = 0;
	float characterSpacing = 0;
	int GetDrawPriority();

	Font* font = nullptr;
	Color color = Color(); // [0;1]
	Shader* shader = nullptr;
	HorizontalAlignment horizontalAligment = H_Center;
	VerticalAlignment verticalAlignment = V_Center;

	void SetOrderInLayer(int orderInLayer)
	{
		this->orderInLayer = orderInLayer;
		needReorder = true;
	}

	int GetOrderInLayer() const
	{
		return orderInLayer;
	}

private:
	void Draw();
	int orderInLayer = 0;

};

