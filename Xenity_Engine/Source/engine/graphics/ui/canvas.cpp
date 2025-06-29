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
#include <engine/graphics/ui/button.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/ui/window.h>
#include <engine/game_elements/rect_transform.h>
#include <engine/engine.h>
#include <engine/debug/stack_debug_object.h>
#include <engine/inputs/input_system.h>

ReflectiveData Canvas::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

void Canvas::OnReflectionUpdated()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
}

void Canvas::UpdateButtons(const std::shared_ptr<GameObject>& gameObject)
{
	std::vector<std::shared_ptr<GameObject>> children;
	for (size_t i = 0; i < gameObject->GetChildrenCount(); i++)
	{
		children.push_back(gameObject->GetChild(static_cast<int>(i)).lock());
	}
	for (const auto& child : children)
	{
		std::shared_ptr<Button> button = child->GetComponent<Button>();
		if (button)
		{
			button->CheckClick(*this);
		}
		UpdateButtons(child);
	}
}

void Canvas::Update()
{
	bool needCheck = InputSystem::GetKeyDown(KeyCode::MOUSE_LEFT);
	if (!needCheck)
	{
		const int touchScreenCount = InputSystem::GetTouchScreenCount();
		if (touchScreenCount != 0)
		{
			const int touchScreenCount = InputSystem::GetTouchCount(0);
			for (int touchIndex = 0; touchIndex < touchScreenCount; touchIndex++)
			{
				if (InputSystem::GetTouch(touchIndex, 0).pressed)
				{
					needCheck = true;
					break;
				}
			}
		}
	}

	if (needCheck)
	{
		UpdateButtons(GetGameObject());
	}
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
#if defined(EDITOR)
	if (Editor::s_lastFocusedGameMenu.lock() != nullptr)
	{
		const Vector2 windowsSize = std::dynamic_pointer_cast<GameMenu>(Editor::s_lastFocusedGameMenu.lock())->lastSize;
		lastSize = Vector2Int(static_cast<int>(windowsSize.x), static_cast<int>(windowsSize.y));
	}
	else 
	{
		lastSize = Vector2Int(1920, 1080);
	}
#else
	lastSize = Vector2Int(Window::GetWidth(), Window::GetHeight());
#endif

	for (uint32_t i = 0; i < GetGameObject()->GetChildrenCount(); i++)
	{
		std::shared_ptr<RectTransform> rect = GetGameObject()->GetChildren()[i].lock()->GetComponent<RectTransform>();
		if (rect)
		{
			rect->UpdatePosition(*this);
		}
	}
}

void Canvas::OnDrawGizmos()
{
#if defined(EDITOR)
	float aspect = Graphics::usedCamera->GetAspectRatio();

	if (Editor::s_lastFocusedGameMenu.lock() != nullptr)
	{
		const Vector2 windowsSize = std::dynamic_pointer_cast<GameMenu>(Editor::s_lastFocusedGameMenu.lock())->lastSize;
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