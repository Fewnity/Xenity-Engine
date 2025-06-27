// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "rect_transform.h"

#if defined(EDITOR)
#include <editor/editor.h>
#include <editor/ui/menus/basic/game_menu.h>
#endif

#include <engine/asset_management/asset_manager.h>
#include <engine/graphics/ui/canvas.h>
#include <engine/game_elements/transform.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>
#include <engine/debug/stack_debug_object.h>

ReflectiveData RectTransform::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, position, "position", true);
	//Reflective::AddVariable(reflectedVariables, anchors, "anchors", true);
	return reflectedVariables;
}

void RectTransform::OnReflectionUpdated()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
}

void RectTransform::UpdatePosition(const Canvas& canvas) 
{
	float aspect = Graphics::usedCamera->GetAspectRatio();
#if defined(EDITOR)
	if (Editor::s_lastFocusedGameMenu.lock() != nullptr)
	{
		const Vector2 windowsSize = std::dynamic_pointer_cast<GameMenu>(Editor::s_lastFocusedGameMenu.lock())->lastSize;
		aspect = windowsSize.x / windowsSize.y;
	}
#endif
	const float xOff = (-aspect * 5) + (position.x * (aspect * 10));
	const float yOff = (-1 * 5) + (position.y * (1 * 10));
	const Vector3 newPos = Vector3(xOff, -yOff, 0);
	GetTransformRaw()->SetLocalPosition(newPos);
}

void RectTransform::UpdatePosition(const RectTransform& rect)
{

}