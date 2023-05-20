#include "text_renderer_canvas.h"

#include "../../../xenity.h"

#pragma region Constructors / Destructor

TextRendererCanvas::TextRendererCanvas()
{
	componentName = "Canvas Text Renderer";
}

TextRendererCanvas::TextRendererCanvas(Font* font, float size, Shader* shader)
{
	this->font = font;
	this->size = size;
	this->shader = shader;
}

TextRendererCanvas::~TextRendererCanvas()
{
}

int TextRendererCanvas::GetDrawPriority() const
{
	return orderInLayer;
}

#pragma endregion

/// <summary>
/// Draw text
/// </summary>
void TextRendererCanvas::Draw()
{
	if (GetGameObject()->GetLocalActive() && GetIsEnabled() && shader != nullptr)
	{
		UiManager::RenderTextCanvas(text, position.x, position.y, position.z, size, lineSpacing, color, font, horizontalAligment, verticalAlignment, *shader);
	}
}
