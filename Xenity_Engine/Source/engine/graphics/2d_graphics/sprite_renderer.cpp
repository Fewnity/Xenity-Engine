#include "sprite_renderer.h"
#include "../../../xenity.h"

#pragma region Constructors / Destructor

SpriteRenderer::SpriteRenderer()
{
	componentName = "Sprite Renderer";
}

SpriteRenderer::~SpriteRenderer()
{
}

int SpriteRenderer::GetDrawPriority() const
{
	return orderInLayer;
}

#pragma endregion

/// <summary>
/// Draw sprite
/// </summary>
void SpriteRenderer::Draw()
{
	if (GetGameObject()->GetLocalActive() && GetIsEnabled() /*&& material != nullptr*/ && texture != nullptr)
	{
		// SpriteManager::RenderSprite(GetTransform()->transformationMatrix,
		// 							color, texture, material);
		SpriteManager::DrawSprite(GetTransform()->GetPosition(), GetTransform()->GetRotation(), GetTransform()->GetScale(), texture);
	}
}