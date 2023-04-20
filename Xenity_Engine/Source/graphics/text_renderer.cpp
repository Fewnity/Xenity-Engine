#include "text_renderer.h"

#include "../ui/ui_manager.h"
#include "../asset_manager.h"

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
	return gameObject->transform.GetPosition().z;
}

#pragma endregion

/// <summary>
/// Draw text
/// </summary>
void TextRenderer::Draw()
{
	if (gameObject != nullptr && gameObject->GetLocalActive() && GetIsEnabled() && shader != nullptr)
	{
		UiManager::RenderText(text, gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y, gameObject->transform.GetRotation().z, size, lineSpacing, color, font, horizontalAligment, V_Center, *shader);
	}
}
