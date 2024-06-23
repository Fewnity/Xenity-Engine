// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "gizmo.h"

// Graphics
#include <engine/graphics/renderer/renderer.h>
#include <engine/graphics/2d_graphics/sprite_manager.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>
#include <engine/graphics/material.h>

#include <engine/engine.h>
#include <engine/game_elements/transform.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/tools/math.h>

Color Gizmo::color;

void Gizmo::Init()
{
	color = Color::CreateFromRGB(255, 255, 255);
}

void Gizmo::DrawLine(Vector3 a, Vector3 b)
{
	a.x = -a.x;
	b.x = -b.x;

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
	const float distance = Vector3::Distance(position, Graphics::usedCamera->GetTransform()->GetPosition());
	float alpha = 1;
	if (distance <= 1.3f)
		alpha = distance - 0.3f;

	const RGBA& rgba = color.GetRGBA();
	SpriteManager::DrawSprite(position, Graphics::usedCamera->GetTransform()->GetRotation(), Vector3(0.2f), Color::CreateFromRGBAFloat(rgba.r, rgba.g, rgba.b, alpha), AssetManager::unlitMaterial, texture);
}

void Gizmo::DrawSphere(const Vector3& position, const float radius)
{
	if (radius == 0)
		return;

	const int steps = 30;
	const float angleStep = 360.0f / steps;
	for (int i = 0; i < steps; i++)
	{
		// Draw sphere with lines
		{
			Vector3 pos0 = position;
			pos0.x += radius * cos((angleStep * i) * Math::PI / 180.0f);
			pos0.z += radius * sin((angleStep * i) * Math::PI / 180.0f);

			Vector3 pos1 = position;
			pos1.x += radius * cos((angleStep * (i + 1)) * Math::PI / 180.0f);
			pos1.z += radius * sin((angleStep * (i + 1)) * Math::PI / 180.0f);

			Gizmo::DrawLine(pos0, pos1);
		}

		{
			Vector3 pos0 = position;
			pos0.x += radius * cos((angleStep * i) * Math::PI / 180.0f);
			pos0.y += radius * sin((angleStep * i) * Math::PI / 180.0f);

			Vector3 pos1 = position;
			pos1.x += radius * cos((angleStep * (i + 1)) * Math::PI / 180.0f);
			pos1.y += radius * sin((angleStep * (i + 1)) * Math::PI / 180.0f);

			Gizmo::DrawLine(pos0, pos1);
		}

		{
			Vector3 pos0 = position;
			pos0.y += radius * cos((angleStep * i) * Math::PI / 180.0f);
			pos0.z += radius * sin((angleStep * i) * Math::PI / 180.0f);

			Vector3 pos1 = position;
			pos1.y += radius * cos((angleStep * (i + 1)) * Math::PI / 180.0f);
			pos1.z += radius * sin((angleStep * (i + 1)) * Math::PI / 180.0f);

			Gizmo::DrawLine(pos0, pos1);
		}
	}
}

void Gizmo::SetColor(const Color& newColor)
{
	color = newColor;
}
