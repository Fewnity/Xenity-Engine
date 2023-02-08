#include "graphics.h"
#include "../asset_manager.h"

Camera* Graphics::usedCamera = nullptr;

void Graphics::DrawAllDrawable()
{
	for (IDrawable* drawable : AssetManager::drawables)
	{
		drawable->Draw();
	}
}