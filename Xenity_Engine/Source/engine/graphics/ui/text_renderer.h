#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once
#include <string>

#include "../../component.h"
#include "../iDrawable.h"
#include "TextAlignments.h"
#include "../color/color.h"
#include "../../engine.h"

class Font;
class TextInfo;
class MeshData;
// class Shader;

class API TextRenderer : public IDrawable
{
public:
	TextRenderer();
	//void SetReflection();
	std::unordered_map<std::string, Variable> GetReflection();
	// TextRenderer(Font* font, float size, Shader* shader);
	// TextRenderer(Font* font, float size, Shader* shader);
	~TextRenderer();

	float size = 16;
	float lineSpacing = 0;
	float characterSpacing = 0;
	int GetDrawPriority() const;

	// Shader *shader = nullptr;
	HorizontalAlignment horizontalAlignment = H_Center;
	VerticalAlignment verticalAlignment = V_Center;

	/**
	* Set order in layer
	* @param orderInLayer Order in layer
	*/
	void SetOrderInLayer(int orderInLayer)
	{
		this->orderInLayer = orderInLayer;
		Engine::drawOrderListDirty = true;
	}

	/**
	* Get order in layer
	*/
	int GetOrderInLayer() const
	{
		return orderInLayer;
	}

	/**
	* Set text color
	* @param color Color
	*/
	void SetColor(Color color)
	{
		this->color = color;
	}

	/**
	* Set text
	* @param text Text
	*/
	void SetText(std::string text);

	/**
	* Set text font
	* @param font Font
	*/
	void SetFont(Font *font);

private:
	Font *font = nullptr;
	std::string text;
	Color color = Color();
	void Draw();
	int orderInLayer = 0;
	TextInfo *textInfo = nullptr;
	bool isTextInfoDirty = true;
	MeshData *mesh = nullptr;
};
