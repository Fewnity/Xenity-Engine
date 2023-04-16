#include "graphics.h"
#include "../asset_manager.h"
#include "sprite_manager.h"

Camera* Graphics::usedCamera = nullptr;
int Graphics::usedShaderProgram = -1;
Material* Graphics::usedMaterial = nullptr;

/// <summary>
/// Draw all Drawable elements
/// </summary>
void Graphics::DrawAllDrawable()
{
	//glEnable(GL_DEPTH_TEST);

	glCullFace(GL_BACK);
	int drawableCount = AssetManager::GetDrawableCount();
	for (int i = 0; i < drawableCount; i++)
	{
		IDrawable* drawable = AssetManager::GetDrawable(i);

		drawable->Draw();
	}
}