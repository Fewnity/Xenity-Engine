#include "mesh_renderer.h"
#include "../../../xenity.h"

using namespace std;

// #pragma region MeshRenderer Constructors / Destructor

/// <summary>
/// Instantiate an empty mesh
/// </summary>
MeshRenderer::MeshRenderer()
{
	componentName = "MeshRenderer";
	type = Draw_3D;
	//SetReflection();
}

/*void MeshRenderer::SetReflection()
{
	reflectedVariables["meshData"] = &meshData;
	reflectedVariables["texture"] = &texture;
}*/

std::unordered_map<std::string, ReflectionEntry> MeshRenderer::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	Reflection::AddReflectionVariable(reflectedVariables, meshData, "meshData", true);
	Reflection::AddReflectionVariable(reflectedVariables, textures, "textures", true);
	return reflectedVariables;
}

/// <summary>
/// Destructor
/// </summary>
MeshRenderer::~MeshRenderer()
{
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
			auto transform = GetTransform();

			MeshManager::DrawMesh(transform->GetPosition(), transform->GetRotation(), transform->GetScale(), textures, meshData, true, false, true);
		}
	}
}