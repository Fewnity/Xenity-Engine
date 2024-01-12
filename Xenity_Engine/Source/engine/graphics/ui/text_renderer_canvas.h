#pragma once
#include <engine/api.h>

#include <string>

#include "TextAlignments.h"
#include <engine/graphics/iDrawable.h>
#include <engine/graphics/color/color.h>
#include <engine/vectors/vector3.h>

class Font;
class TextInfo;
class MeshData;

class API TextRendererCanvas : public IDrawable
{
public:
	TextRendererCanvas();
	~TextRendererCanvas();

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	Vector3 position = Vector3(0, 0, 0);
	float size = 16;
	float lineSpacing = 0;
	float characterSpacing = 0;

	/**
	 * [Internal]
	 */
	int GetDrawPriority() const override;

	std::shared_ptr<Material> material = nullptr;
	HorizontalAlignment horizontalAlignment = H_Center;
	VerticalAlignment verticalAlignment = V_Center;

	/**
	* Set text color
	* @param color Color
	*/
	void SetColor(const Color& color)
	{
		this->color = color;
	}

	/**
	* Set order in layer
	* @param orderInLayer Order in layer
	*/
	void SetOrderInLayer(int orderInLayer);

	/**
	* Get order in layer
	*/
	int GetOrderInLayer() const
	{
		return orderInLayer;
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
	void SetFont(const std::shared_ptr<Font>& font);

private:
	void Draw() override;
	std::shared_ptr<Font> font;
	std::string text;
	Color color = Color();
	int orderInLayer = 0;
	TextInfo* textInfo = nullptr;
	bool isTextInfoDirty = true;
	std::shared_ptr<MeshData> mesh = nullptr;
};
