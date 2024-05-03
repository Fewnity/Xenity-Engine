#include "mesh_renderer.h"

#include <engine/graphics/renderer/renderer.h>
#include <engine/graphics/3d_graphics/mesh_manager.h>
#include <engine/graphics/material.h>
#include <engine/engine.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/game_elements/transform.h>

#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>
#include <engine/inputs/input_system.h>
#include <engine/debug/debug.h>
#include <engine/graphics/graphics.h>

using namespace std;

// #pragma region MeshRenderer Constructors / Destructor

/// <summary>
/// Instantiate an empty mesh
/// </summary>
MeshRenderer::MeshRenderer()
{
	componentName = "MeshRenderer";
	AssetManager::AddReflection(this);
}

ReflectiveData MeshRenderer::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, meshData, "meshData", true);
	Reflective::AddVariable(reflectedVariables, materials, "materials", true);
	return reflectedVariables;
}

void MeshRenderer::OnReflectionUpdated()
{
	if (meshData)
		materials.resize(meshData->subMeshCount);

	matCount = materials.size();
	Graphics::isRenderingBatchDirty = true;
}

/// <summary>
/// Destructor
/// </summary>
MeshRenderer::~MeshRenderer()
{
	AssetManager::RemoveReflection(this);
}

int MeshRenderer::GetDrawPriority() const
{
	return 0;
}

void MeshRenderer::CreateRenderCommands(RenderBatch& renderBatch)
{
	if (!meshData)
		return;

	// Create a command for each submesh
	const int subMeshCount = meshData->subMeshCount;
	std::shared_ptr<Material> material = nullptr;
	for (int i = 0; i < subMeshCount; i++)
	{
		if (i != matCount)
			material = materials[i];
		else
			break;
		if (material == nullptr)
			continue;

		RenderCommand command = RenderCommand();
		command.material = material;
		command.drawable = this;
		command.subMesh = meshData->subMeshes[i];
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
}

void MeshRenderer::SetMeshData(std::shared_ptr<MeshData> meshData)
{
	this->meshData = meshData;
	if (meshData) 
	{
		materials.resize(meshData->subMeshCount);
		matCount = meshData->subMeshCount;
	}
	else 
	{

	}
	Graphics::isRenderingBatchDirty = true;
}

std::shared_ptr<MeshData> MeshRenderer::GetMeshData()
{
	return meshData;
}

void MeshRenderer::SetMaterial(std::shared_ptr<Material> material, int index)
{
	if (index < materials.size())
	{
		materials[index] = material;
		Graphics::isRenderingBatchDirty = true;
	}
}

void MeshRenderer::OnDisabled()
{
	Graphics::isRenderingBatchDirty = true;
}

void MeshRenderer::OnEnabled()
{
	Graphics::isRenderingBatchDirty = true;
}

void MeshRenderer::DrawCommand(const RenderCommand& renderCommand)
{
	RenderingSettings renderSettings = RenderingSettings();
	renderSettings.invertFaces = false;
	renderSettings.useDepth = true;
	renderSettings.useTexture = true;
	renderSettings.useLighting = renderCommand.material->useLighting;
	renderSettings.useBlend = renderCommand.material->useTransparency;
	MeshManager::DrawMesh(GetTransform(), *renderCommand.subMesh, renderCommand.material, renderSettings);
}
