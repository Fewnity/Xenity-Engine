#include "mesh_renderer.h"

#include <engine/graphics/renderer/renderer.h>
#include <engine/graphics/3d_graphics/mesh_manager.h>

#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>


using namespace std;

// #pragma region MeshRenderer Constructors / Destructor

/// <summary>
/// Instantiate an empty mesh
/// </summary>
MeshRenderer::MeshRenderer()
{
	componentName = "MeshRenderer";
	type = Draw_3D;
	AssetManager::AddReflection(this);
	material = AssetManager::standardMaterial;
}

ReflectiveData MeshRenderer::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, meshData, "meshData", true);
	Reflective::AddVariable(reflectedVariables, textures, "textures", true);
	Reflective::AddVariable(reflectedVariables, material, "material", true);
	return reflectedVariables;
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

void MeshRenderer::Draw()
{
	if (const std::shared_ptr<GameObject> gameObject = GetGameObject())
	{
		if (gameObject->GetLocalActive() && GetIsEnabled() && meshData)
		{
			RenderingSettings renderSettings = RenderingSettings();
			renderSettings.invertFaces = false;
			renderSettings.useBlend = false;
			renderSettings.useDepth = true;
			renderSettings.useTexture = true;
			renderSettings.useLighting = true;
			MeshManager::DrawMesh(GetTransform(), textures, meshData, renderSettings, material);
		}
	}
}