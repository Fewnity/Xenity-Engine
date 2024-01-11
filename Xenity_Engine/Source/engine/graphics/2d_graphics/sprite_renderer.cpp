#include "sprite_renderer.h"

#include <engine/graphics/graphics.h>
#include "sprite_manager.h"

#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>


#pragma region Constructors / Destructor

SpriteRenderer::SpriteRenderer()
{
	componentName = "SpriteRenderer";
	type = Draw_2D;
	AssetManager::AddReflection(this);
	material = AssetManager::unlitMaterial;
}

ReflectiveData SpriteRenderer::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, color, "color", true);
	Reflective::AddVariable(reflectedVariables, texture, "texture", true);
	return reflectedVariables;
}

SpriteRenderer::~SpriteRenderer()
{
	AssetManager::RemoveReflection(this);
}

int SpriteRenderer::GetDrawPriority() const
{
	return orderInLayer;
}

void SpriteRenderer::SetOrderInLayer(int orderInLayer)
{
	this->orderInLayer = orderInLayer;
	Graphics::SetDrawOrderListAsDirty();
}

#pragma endregion

/// <summary>
/// Draw sprite
/// </summary>
void SpriteRenderer::Draw()
{
	if (const std::shared_ptr<GameObject> gameObject = GetGameObject())
	{
		// Draw the sprite only if there is a texture and if the component/gameobject is active
		if (gameObject->GetLocalActive() && GetIsEnabled())
		{
			SpriteManager::DrawSprite(GetTransform(), texture, color, material);
		}
	}
}