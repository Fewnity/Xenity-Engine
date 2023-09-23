#include "sprite_renderer.h"
#include "../../../xenity.h"

#pragma region Constructors / Destructor

SpriteRenderer::SpriteRenderer()
{
	componentName = "SpriteRenderer";
	type = Draw_2D;
	AssetManager::AddReflection(this);
	material = Engine::standardMaterial;
}

std::unordered_map<std::string, ReflectionEntry> SpriteRenderer::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	Reflection::AddReflectionVariable(reflectedVariables, color, "color", true);
	Reflection::AddReflectionVariable(reflectedVariables, texture, "texture", true);
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
			SpriteManager::DrawSprite(GetTransform(), texture, color, material);
		}
	}
}