#include "gizmo.h"

// Graphics
#include <engine/graphics/renderer/renderer.h>
#include <engine/graphics/2d_graphics/sprite_manager.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>

#include <engine/engine.h>
#include <engine/game_elements/transform.h>
#include <engine/asset_management/asset_manager.h>

Color Gizmo::color;

void Gizmo::Init()
{
	color = Color::CreateFromRGB(255, 255, 255);
}

void Gizmo::DrawLine(const Vector3& a, const Vector3& b)
{
	// Currently lines do not support shaders
	if (!Graphics::UseOpenGLFixedFunctions)
	{
		Engine::GetRenderer().UseShaderProgram(0);
		Graphics::currentShader = nullptr;
		Graphics::currentMaterial = nullptr;
	}

	RenderingSettings renderSettings = RenderingSettings();
	renderSettings.useBlend = true;
	renderSettings.useDepth = false;
	renderSettings.useLighting = false;
	renderSettings.useTexture = false;
	Engine::GetRenderer().DrawLine(a, b, color, renderSettings);
}

void Gizmo::DrawBillboard(const Vector3& position, const Vector2& scale, const std::shared_ptr<Texture>& texture, const Color& color)
{
	float distance = Vector3::Distance(position, Graphics::usedCamera.lock()->GetTransform()->GetPosition());
	float alpha = 1;
	if (distance <= 1.3f)
		alpha = distance - 0.3f;

	RGBA rgba = color.GetRGBA();
	SpriteManager::DrawSprite(position, Graphics::usedCamera.lock()->GetTransform()->GetRotation(), Vector3(0.2f), texture, Color::CreateFromRGBAFloat(rgba.r, rgba.g, rgba.b, alpha), AssetManager::unlitMaterial);
}

void Gizmo::SetColor(const Color& newColor)
{
	color = newColor;
}
