#include "gizmo.h"
#include "../xenity.h"
#include "../engine/graphics/renderer/renderer.h"

Color Gizmo::color;

void Gizmo::Init()
{
	color = Color::CreateFromRGB(255, 255, 255);
}

void Gizmo::DrawLine(Vector3 a, Vector3 b)
{
	RenderingSettings renderSettings = RenderingSettings();
	renderSettings.useBlend = true;
	renderSettings.useDepth = false;
	renderSettings.useLighting = false;
	renderSettings.useTexture = false;
	Engine::renderer->DrawLine(a, b, color, renderSettings);
}

void Gizmo::SetColor(Color newColor)
{
	color = newColor;
}
