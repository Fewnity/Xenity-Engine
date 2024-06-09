// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "canvas.h"

#if defined(EDITOR)
#include <editor/gizmo.h>
#include <editor/ui/editor_ui.h>
#include <editor/ui/menus/game_menu.h>
#endif

#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/graphics/color/color.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/ui/window.h>
#include <engine/game_elements/rect_transform.h>

ReflectiveData Canvas::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, lastSize, "lastSize", true);
	return reflectedVariables;
}

void Canvas::OnReflectionUpdated()
{
}

int Canvas::GetDrawPriority() const
{
	return 0;
}

void Canvas::OnDisabled()
{
	Graphics::isRenderingBatchDirty = true;
}

void Canvas::OnEnabled()
{
	Graphics::isRenderingBatchDirty = true;
}

void Canvas::CreateRenderCommands(RenderBatch& renderBatch)
{
	RenderCommand command = RenderCommand();
	command.material = nullptr;
	command.drawable = this;
	command.subMesh = nullptr;
	command.transform = GetTransform();
	command.isEnabled = GetIsEnabled() && GetGameObject()->GetLocalActive();

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
	componentName = "Canvas";

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
		const Vector2Int windowsSize = std::dynamic_pointer_cast<GameMenu>(Editor::lastFocusedGameMenu.lock())->lastSize;
		aspect = (float)windowsSize.x / (float)windowsSize.y;
	}

	const float xOff = (-aspect * 5) + (GetTransform()->GetPosition().x * (aspect * 10));
	const float yOff = (-1 * 5) + (GetTransform()->GetPosition().y * (1 * 10));
	const Vector3 pos = Vector3(xOff, -yOff, 1); // Z 1 to avoid issue with near clipping plane

	const Color lineColor = Color::CreateFromRGBAFloat(1, 1, 1, 1);
	Gizmo::SetColor(lineColor);

	Engine::GetRenderer().SetCameraPosition(*Graphics::usedCamera);
	Gizmo::DrawLine(Vector3(xOff, yOff, 0) * -1, Vector3(xOff, -yOff, 0) * -1);
	Gizmo::DrawLine(Vector3(-xOff, yOff, 0) * -1, Vector3(-xOff, -yOff, 0) * -1);

	Gizmo::DrawLine(Vector3(xOff, yOff, 0) * -1, Vector3(-xOff, yOff, 0) * -1);
	Gizmo::DrawLine(Vector3(xOff, -yOff, 0) * -1, Vector3(-xOff, -yOff, 0) * -1);
#endif
}