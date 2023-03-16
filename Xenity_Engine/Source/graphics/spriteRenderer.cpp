#include "spriteRenderer.h"
#include "sprite_manager.h"
#include "texture.h"
#include "../asset_manager.h"
#include <iostream>

#pragma region Constructors / Destructor

SpriteRenderer::SpriteRenderer()
{
}

SpriteRenderer::SpriteRenderer(const Texture* texture, Material* material)
{
	this->texture = texture;
	this->material = material;
}

SpriteRenderer::~SpriteRenderer()
{
}

#pragma endregion

/// <summary>
/// Draw sprite
/// </summary>
void SpriteRenderer::Draw() {
	if (gameObject != nullptr && material != nullptr && texture != nullptr) 
	{
		SpriteManager::RenderSprite(gameObject->transform.transformationMatrix,
			texture, material);
		/*SpriteManager::RenderSprite(gameObject->transform,
									texture->GetWidth(),
									texture->GetHeight(),
									texture, material);*/
	}
}