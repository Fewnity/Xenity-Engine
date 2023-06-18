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
	// Draw the sprite only if there is a texture and if the component/gameobject is active
	if (GetGameObject().lock()->GetLocalActive() && GetIsEnabled() && texture != nullptr)
	{
		SpriteManager::DrawSprite(GetTransform().lock()->GetPosition(), GetTransform().lock()->GetRotation(), GetTransform().lock()->GetScale(), texture, color);
	}
}