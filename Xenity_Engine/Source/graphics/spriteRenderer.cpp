#include "spriteRenderer.h"

#include "sprite_manager.h"
#include "../asset_manager.h"
#include <iostream>

SpriteRenderer::SpriteRenderer()
{
	//Add the sprite renderer to the drawable list
	AssetManager::AddDrawable(this);
}

/// <summary>
/// Draw sprite
/// </summary>
void SpriteRenderer::Draw() {

	if (gameObject != nullptr && shader != nullptr && texture != nullptr) {
		SpriteManager::RenderSprite(gameObject->transform,
									width,
									height,
									texture, shader);
	}
}