#include "text_renderer.h"

#include "../ui/ui_manager.h"
#include "../asset_manager.h"

TextRenderer::TextRenderer()
{
	AssetManager::AddDrawable(this);
}

TextRenderer::~TextRenderer()
{
}

void TextRenderer::Draw()
{
	if (gameObject != nullptr)
	{
		UiManager::RenderText(*shader, text, gameObject->transform.GetPosition().x, gameObject->transform.GetPosition().y, 0, 0.1f * size, glm::vec3(color.x, color.y, color.z), font);
	}
}
