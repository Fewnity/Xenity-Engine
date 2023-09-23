#include "mesh_renderer.h"
#include "../../../xenity.h"
#include "../renderer/renderer.h"

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
	material = Engine::standardMaterial;
}

std::unordered_map<std::string, ReflectionEntry> MeshRenderer::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	Reflection::AddReflectionVariable(reflectedVariables, meshData, "meshData", true);
	Reflection::AddReflectionVariable(reflectedVariables, textures, "textures", true);
	Reflection::AddReflectionVariable(reflectedVariables, material, "material", true);
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
	if (auto gameObject = GetGameObject())
	{
		if (gameObject->GetLocalActive() && GetIsEnabled())
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