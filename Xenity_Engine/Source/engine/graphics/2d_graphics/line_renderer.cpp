#include "line_renderer.h"
#include "../../../xenity.h"
#include "../../graphics/3d_graphics/mesh_data.h"

#pragma region Constructors / Destructor

LineRenderer::LineRenderer()
{
	componentName = "LineRenderer";
	//SetReflection();
}

/*void LineRenderer::SetReflection()
{
	reflectedVariables["color"] = &color;
	reflectedVariables["startPosition"] = &startPosition;
	reflectedVariables["endPosition"] = &endPosition;
	reflectedVariables["width"] = &width;
}*/

std::unordered_map<std::string, Variable> LineRenderer::GetReflection()
{
	std::unordered_map<std::string, Variable> reflectedVariables;
	reflectedVariables["color"] = color;
	reflectedVariables["startPosition"] = startPosition;
	reflectedVariables["endPosition"] = endPosition;
	reflectedVariables["width"] = width;
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

/// <summary>
/// Draw sprite
/// </summary>
void LineRenderer::Draw()
{
	if (auto gameObject = GetGameObject())
	{
		// Draw the sprite only if there is a texture and if the component/gameobject is active
		if (gameObject->GetLocalActive() && GetIsEnabled())
		{
			if (meshData)
				delete meshData;

			//float sizeFixer = 0.1f;
			float sizeFixer = 1.0f;

			Vector2 dir = (Vector2(endPosition.x, endPosition.y) - Vector2(startPosition.x, startPosition.y)).Normalize();

			Vector3 start = startPosition * sizeFixer;
			Vector3 end = endPosition * sizeFixer;
			start.x = -start.x;
			end.x = -end.x;

			float width2 = width * sizeFixer;

			float fixedXWidth = width2 / 2.0f * dir.y;
			float fixedYWidth = width2 / 2.0f * dir.x;

			meshData = new MeshData(4, 6, false, false);
			meshData->AddVertex(1.0f, 1.0f, start.x - fixedXWidth, start.y - fixedYWidth, 0.0f, 0);
			meshData->AddVertex(0.0f, 0.0f, end.x - fixedXWidth, end.y - fixedYWidth, 0.0f, 1);
			meshData->AddVertex(1.0f, 0.0f, end.x + fixedXWidth, end.y + fixedYWidth, 0.0f, 2);
			meshData->AddVertex(0.0f, 1.0f, start.x + fixedXWidth, start.y + fixedYWidth, 0.0f, 3);

			meshData->indices[0] = 0;
			meshData->indices[1] = 2;
			meshData->indices[2] = 1;
			meshData->indices[3] = 2;
			meshData->indices[4] = 0;
			meshData->indices[5] = 3;

#if defined(__PSP__)
			sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif

			SpriteManager::Render2DLine(meshData);
		}
	}
}