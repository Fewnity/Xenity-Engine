#pragma once
#include <string>

#include "../../component.h"
#include "../iDrawable.h"
#include "TextAlignments.h"
#include "../color/color.h"
#include "../../engine.h"

class Font;
class Shader;

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
	int GetDrawPriority() const;

	Font* font = nullptr;
	Color color = Color();
	Shader* shader = nullptr;
	HorizontalAlignment horizontalAligment = H_Center;
	VerticalAlignment verticalAlignment = V_Center;

	void SetOrderInLayer(int orderInLayer)
	{
		this->orderInLayer = orderInLayer;
		Engine::drawOrderListDirty = true;
	}

	int GetOrderInLayer() const
	{
		return orderInLayer;
	}

private:
	void Draw();
	int orderInLayer = 0;

};

