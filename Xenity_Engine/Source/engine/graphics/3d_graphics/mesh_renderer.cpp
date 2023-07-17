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
	SetReflection();
}

void MeshRenderer::SetReflection()
{
	reflectedVariables["meshData"] = &meshData;
	reflectedVariables["texture"] = &texture;
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