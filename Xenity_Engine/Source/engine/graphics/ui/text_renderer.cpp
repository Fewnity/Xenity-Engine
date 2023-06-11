#include "text_renderer.h"

#include "../../../xenity.h"

#pragma region Constructors / Destructor

TextRenderer::TextRenderer()
{
	componentName = "Text Renderer";
}

// TextRenderer::TextRenderer(Font *font, float size, Shader *shader)
// {
// 	this->font = font;
// 	this->size = size;
// 	this->shader = shader;
// }

TextRenderer::~TextRenderer()
{
}

int TextRenderer::GetDrawPriority() const
{
	return orderInLayer;
}

#pragma endregion

/// <summary>
/// Draw text
/// </summary>
void TextRenderer::Draw()
{
	if (GetGameObject()->GetLocalActive() && GetIsEnabled() /*&& shader != nullptr*/)
	{
		TextManager::DrawText(text, horizontalAligment, verticalAlignment, GetTransform(), color, false);
		// UiManager::RenderText(text, GetTransform()->GetPosition().x, GetTransform()->GetPosition().y, GetTransform()->GetRotation().z, size, lineSpacing, color, font, horizontalAligment, verticalAlignment, *shader);
	}
}
