// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "rect_transform.h"

#if defined(EDITOR)
#include <editor/editor.h>
#include <editor/ui/menus/game_menu.h>
#endif

#include <engine/asset_management/asset_manager.h>
#include <engine/graphics/ui/canvas.h>
#include <engine/game_elements/transform.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>

RectTransform::RectTransform()
{
	m_componentName = "RectTransform";

	AssetManager::AddReflection(this);
}

RectTransform::~RectTransform()
{
	AssetManager::RemoveReflection(this);
}

ReflectiveData RectTransform::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, position, "position", true);
	Reflective::AddVariable(reflectedVariables, anchors, "anchors", true);
	return reflectedVariables;
}

void RectTransform::OnReflectionUpdated()
{
}

void RectTransform::UpdatePosition(const std::shared_ptr <Canvas>& canvas) 
{
	XASSERT(canvas != nullptr, "[RectTransform::UpdatePosition] canvas is nullptr");

	float aspect = Graphics::usedCamera->GetAspectRatio();
#if defined(EDITOR)
	if (Editor::lastFocusedGameMenu.lock() != nullptr) 
	{
		Vector2 windowsSize = std::dynamic_pointer_cast<GameMenu>(Editor::lastFocusedGameMenu.lock())->lastSize;
		aspect = windowsSize.x / windowsSize.y;
	}
#endif
	const float xOff = (-aspect * 5) + (position.x * (aspect * 10));
	const float yOff = (-1 * 5) + (position.y * (1 * 10));
	const Vector3 newPos = Vector3(xOff, -yOff, 0); // Z 1 to avoid issue with near clipping plane
	GetTransformRaw()->SetLocalPosition(newPos);
}

void RectTransform::UpdatePosition(const std::shared_ptr <RectTransform>& rect)
{

}