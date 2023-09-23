#include "billboard_renderer.h"
#include "../../../xenity.h"

#pragma region Constructors / Destructor

BillboardRenderer::BillboardRenderer()
{
	componentName = "BillboardRenderer";
	type = Draw_2D;
	AssetManager::AddReflection(this);
	material = Engine::standardMaterial;
}

std::unordered_map<std::string, ReflectionEntry> BillboardRenderer::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	Reflection::AddReflectionVariable(reflectedVariables, color, "color", true);
	Reflection::AddReflectionVariable(reflectedVariables, texture, "texture", true);
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

#pragma endregion

/// <summary>
/// Draw sprite
/// </summary>
void BillboardRenderer::Draw()
{
	if (auto gameObject = GetGameObject())
	{
		// Draw the sprite only if there is a texture and if the component/gameobject is active
		if (gameObject->GetLocalActive() && GetIsEnabled())
		{
			auto transform = GetTransform();
			SpriteManager::DrawSprite(transform->GetPosition(), transform->GetRotation() + Graphics::usedCamera.lock()->GetTransform()->GetRotation(), transform->GetScale(), texture, color, material);
		}
	}
}