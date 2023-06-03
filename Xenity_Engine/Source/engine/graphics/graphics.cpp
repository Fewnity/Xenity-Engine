#include "graphics.h"
#include "../../xenity.h"
#include "../graphics/renderer/renderer.h"
#include <algorithm>
#include "../file_system/mesh_loader/wavefront_loader.h"
#include "3d_graphics/mesh_data.h"

Camera *Graphics::usedCamera = nullptr;
int Graphics::usedShaderProgram = -1;
Material *Graphics::usedMaterial = nullptr;
int Graphics::iDrawablesCount = 0;
std::vector<IDrawable *> Graphics::orderedIDrawable;
// ProfilerBenchmark *orderBenchmark = new ProfilerBenchmark("Order Drawables");
// ProfilerBenchmark *gameobjectScanBenchmark = new ProfilerBenchmark("Scan GameObjects");

bool loaded = false;
MeshData *cubeData = nullptr;
Texture *texture2 = nullptr;

/// <summary>
/// Draw all Drawable elements
/// </summary>
void Graphics::DrawAllDrawable()
{
	if (!loaded)
	{
		cubeData = WavefrontLoader::LoadFromRawData("DonutTriangulate.obj");

		loaded = true;
		texture2 = new Texture("container.jpg", "Container");
	}

	Graphics::OrderDrawables();

	Engine::renderer->NewFrame();
	Engine::renderer->Clear();
	MeshManager::DrawMesh(Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(1, 1, 1), texture2, cubeData);

	for (int i = 0; i < iDrawablesCount; i++)
	{
		orderedIDrawable[i]->Draw();
	}
	Engine::renderer->EndFrame();
}

bool spriteComparator(const IDrawable *t1, const IDrawable *t2)
{
	const int priority1 = t1->GetDrawPriority();
	const int priority2 = t2->GetDrawPriority();

	if (priority1 <= priority2)
	{
		if (priority1 == priority2)
		{
			return t1->GetTransform()->GetPosition().z > t2->GetTransform()->GetPosition().z;
		}
		return true;
	}
	return false;
	// return (t1->GetDrawPriority() < t2->GetDrawPriority()) || (t1->GetDrawPriority() == t2->GetDrawPriority() && t1->GetGameObject()->transform.GetPosition().z > t2->GetGameObject()->transform.GetPosition().z);
}

void Graphics::OrderDrawables()
{
	// orderBenchmark->Start();
	// gameobjectScanBenchmark->Start();
	for (int iDrawIndex = 0; iDrawIndex < iDrawablesCount; iDrawIndex++)
	{
		IDrawable *drawableToCheck = orderedIDrawable[iDrawIndex];
		if (drawableToCheck->GetTransform()->movedLastFrame)
		{
			Engine::drawOrderListDirty = true;
			break;
		}
	}
	// gameobjectScanBenchmark->Stop();

	if (Engine::drawOrderListDirty)
	{
		std::sort(orderedIDrawable.begin(), orderedIDrawable.end(), spriteComparator);
		Engine::drawOrderListDirty = false;
	}
	// orderBenchmark->Stop();
}

void Graphics::AddDrawable(IDrawable *drawableToPlace)
{
	orderedIDrawable.push_back(drawableToPlace);
	iDrawablesCount++;
}