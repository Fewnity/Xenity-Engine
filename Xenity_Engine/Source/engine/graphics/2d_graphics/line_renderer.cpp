// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "line_renderer.h"

#include <engine/graphics/graphics.h>
#include "sprite_manager.h"
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/graphics/material.h>

#include <engine/game_elements/gameobject.h>

#if defined(__PSP__)
#include <pspkernel.h>
#endif
#include <engine/graphics/3d_graphics/mesh_manager.h>

#pragma region Constructors / Destructor

LineRenderer::LineRenderer()
{
	componentName = "LineRenderer";
}

ReflectiveData LineRenderer::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, color, "color", true);
	Reflective::AddVariable(reflectedVariables, startPosition, "startPosition", true);
	Reflective::AddVariable(reflectedVariables, endPosition, "endPosition", true);
	Reflective::AddVariable(reflectedVariables, width, "width", true);
	Reflective::AddVariable(reflectedVariables, material, "material", true);

	return reflectedVariables;
}

void LineRenderer::OnReflectionUpdated()
{
	Graphics::isRenderingBatchDirty = true;
}

LineRenderer::~LineRenderer()
{
}

int LineRenderer::GetDrawPriority() const
{
	return orderInLayer;
}

#pragma endregion

void LineRenderer::SetOrderInLayer(int orderInLayer)
{
	this->orderInLayer = orderInLayer;
	Graphics::SetDrawOrderListAsDirty();
}

void LineRenderer::CreateRenderCommands(RenderBatch& renderBatch)
{
	if (material == nullptr)
		return;

	RenderCommand command = RenderCommand();
	command.material = material;
	command.drawable = this;
	command.subMesh = nullptr;
	command.transform = GetTransform();
	command.isEnabled = GetIsEnabled() && GetGameObject()->GetLocalActive();
	if (!material->useTransparency)
	{
		RenderQueue& renderQueue = renderBatch.renderQueues[material->fileId];
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
	Graphics::isRenderingBatchDirty = true;
}

void LineRenderer::OnEnabled()
{
	Graphics::isRenderingBatchDirty = true;
}

/// <summary>
/// Draw sprite
/// </summary>
void LineRenderer::DrawCommand(const RenderCommand& renderCommand)
{
	if (meshData)
		meshData.reset();

	//float sizeFixer = 0.1f;
	const float sizeFixer = 1.0f;

	const Vector2 dir = (Vector2(endPosition.x, endPosition.y) - Vector2(startPosition.x, startPosition.y)).Normalized();

	Vector3 start = startPosition * sizeFixer;
	Vector3 end = endPosition * sizeFixer;
	start.x = -start.x;
	end.x = -end.x;

	const float width2 = width * sizeFixer;

	const float fixedXWidth = width2 / 2.0f * dir.y;
	const float fixedYWidth = width2 / 2.0f * dir.x;

	meshData = MeshData::MakeMeshData(4, 6, false, false, true);
	meshData->AddVertex(1.0f, 1.0f, start.x - fixedXWidth, start.y - fixedYWidth, 0.0f, 0, 0);
	meshData->AddVertex(0.0f, 0.0f, end.x - fixedXWidth, end.y - fixedYWidth, 0.0f, 1, 0);
	meshData->AddVertex(1.0f, 0.0f, end.x + fixedXWidth, end.y + fixedYWidth, 0.0f, 2, 0);
	meshData->AddVertex(0.0f, 1.0f, start.x + fixedXWidth, start.y + fixedYWidth, 0.0f, 3, 0);

	MeshData::SubMesh*& subMesh = meshData->subMeshes[0];
	subMesh->indices[0] = 0;
	subMesh->indices[1] = 2;
	subMesh->indices[2] = 1;
	subMesh->indices[3] = 2;
	subMesh->indices[4] = 0;
	subMesh->indices[5] = 3;

	meshData->OnLoadFileReferenceFinished();

#if defined(__PSP__)
	sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif
	RenderingSettings renderSettings = RenderingSettings();
	renderSettings.invertFaces = false;
	renderSettings.useDepth = true;
	renderSettings.useTexture = true;
	renderSettings.useLighting = material->useLighting;
	renderSettings.useBlend = material->useTransparency;
	MeshManager::DrawMesh(GetTransform(), *subMesh, material, renderSettings);
	//Graphics::DrawSubMesh(*subMesh, material, )
	//SpriteManager::Render2DLine(meshData);
}