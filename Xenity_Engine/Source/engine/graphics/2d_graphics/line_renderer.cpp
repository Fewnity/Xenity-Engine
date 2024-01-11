#include "line_renderer.h"

#include <engine/graphics/graphics.h>
#include "sprite_manager.h"
#include <engine/graphics/3d_graphics/mesh_data.h>

#include <engine/game_elements/gameobject.h>

#if defined(__PSP__)
#include <pspkernel.h>
#endif

#pragma region Constructors / Destructor

LineRenderer::LineRenderer()
{
	componentName = "LineRenderer";
	type = Draw_2D;
}

ReflectiveData LineRenderer::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, color, "color", true);
	Reflective::AddVariable(reflectedVariables, startPosition, "startPosition", true);
	Reflective::AddVariable(reflectedVariables, endPosition, "endPosition", true);
	Reflective::AddVariable(reflectedVariables, width, "width", true);
	return reflectedVariables;
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

/// <summary>
/// Draw sprite
/// </summary>
void LineRenderer::Draw()
{
	if (const std::shared_ptr<GameObject> gameObject = GetGameObject())
	{
		// Draw the sprite only if there is a texture and if the component/gameobject is active
		if (gameObject->GetLocalActive() && GetIsEnabled())
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

			SpriteManager::Render2DLine(meshData);
		}
	}
}