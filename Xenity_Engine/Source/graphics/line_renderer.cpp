#include "line_renderer.h"
#include "sprite_manager.h"
#include "texture.h"
#include "../asset_manager.h"
#include <iostream>

#pragma region Constructors / Destructor

LineRenderer::LineRenderer()
{
}

LineRenderer::LineRenderer(float width, Material* material)
{
	this->width = width;
	this->material = material;
}

LineRenderer::~LineRenderer()
{
}

int LineRenderer::GetDrawPriority()
{
	return gameObject->transform.GetPosition().z;
}

#pragma endregion

/// <summary>
/// Draw sprite
/// </summary>
void LineRenderer::Draw()
{
	if (gameObject != nullptr && gameObject->GetLocalActive() && GetIsEnabled() && material != nullptr)
	{
		SpriteManager::Render2DLine(startPosition, endPosition, width, color, material);
	}
}