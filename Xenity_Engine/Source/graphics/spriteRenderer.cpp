#include "spriteRenderer.h"

#include "sprite_manager.h"
#include "../asset_manager.h"
#include <iostream>

SpriteRenderer::SpriteRenderer()
{
	AssetManager::AddDrawable(this);
}

void SpriteRenderer::Draw() {

	if (gameObject != nullptr && shader != nullptr && texture != nullptr) {
		SpriteManager::RenderSprite(gameObject->transform,
									width,
									height,
									texture, shader);
	}
}