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

using namespace std;

// #pragma region MeshRenderer Constructors / Destructor

/// <summary>
/// Instantiate an empty mesh
/// </summary>
MeshRenderer::MeshRenderer()
{
	componentName = "MeshRenderer";
	type = IDrawableTypes::Draw_3D;
	AssetManager::AddReflection(this);
	//material = AssetManager::standardMaterial;
}

ReflectiveData MeshRenderer::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, meshData, "meshData", true);
	Reflective::AddVariable(reflectedVariables, textures, "textures", true);
	Reflective::AddVariable(reflectedVariables, materials, "materials", true);
	Reflective::AddVariable(reflectedVariables, backupMaterials, "backupMaterials", true);
	Reflective::AddVariable(reflectedVariables, startBackup, "startBackup", true);
	return reflectedVariables;
}

void MeshRenderer::OnReflectionUpdated()
{
	matCount = materials.size();
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

void MeshRenderer::Update()
{
}

void MeshRenderer::CreateRenderCommands(RenderBatch& renderBatch)
{
	if (!meshData)
		return;

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

		RenderCommand* command = nullptr;
		if(!material->useTransparency)
		{
			RenderQueue& renderQueue = renderBatch.renderQueues[material->fileId];
			command = &renderQueue.commands[renderQueue.commandIndex];
			renderQueue.commandIndex++;
		}
		else
		{
			command = &renderBatch.transparentMeshCommands[renderBatch.transparentMeshCommandIndex];
			renderBatch.transparentMeshCommandIndex++;
		}
		command->material = material;
		command->drawable = this;
		command->subMesh = meshData->subMeshes[i];
		command->transform = GetTransform();
		command->isEnabled = GetIsEnabled() && GetGameObject()->GetLocalActive();
	}
}

void MeshRenderer::Draw()
{
	/*if (startBackup)
	{
		const int gmSize = GameplayManager::gameObjects.size();
		for (int gmI = 0; gmI < gmSize; gmI++)
		{
			std::shared_ptr<MeshRenderer> mr = GameplayManager::gameObjects[gmI]->GetComponent<MeshRenderer>();
			if (mr)
			{
				const int tSize = mr->textures.size();
				const int mSize = backupMaterials.size();
				for (int i = 0; i < tSize; i++)
				{
					for (int im = 0; im < mSize; im++)
					{
						mr->materials.push_back(nullptr);
						if (backupMaterials[im] && mr->textures[i] == backupMaterials[im]->texture)
						{
							mr->materials[i] = backupMaterials[im];
						}
					}
				}
			}
		}
		startBackup = false;
		Debug::Print("A");
	}*/
	/*if (const std::shared_ptr<GameObject> gameObject = GetGameObject())
	{
		if (gameObject->GetLocalActive() && GetIsEnabled() && meshData && !culled)
		{
			RenderingSettings renderSettings = RenderingSettings();
			renderSettings.invertFaces = false;
			renderSettings.useDepth = true;
			renderSettings.useTexture = true;
			renderSettings.useLighting = true;
			MeshManager::DrawMesh(GetTransform(), meshData, materials, renderSettings);
		}
	}*/
}

void MeshRenderer::DrawSubMesh(const RenderCommand& renderCommand)
{
	RenderingSettings renderSettings = RenderingSettings();
	renderSettings.invertFaces = false;
	renderSettings.useDepth = true;
	renderSettings.useTexture = true;
	renderSettings.useLighting = true;
	renderSettings.useBlend = renderCommand.material->useTransparency;
	MeshManager::DrawMesh(GetTransform(), *renderCommand.subMesh, renderCommand.material, renderSettings);
}
