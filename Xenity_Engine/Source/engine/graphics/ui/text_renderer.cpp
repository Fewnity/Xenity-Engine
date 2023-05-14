#include "text_renderer.h"

#include "../../../xenity.h"

#pragma region Constructors / Destructor

TextRenderer::TextRenderer()
{
}

TextRenderer::TextRenderer(Font* font, float size, Shader* shader)
{
	this->font = font;
	this->size = size;
	this->shader = shader;
}

TextRenderer::~TextRenderer()
{
}

int TextRenderer::GetDrawPriority()
{
	return orderInLayer;
}

#pragma endregion

/// <summary>
/// Draw text
/// </summary>
void TextRenderer::Draw()
{
	if (GetGameObject()->GetLocalActive() && GetIsEnabled() && shader != nullptr)
	{
		UiManager::RenderText(text, GetGameObject()->transform.GetPosition().x, GetGameObject()->transform.GetPosition().y, GetGameObject()->transform.GetRotation().z, size, lineSpacing, color, font, horizontalAligment, verticalAlignment, *shader);
	}
}
