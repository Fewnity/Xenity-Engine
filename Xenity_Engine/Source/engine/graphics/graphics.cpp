#include "graphics.h"
#include "../../xenity.h"
#include "../graphics/renderer/renderer.h"

Camera* Graphics::usedCamera = nullptr;
int Graphics::usedShaderProgram = -1;
Material* Graphics::usedMaterial = nullptr;
int Graphics::iDrawablesCount = 0;
std::vector<IDrawable*> Graphics::orderedIDrawable;
ProfilerBenchmark* orderBenchmark = new ProfilerBenchmark("Order Drawables");

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

	int drawableCount = AssetManager::GetDrawableCount();
	iDrawablesCount = 0;
	orderedIDrawable.clear();
	for (int iDrawIndex = 0; iDrawIndex < drawableCount; iDrawIndex++)
	{
		IDrawable* drawableToCheck = AssetManager::GetDrawable(iDrawIndex);
		//if (drawableToCheck->gameObject) {
		bool placeFound = false;

		for (int i = 0; i < iDrawablesCount; i++)
		{
			//Check if the checked has a higher priority (lower value) than the component in the list
			if (drawableToCheck->GetDrawPriority() <= orderedIDrawable[i]->GetDrawPriority())
			{
				orderedIDrawable.insert(orderedIDrawable.begin() + i, drawableToCheck);
				placeFound = true;
				break;
			}
		}
		//if the priority is lower than all components's priorities in the list, add it the end of the list
		if (!placeFound)
		{
			orderedIDrawable.push_back(drawableToCheck);
		}
		iDrawablesCount++;

		//}
	}

	orderBenchmark->Stop();
}