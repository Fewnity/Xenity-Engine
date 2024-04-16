#pragma once
#include <engine/api.h>

#include "TextAlignments.h"

#include <engine/graphics/iDrawable.h>
#include <engine/graphics/color/color.h>

#include <string>

class Font;
class TextInfo;
class MeshData;

class API TextRenderer : public IDrawable
{
public:
	TextRenderer();
	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	~TextRenderer();

	/**
	 * [Internal]
	 */
	int GetDrawPriority() const override;

	std::shared_ptr<Material> material = nullptr;

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
	void SetFont(const std::shared_ptr<Font>& font);

private:

	/**
	* Called when the component is disabled
	*/
	void OnDisabled() override;

	/**
	* Called when the component is enabled
	*/
	void OnEnabled() override;

	/**
	* Create the render commands
	*/
	void CreateRenderCommands(RenderBatch& renderBatch) override;

	/**
	* Draw the command
	*/
	void DrawCommand(const RenderCommand& renderCommand) override;

	TextInfo* textInfo = nullptr;
	std::shared_ptr <MeshData> mesh = nullptr;
	std::shared_ptr<Font> font;
	std::string text;
	Color color = Color();
	int orderInLayer = 0;
	float fontSize = 1;

public:
	float size = 16;
	float lineSpacing = 0;
	float characterSpacing = 0;
	HorizontalAlignment horizontalAlignment = HorizontalAlignment::Center;
	VerticalAlignment verticalAlignment = VerticalAlignment::Center;

private:
	bool isTextInfoDirty = true;
};
