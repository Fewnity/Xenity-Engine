#include "graphics.h"
#include "../../xenity.h"
#include "../graphics/renderer/renderer.h"

Camera* Graphics::usedCamera = nullptr;
int Graphics::usedShaderProgram = -1;
Material* Graphics::usedMaterial = nullptr;
int Graphics::iDrawablesCount = 0;
std::vector<IDrawable*> Graphics::orderedIDrawable;
ProfilerBenchmark* orderBenchmark = new ProfilerBenchmark("Order Drawables");
ProfilerBenchmark* gameobjectScanBenchmark = new ProfilerBenchmark("Scan GameObjects");

bool ordered = false;

/// <summary>
/// Draw all Drawable elements
/// </summary>
void Graphics::DrawAllDrawable()
{
	//if(!ordered)
	Graphics::OrderDrawables();
	//ordered = true;
	//glEnable(GL_DEPTH_TEST);
	Engine::renderer->SetCullFace(Back);

	for (int i = 0; i < iDrawablesCount; i++)
	{
		orderedIDrawable[i]->Draw();
	}
	/*int drawableCount = AssetManager::GetDrawableCount();
	for (int i = 0; i < drawableCount; i++)
	{
		IDrawable* drawable = AssetManager::GetDrawable(i);

		drawable->Draw();
	}*/
}

void Graphics::OrderDrawables()
{
	orderBenchmark->Start();
	int drawableCount = orderedIDrawable.size();
	//int drawableCount = AssetManager::GetDrawableCount();
	for (int iDrawIndex = 0; iDrawIndex < drawableCount; iDrawIndex++)
	{
		IDrawable* drawableToCheck = orderedIDrawable[iDrawIndex];
		//IDrawable* drawableToCheck = AssetManager::GetDrawable(iDrawIndex);
		if (drawableToCheck->GetGameObject()->transform.movedLastFrame || drawableToCheck->needReorder)
		{
			gameobjectScanBenchmark->Start();
			drawableToCheck->GetGameObject()->transform.movedLastFrame = false;
			drawableToCheck->needReorder = false;
			orderedIDrawable.erase(orderedIDrawable.begin() + iDrawIndex);
			iDrawablesCount--;
			drawableCount--;
			iDrawIndex--;
			OrderOneDrawable(drawableToCheck);
			gameobjectScanBenchmark->Stop();
		}
	}

	//bool needReorder = 
	drawableCount = AssetManager::GetDrawableCount();
	if (Engine::drawOrderListDirty)
	{
		Engine::drawOrderListDirty = false;

		iDrawablesCount = 0;
		orderedIDrawable.clear();

		for (int iDrawIndex = 0; iDrawIndex < drawableCount; iDrawIndex++)
		{
			IDrawable* drawableToCheck = AssetManager::GetDrawable(iDrawIndex);
			OrderOneDrawable(drawableToCheck);
		}
	}
	orderBenchmark->Stop();
}

void Graphics::AddDrawable(IDrawable* drawableToPlace)
{
	OrderOneDrawable(drawableToPlace);
}

void Graphics::OrderOneDrawable(IDrawable* drawableToPlace)
{
	float z = drawableToPlace->GetGameObject()->transform.GetPosition().z;
	bool placeFound = false;

	for (int i = 0; i < iDrawablesCount; i++)
	{
		IDrawable* drawable = orderedIDrawable[i];
		int d1 = drawableToPlace->GetDrawPriority();
		int d2 = drawable->GetDrawPriority();

		//Check if the checked has a higher priority (lower value) than the component in the list
		if (d1 <= d2)
		{
			if (d1 == d2)
			{
				if (z >= drawable->GetGameObject()->transform.GetPosition().z)
				{
					orderedIDrawable.insert(std::begin(orderedIDrawable) + i, drawableToPlace);
					placeFound = true;
					break;
				}
			}
			else
			{
				orderedIDrawable.insert(std::begin(orderedIDrawable) + i, drawableToPlace);
				placeFound = true;
				break;
			}
		}
	}

	//if the priority is lower than all components's priorities in the list, add it the end of the list
	if (!placeFound)
	{
		orderedIDrawable.push_back(drawableToPlace);
	}
	iDrawablesCount++;
}