#include "graphics.h"
#include "../../xenity.h"

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

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

	Graphics::OrderDrawables();

	//glEnable(GL_DEPTH_TEST);

	glCullFace(GL_BACK);
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