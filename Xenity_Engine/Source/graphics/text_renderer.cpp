#include "text_renderer.h"

#include "../ui/ui_manager.h"
#include "../asset_manager.h"

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

void TextRenderer::Draw()
{
	if (gameObject != nullptr)
	{
		UiManager::RenderText(*shader, text, gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y, gameObject->transform.GetRotation().z, 0.1f * size, color, font);
	}
}
