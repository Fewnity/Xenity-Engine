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
#include "../../game_elements/gameplay_manager.h"

class Font;
class TextInfo;
class MeshData;

class API TextRenderer : public IDrawable
{
public:
	TextRenderer();
	std::unordered_map<std::string, ReflectionEntry> GetReflection() override;
	void OnReflectionUpdated() override;

	~TextRenderer();

	float size = 16;
	float lineSpacing = 0;
	float characterSpacing = 0;
	int GetDrawPriority() const override;

	std::shared_ptr<Material> material = nullptr;
	HorizontalAlignment horizontalAlignment = H_Center;
	VerticalAlignment verticalAlignment = V_Center;

	/**
	* Set order in layer
	* @param orderInLayer Order in layer
	*/
	void SetOrderInLayer(int orderInLayer)
	{
		this->orderInLayer = orderInLayer;
		GameplayManager::drawOrderListDirty = true;
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
	void SetColor(const Color& color)
	{
		this->color = color;
	}

	/**
	* Set text
	* @param text Text
	*/
	void SetText(const std::string& text);

	/**
	* Set text font
	* @param font Font
	*/
	void SetFont(std::shared_ptr<Font> font);

private:
	void Draw() override;
	std::shared_ptr<Font> font;
	std::string text;
	Color color = Color();
	int orderInLayer = 0;
	TextInfo* textInfo = nullptr;
	bool isTextInfoDirty = true;
	std::shared_ptr <MeshData> mesh = nullptr;
};
