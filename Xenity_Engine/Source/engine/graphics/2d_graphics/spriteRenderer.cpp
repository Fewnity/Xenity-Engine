#include "spriteRenderer.h"
#include "../../../xenity.h"

#pragma region Constructors / Destructor

SpriteRenderer::SpriteRenderer()
{
	componentName = "Sprite Renderer";
}

SpriteRenderer::~SpriteRenderer()
{
}

int SpriteRenderer::GetDrawPriority()
{
	return orderInLayer;
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