#include "graphics.h"
#include "../../xenity.h"
#include "../graphics/renderer/renderer.h"
#include <algorithm>

Camera* Graphics::usedCamera = nullptr;
int Graphics::usedShaderProgram = -1;
Material* Graphics::usedMaterial = nullptr;
int Graphics::iDrawablesCount = 0;
std::vector<IDrawable*> Graphics::orderedIDrawable;
ProfilerBenchmark* orderBenchmark = new ProfilerBenchmark("Order Drawables");
ProfilerBenchmark* gameobjectScanBenchmark = new ProfilerBenchmark("Scan GameObjects");

/// <summary>
/// Draw all Drawable elements
/// </summary>
void Graphics::DrawAllDrawable()
{
	Graphics::OrderDrawables();
	Engine::renderer->SetCullFace(Back);

	for (int i = 0; i < iDrawablesCount; i++)
	{
		orderedIDrawable[i]->Draw();
	}
}

bool spriteComparator(const IDrawable* t1, const IDrawable* t2)
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
	//return (t1->GetDrawPriority() < t2->GetDrawPriority()) || (t1->GetDrawPriority() == t2->GetDrawPriority() && t1->GetGameObject()->transform.GetPosition().z > t2->GetGameObject()->transform.GetPosition().z);
}

void Graphics::OrderDrawables()
{
	orderBenchmark->Start();
	gameobjectScanBenchmark->Start();
	for (int iDrawIndex = 0; iDrawIndex < iDrawablesCount; iDrawIndex++)
	{
		IDrawable* drawableToCheck = orderedIDrawable[iDrawIndex];
		if (drawableToCheck->GetTransform()->movedLastFrame)
		{
			Engine::drawOrderListDirty = true;
			break;
		}
	}
	gameobjectScanBenchmark->Stop();

	if (Engine::drawOrderListDirty)
	{
		std::sort(orderedIDrawable.begin(), orderedIDrawable.end(), spriteComparator);
		Engine::drawOrderListDirty = false;
	}
	orderBenchmark->Stop();
}

void Graphics::AddDrawable(IDrawable* drawableToPlace)
{
	orderedIDrawable.push_back(drawableToPlace);
	iDrawablesCount++;
}