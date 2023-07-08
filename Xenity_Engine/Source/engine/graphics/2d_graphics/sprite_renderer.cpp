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
	if (auto gameObject = GetGameObject())
	{
		// Draw the sprite only if there is a texture and if the component/gameobject is active
		if (gameObject->GetLocalActive() && GetIsEnabled())
		{
			auto transform = GetTransform();
			SpriteManager::DrawSprite(transform->GetPosition(), transform->GetRotation(), transform->GetScale(), texture, color, transform);
		}
	}
}