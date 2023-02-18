#include "spriteRenderer.h"
#include "sprite_manager.h"
#include "texture.h"
#include "../asset_manager.h"
#include <iostream>

#pragma region Constructors / Destructor

SpriteRenderer::SpriteRenderer()
{
}

SpriteRenderer::SpriteRenderer(const Texture* texture, Shader* shader)
{
	this->texture = texture;
	this->shader = shader;
}

SpriteRenderer::~SpriteRenderer()
{
}

#pragma endregion

/// <summary>
/// Draw sprite
/// </summary>
void SpriteRenderer::Draw() {

	if (gameObject != nullptr && shader != nullptr && texture != nullptr) {
		SpriteManager::RenderSprite(gameObject->transform,
									texture->GetWidth(),
									texture->GetHeight(),
									texture, shader);
	}
}