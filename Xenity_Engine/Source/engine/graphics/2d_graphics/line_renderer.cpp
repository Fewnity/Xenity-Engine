#include "line_renderer.h"
#include "../../../xenity.h"

#pragma region Constructors / Destructor

LineRenderer::LineRenderer()
{
	componentName = "Line Renderer";
}

LineRenderer::~LineRenderer()
{
}

int LineRenderer::GetDrawPriority()
{
	return orderInLayer;
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