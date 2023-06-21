#include "mesh_renderer.h"
#include "../../../xenity.h"

using namespace std;

// #pragma region MeshRenderer Constructors / Destructor

/// <summary>
/// Instantiate an empty mesh
/// </summary>
MeshRenderer::MeshRenderer()
{
	componentName = "Mesh Renderer";
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
	if (auto gameObject = GetGameObject().lock())
	{
		if (gameObject->GetLocalActive() && GetIsEnabled())
		{
			auto transform = GetTransform().lock();
			MeshManager::DrawMesh(transform->GetPosition(), transform->GetRotation(), transform->GetScale(), texture, meshData, true, true);
		}
	}
}