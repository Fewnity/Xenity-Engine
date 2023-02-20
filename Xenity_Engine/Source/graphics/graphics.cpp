#include "graphics.h"
#include "../asset_manager.h"

Camera* Graphics::usedCamera = nullptr;

/// <summary>
/// Draw all Drawable elements
/// </summary>
void Graphics::DrawAllDrawable()
{
	int drawableCount = AssetManager::GetDrawableCount();
	for (int i = 0; i < drawableCount; i++)
	{
		IDrawable * drawable = AssetManager::GetDrawable(i);
		if(drawable->invertedTriangles)
			glCullFace(GL_FRONT);
		else
			glCullFace(GL_BACK);

		drawable->Draw();
	}
}