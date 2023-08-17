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
	//SetReflection();
}

/*void MeshRenderer::SetReflection()
{
	reflectedVariables["meshData"] = &meshData;
	reflectedVariables["texture"] = &texture;
}*/

std::unordered_map<std::string, Variable> MeshRenderer::GetReflection()
{
	std::unordered_map<std::string, Variable> reflectedVariables;
	reflectedVariables.insert_or_assign("meshData", meshData);
	reflectedVariables.insert_or_assign("texture", texture);
	reflectedVariables.insert_or_assign("textures", textures);
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
			MeshManager::DrawMesh(transform->GetPosition(), transform->GetRotation(), transform->GetScale(), texture, meshData, true, false, true);
		}
	}
}