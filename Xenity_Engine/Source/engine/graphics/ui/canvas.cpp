// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "canvas.h"

#if defined(EDITOR)
#include <editor/rendering/gizmo.h>
#include <editor/ui/menus/basic/game_menu.h>
#include <editor/editor.h>
#endif

#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/graphics/color/color.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/ui/window.h>
#include <engine/game_elements/rect_transform.h>
#include <engine/engine.h>
#include <engine/debug/stack_debug_object.h>

ReflectiveData Canvas::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, lastSize, "lastSize", true);
	return reflectedVariables;
}

void Canvas::OnReflectionUpdated()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
}

void Canvas::OnDisabled()
{
	Graphics::s_isRenderingBatchDirty = true;
}

void Canvas::OnEnabled()
{
	Graphics::s_isRenderingBatchDirty = true;
}

void Canvas::CreateRenderCommands(RenderBatch& renderBatch)
{
	RenderCommand command = RenderCommand();
	command.drawable = this;
	command.transform = GetTransformRaw();
	command.isEnabled = IsEnabled() && GetGameObject()->IsLocalActive();

	renderBatch.uiCommands.push_back(command);
	renderBatch.uiCommandIndex++;
}

void Canvas::DrawCommand(const RenderCommand& renderCommand)
{
	/*if (Window::GetWidth() != lastSize.x || Window::GetHeight() != lastSize.y)
	{*/
	lastSize = Vector2Int(Window::GetWidth(), Window::GetHeight());

	for (int i = 0; i < GetGameObject()->GetChildrenCount(); i++)
	{
		std::shared_ptr<RectTransform> rect = GetGameObject()->GetChildren()[i].lock()->GetComponent<RectTransform>();
		if (rect)
		{
			rect->UpdatePosition(std::dynamic_pointer_cast<Canvas>(shared_from_this()));
		}
	}
	//}
}

Canvas::Canvas()
{
	AssetManager::AddReflection(this);
}

Canvas::~Canvas()
{
	AssetManager::RemoveReflection(this);
}

void Canvas::OnDrawGizmos()
{
#if defined(EDITOR)
	float aspect = Graphics::usedCamera->GetAspectRatio();

	if (Editor::lastFocusedGameMenu.lock() != nullptr)
	{
		const Vector2 windowsSize = std::dynamic_pointer_cast<GameMenu>(Editor::lastFocusedGameMenu.lock())->lastSize;
		aspect = windowsSize.x / windowsSize.y;
	}

	const float xOff = (-aspect * 5) + (GetTransformRaw()->GetPosition().x * (aspect * 10));
	const float yOff = (-1 * 5) + (GetTransformRaw()->GetPosition().y * (1 * 10));
	const Vector3 pos = Vector3(xOff, -yOff, 1); // Z 1 to avoid issue with near clipping plane

	const Color lineColor = Color::CreateFromRGBAFloat(1, 1, 1, 1);
	Gizmo::SetColor(lineColor);

	Gizmo::DrawLine(Vector3(xOff, yOff, 0) * -1, Vector3(xOff, -yOff, 0) * -1);
	Gizmo::DrawLine(Vector3(-xOff, yOff, 0) * -1, Vector3(-xOff, -yOff, 0) * -1);

	Gizmo::DrawLine(Vector3(xOff, yOff, 0) * -1, Vector3(-xOff, yOff, 0) * -1);
	Gizmo::DrawLine(Vector3(xOff, -yOff, 0) * -1, Vector3(-xOff, -yOff, 0) * -1);
#endif
}