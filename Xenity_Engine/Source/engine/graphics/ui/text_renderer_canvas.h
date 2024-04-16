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

	/**
	 * [Internal]
	 */
	int GetDrawPriority() const override;

	std::shared_ptr<Material> material = nullptr;


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

	std::shared_ptr<MeshData> mesh = nullptr;
	std::shared_ptr<Font> font;
	TextInfo* textInfo = nullptr;
	std::string text;
	Color color = Color();
	int orderInLayer = 0;
	float fontSize = 1;

public:
	HorizontalAlignment horizontalAlignment = HorizontalAlignment::Center;
	VerticalAlignment verticalAlignment = VerticalAlignment::Center;
	float size = 16;
	float lineSpacing = 0;
	float characterSpacing = 0;

private:
	bool isTextInfoDirty = true;
};
