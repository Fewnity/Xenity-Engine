#include "graphics.h"
#include "../asset_manager.h"

Camera* Graphics::usedCamera = nullptr;

void Graphics::DrawAllDrawable()
{
	int drawableCount = AssetManager::GetDrawableCount();
	for (int i = 0; i < drawableCount; i++)
	{
		AssetManager::GetDrawable(i)->Draw();
	}
}