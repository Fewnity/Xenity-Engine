#include "text_renderer_canvas.h"

#include "../../../xenity.h"

#pragma region Constructors / Destructor

TextRendererCanvas::TextRendererCanvas()
{
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

int TextRendererCanvas::GetDrawPriority()
{
	return gameObject->transform.GetPosition().z + layerOrder;
}

#pragma endregion

/// <summary>
/// Draw text
/// </summary>
void TextRendererCanvas::Draw()
{
	if (gameObject->GetLocalActive() && GetIsEnabled() && shader != nullptr)
	{
		UiManager::RenderTextCanvas(text, position.x, position.y, position.z, size, lineSpacing, color, font, horizontalAligment, verticalAlignment, *shader);
	}
}
