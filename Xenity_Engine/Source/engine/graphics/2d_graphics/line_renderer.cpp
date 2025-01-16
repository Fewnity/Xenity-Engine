// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(ENABLE_EXPERIMENTAL_FEATURES)

#include "line_renderer.h"

#if defined(__PSP__)
#include <pspkernel.h>
#endif

#include <engine/graphics/graphics.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/graphics/material.h>
#include <engine/game_elements/gameobject.h>
#include <engine/graphics/3d_graphics/mesh_manager.h>
#include <engine/debug/stack_debug_object.h>

#pragma region Constructors / Destructor

LineRenderer::LineRenderer()
{
}

ReflectiveData LineRenderer::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, m_color, "color", true);
	Reflective::AddVariable(reflectedVariables, m_startPosition, "startPosition", true);
	Reflective::AddVariable(reflectedVariables, m_endPosition, "endPosition", true);
	Reflective::AddVariable(reflectedVariables, width, "width", true);
	Reflective::AddVariable(reflectedVariables, m_material, "material", true);

	return reflectedVariables;
}

void LineRenderer::OnReflectionUpdated()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	Graphics::s_isRenderingBatchDirty = true;
}

LineRenderer::~LineRenderer()
{
}

#pragma endregion

void LineRenderer::SetOrderInLayer(int orderInLayer)
{
	m_orderInLayer = orderInLayer;
	Graphics::SetDrawOrderListAsDirty();
}

void LineRenderer::CreateRenderCommands(RenderBatch& renderBatch)
{
	if (m_material == nullptr)
		return;

	RenderCommand command = RenderCommand();
	command.material = m_material.get();
	command.drawable = this;
	command.transform = GetTransformRaw();
	command.isEnabled = IsEnabled() && GetGameObjectRaw()->IsLocalActive();

	if (m_material->GetRenderingMode() == MaterialRenderingModes::Opaque || m_material->GetRenderingMode() == MaterialRenderingModes::Cutout)
	{
		RenderQueue& renderQueue = renderBatch.renderQueues[m_material->GetFileId()];
		renderQueue.commands.push_back(command);
		renderQueue.commandIndex++;
	}
	else
	{
		renderBatch.transparentMeshCommands.push_back(command);
		renderBatch.transparentMeshCommandIndex++;
	}
}

void LineRenderer::OnDisabled()
{
	Graphics::s_isRenderingBatchDirty = true;
}

void LineRenderer::OnEnabled()
{
	Graphics::s_isRenderingBatchDirty = true;
}

/// <summary>
/// Draw sprite
/// </summary>
void LineRenderer::DrawCommand(const RenderCommand& renderCommand)
{
	if (m_meshData)
		m_meshData.reset();

	//float sizeFixer = 0.1f;
	const float sizeFixer = 1.0f;

	const Vector2 dir = (Vector2(m_endPosition.x, m_endPosition.y) - Vector2(m_startPosition.x, m_startPosition.y)).Normalized();

	Vector3 start = m_startPosition * sizeFixer;
	Vector3 end = m_endPosition * sizeFixer;
	start.x = -start.x;
	end.x = -end.x;

	const float width2 = width * sizeFixer;

	const float fixedXWidth = width2 / 2.0f * dir.y;
	const float fixedYWidth = width2 / 2.0f * dir.x;

	m_meshData = MeshData::MakeMeshData(4, 6, false, false, true);
	m_meshData->AddVertex(1.0f, 1.0f, start.x - fixedXWidth, start.y - fixedYWidth, 0.0f, 0, 0);
	m_meshData->AddVertex(0.0f, 0.0f, end.x - fixedXWidth, end.y - fixedYWidth, 0.0f, 1, 0);
	m_meshData->AddVertex(1.0f, 0.0f, end.x + fixedXWidth, end.y + fixedYWidth, 0.0f, 2, 0);
	m_meshData->AddVertex(0.0f, 1.0f, start.x + fixedXWidth, start.y + fixedYWidth, 0.0f, 3, 0);

	std::unique_ptr<MeshData::SubMesh>& subMesh = m_meshData->m_subMeshes[0];
	subMesh->indices[0] = 0;
	subMesh->indices[1] = 2;
	subMesh->indices[2] = 1;
	subMesh->indices[3] = 2;
	subMesh->indices[4] = 0;
	subMesh->indices[5] = 3;

	m_meshData->OnLoadFileReferenceFinished();

#if defined(__PSP__)
	sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif
	RenderingSettings renderSettings = RenderingSettings();
	renderSettings.invertFaces = false;
	renderSettings.useDepth = true;
	renderSettings.useTexture = true;
	renderSettings.useLighting = m_material->GetUseLighting();
	renderSettings.renderingMode = renderCommand.material->GetRenderingMode();
	MeshManager::DrawMesh(*GetTransformRaw(), *subMesh, *m_material, renderSettings);
}

#endif // ENABLE_EXPERIMENTAL_FEATURES