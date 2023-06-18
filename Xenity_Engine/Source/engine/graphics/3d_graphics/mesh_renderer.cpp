#include "mesh_renderer.h"
#include "../../../xenity.h"
#include "../../graphics/renderer/renderer.h"

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
    MeshManager::DrawMesh(GetTransform().lock()->GetPosition(), GetTransform().lock()->GetRotation(), GetTransform().lock()->GetScale(), texture, meshData, true);
}