#include "billboard_renderer.h"

#include <engine/graphics/graphics.h>
#include "sprite_manager.h"
#include <engine/graphics/camera.h>

#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>

#pragma region Constructors / Destructor

BillboardRenderer::BillboardRenderer()
{
	componentName = "BillboardRenderer";
	type = IDrawableTypes::Draw_2D;
	AssetManager::AddReflection(this);
	material = AssetManager::standardMaterial;
}

ReflectiveData BillboardRenderer::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, color, "color", true);
	Reflective::AddVariable(reflectedVariables, texture, "texture", true);
	return reflectedVariables;
}

BillboardRenderer::~BillboardRenderer()
{
	AssetManager::RemoveReflection(this);
}

int BillboardRenderer::GetDrawPriority() const
{
	return orderInLayer;
}

void BillboardRenderer::SetOrderInLayer(int orderInLayer)
{
		this->orderInLayer = orderInLayer;
		Graphics::SetDrawOrderListAsDirty();
}

#pragma endregion

/// <summary>
/// Draw sprite
/// </summary>
void BillboardRenderer::Draw()
{
	if (const std::shared_ptr<GameObject> gameObject = GetGameObject())
	{
		// Draw the sprite only if there is a texture and if the component/gameobject is active
		if (gameObject->GetLocalActive() && GetIsEnabled())
		{
			const std::shared_ptr<Transform> transform = GetTransform();
			SpriteManager::DrawSprite(transform->GetPosition(), transform->GetRotation() + Graphics::usedCamera.lock()->GetTransform()->GetRotation(), transform->GetScale(), texture, color, material);
		}
	}
}