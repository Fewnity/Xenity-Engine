// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "gizmo.h"

#include <glm/gtx/quaternion.hpp>

// Graphics
#include <engine/graphics/renderer/renderer.h>
#include <engine/graphics/2d_graphics/sprite_manager.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>

#include <engine/engine.h>
#include <engine/game_elements/transform.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/tools/math.h>
#include <engine/debug/stack_debug_object.h>

Color Gizmo::color;

void Gizmo::Init()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	color = Color::CreateFromRGB(255, 255, 255);
}

void Gizmo::DrawLine(const Vector3& a, const Vector3& b)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	Vector3 aCopy = a;
	Vector3 bCopy = b;
	aCopy.x = -aCopy.x;
	bCopy.x = -bCopy.x;

	// Currently lines do not support shaders
	if constexpr (!s_UseOpenGLFixedFunctions)
	{
		Engine::GetRenderer().UseShaderProgram(0);
		Graphics::s_currentShader = nullptr;
		Graphics::s_currentMaterial = nullptr;
	}

	RenderingSettings renderSettings = RenderingSettings();
	renderSettings.renderingMode = MaterialRenderingMode::Transparent;
	renderSettings.useDepth = false;
	renderSettings.useLighting = false;
	renderSettings.useTexture = false;
	Engine::GetRenderer().DrawLine(aCopy, bCopy, color, renderSettings);
}

void Gizmo::DrawBillboard(const Vector3& position, const Vector2& scale, const std::shared_ptr<Texture>& texture, const Color& color)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	const float distance = Vector3::Distance(position, Graphics::usedCamera->GetTransform()->GetPosition());
	float alpha = 1;
	if (distance <= 1.3f)
		alpha = distance - 0.3f;
	if (alpha < 0)
		alpha = 0;

	const RGBA& rgba = color.GetRGBA();
	SpriteManager::DrawSprite(position, Graphics::usedCamera->GetTransform()->GetRotation(), Vector3(0.2f), Color::CreateFromRGBAFloat(rgba.r, rgba.g, rgba.b, alpha), *AssetManager::unlitMaterial, texture.get());
}

void Gizmo::DrawSphere(const Vector3& position, const Quaternion& rotation, const float radius)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	if (radius == 0)
		return;

	glm::quat quat = glm::quat(rotation.w, rotation.x, rotation.y, rotation.z);


	const int steps = 30;
	const float angleStep = 360.0f / steps;
	Vector3 pos0;
	Vector3 pos1;
	for (int i = 0; i < steps; i++)
	{
		// Draw sphere with lines
		{
			pos0.x = radius * std::cos((angleStep * i) * Math::PI / 180.0f);
			pos0.y = 0;
			pos0.z = radius * std::sin((angleStep * i) * Math::PI / 180.0f);

			pos1.x = radius * std::cos((angleStep * (i + 1)) * Math::PI / 180.0f);
			pos1.y = 0;
			pos1.z = radius * std::sin((angleStep * (i + 1)) * Math::PI / 180.0f);

			Gizmo::DrawLine((rotation * Vector3(pos0)) + position, (rotation * Vector3(pos1)) + position);
		}

		{
			pos0.x = radius * std::cos((angleStep * i) * Math::PI / 180.0f);
			pos0.y = radius * std::sin((angleStep * i) * Math::PI / 180.0f);
			pos0.z = 0;

			pos1.x = radius * std::cos((angleStep * (i + 1)) * Math::PI / 180.0f);
			pos1.y = radius * std::sin((angleStep * (i + 1)) * Math::PI / 180.0f);
			pos1.z = 0;

			Gizmo::DrawLine((rotation * Vector3(pos0)) + position, (rotation * Vector3(pos1)) + position);
		}

		{
			pos0.x = 0;
			pos0.y = radius * std::cos((angleStep * i) * Math::PI / 180.0f);
			pos0.z = radius * std::sin((angleStep * i) * Math::PI / 180.0f);

			pos1.x = 0;
			pos1.y = radius * std::cos((angleStep * (i + 1)) * Math::PI / 180.0f);
			pos1.z = radius * std::sin((angleStep * (i + 1)) * Math::PI / 180.0f);

			Gizmo::DrawLine((rotation * Vector3(pos0)) + position, (rotation * Vector3(pos1)) + position);
		}
	}
}

void Gizmo::SetColor(const Color& newColor)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	color = newColor;
}
