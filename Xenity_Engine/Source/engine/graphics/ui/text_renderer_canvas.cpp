#include "text_renderer_canvas.h"

#include "../../../xenity.h"

#pragma region Constructors / Destructor

TextRendererCanvas::TextRendererCanvas()
{
	componentName = "Canvas Text Renderer";

	type = Draw_UI;
}

// TextRendererCanvas::TextRendererCanvas(Font *font, float size, Shader *shader)
// {
// 	this->font = font;
// 	this->size = size;
// 	this->shader = shader;
// }

TextRendererCanvas::~TextRendererCanvas()
{
}

int TextRendererCanvas::GetDrawPriority() const
{
	return 5000 + orderInLayer;
}

#pragma endregion

/// <summary>
/// Draw text
/// </summary>
void TextRendererCanvas::Draw()
{
	if (GetGameObject().lock()->GetLocalActive() && GetIsEnabled() /* && shader != nullptr*/)
	{
		TextManager::DrawText(text, horizontalAligment, verticalAlignment, GetTransform(), color, true);
		// UiManager::RenderTextCanvas(text, position.x, position.y, position.z, size, lineSpacing, color, font, horizontalAligment, verticalAlignment, *shader);
	}
}
