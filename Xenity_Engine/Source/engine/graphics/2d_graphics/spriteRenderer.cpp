#include "spriteRenderer.h"
#include "../../../xenity.h"

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

int SpriteRenderer::GetDrawPriority()
{
	return gameObject->transform.GetPosition().z + layerOrder;
}

#pragma endregion

/// <summary>
/// Draw sprite
/// </summary>
void SpriteRenderer::Draw() 
{
	if (gameObject != nullptr && gameObject->GetLocalActive() && GetIsEnabled() && material != nullptr && texture != nullptr)
	{
		SpriteManager::RenderSprite(gameObject->transform.transformationMatrix,
			color, texture, material);
	}
}