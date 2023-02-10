#include "engine.h"
#include "ui/ui_manager.h"
#include "file_system/file.h"
#include "inputs/input_system.h"
#include "scene_manager/scene.h"
#include "ui/window.h"
#include "gameobject.h"
#include "game.h"
#include "graphics/graphics.h"
#include <SDL2/SDL.h>
#include "asset_manager.h"
#include "engine_settings.h"
#include "graphics/sprite_manager.h"
#include "debug.h"

Light * pointLight = new Light();
Light* spotLight = new Light();
Light* spotLight2 = new Light();
Light* directionalLight = new Light();
GameObject * pointLightGameObject = new GameObject();
GameObject * spotLightGameObject = new GameObject();
GameObject * spotLight2GameObject = new GameObject();
GameObject * directionalLightGameObject = new GameObject();

std::vector<GameObject*> Engine::gameObjects;
float lastTick = 0;

/// <summary>
/// Init engine
/// </summary>
/// <returns></returns>
int Engine::Init()
{
	Debug::Init();
	/* Initialize the library */
	File::InitFileSystem();
	if (Window::InitWindow() != 0 || UiManager::Init() != 0 || Audio::Init() != 0) {
		return -1;
	}
	InputSystem::Init();
	SpriteManager::Init();
	
	pointLightGameObject->transform.SetPosition(Vector3(1.5f, 1.5, 1.5f));
	pointLightGameObject->AddExistingComponent(pointLight);
	pointLight->type = Light::Point;
	pointLight->color = Vector3(1, 0.1f, 0.1f);
	pointLight->intensity = 10;
	pointLight->SetRange(7);
	
	spotLightGameObject->transform.SetPosition(Vector3(0, 3, 0));
	spotLightGameObject->transform.SetRotation(Vector3(0.0f, -1.0f, 0.0f));
	spotLightGameObject->AddExistingComponent(spotLight);
	spotLight->type = Light::Spot;
	spotLight->color = Vector3(0.05f, 0.05f, 1);
	spotLight->intensity = 200;
	spotLight->SetRange(7);
	spotLight->SetSpotSmoothness(0.0f);
	spotLight->SetSpotAngle(17.0f);

	spotLight2GameObject->transform.SetPosition(Vector3(5, 3, 0));
	spotLight2GameObject->transform.SetRotation(Vector3(0.0f, -1.0f, 0.0f));
	spotLight2GameObject->AddExistingComponent(spotLight2);
	spotLight2->type = Light::Spot;
	spotLight2->color = Vector3(0.05f, 0.05f, 1);
	spotLight2->intensity = 200;
	spotLight2->SetRange(7);
	spotLight2->SetSpotSmoothness(1.0f);
	spotLight2->SetSpotAngle(17.0f);
	
	directionalLightGameObject->transform.SetRotation(Vector3(0.0f, -1.0f, -1.0f));
	directionalLightGameObject->AddExistingComponent(directionalLight);
	directionalLight->type = Light::Directional;
	directionalLight->color = Vector3(0.3f, 0.7f, 0.3f);
	directionalLight->intensity = 1;

	return 0;
}

/// <summary>
/// Update all components
/// </summary>
void Engine::UpdateComponents() {
	//Update all gameobjects components
	int gameObjectCount = gameObjects.size();
	for (int gIndex = 0; gIndex < gameObjectCount; gIndex++)
	{
		int componentCount = gameObjects[gIndex]->components.size();
		for (int cIndex = 0; cIndex < componentCount; cIndex++)
		{
			gameObjects[gIndex]->components[cIndex]->Update();
		}
	}
}

/// <summary>
/// Engine loop
/// </summary>
void Engine::Loop()
{
	bool running = true;
	while (running)
	{
		float tempTick = (float)SDL_GetTicks64();
		EngineSettings::deltaTime = (tempTick - lastTick) / 1000.0f;
		lastTick = tempTick;

		SDL_Event event;
		InputSystem::ClearInputs();
		while (SDL_PollEvent(&event))
		{
			InputSystem::UpdateInputs(event);
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					Window::OnResize(event.window.data1, event.window.data2);
				}
				break;
			default:
				break;
			}
		}

		UpdateComponents();

		Game::Loop();

		//Clear the OpenGL window
		glClearColor(0.529f, 0.808f, 0.922f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Material
		AssetManager::shaders[0]->SetShaderAttribut("ambiantLightColor", Vector3(1, 1, 1));
		AssetManager::shaders[0]->SetShaderAttribut("cameraPos", Graphics::usedCamera->gameObject->transform.GetPosition());
		AssetManager::shaders[0]->SetShaderAttribut("offsetPosition", Vector3(0, 0, 0));
		AssetManager::shaders[0]->SetShaderAttribut("material.ambient", Vector3(0.529f, 0.808f, 0.922f));
		AssetManager::shaders[0]->SetShaderAttribut("material.shininess", 32.0f);

		AssetManager::shaders[1]->SetShaderAttribut("ambiantLightColor", Vector3(1, 1, 1));
		AssetManager::shaders[1]->SetShaderAttribut("cameraPos", Graphics::usedCamera->gameObject->transform.GetPosition());
		AssetManager::shaders[1]->SetShaderAttribut("offsetPosition", Vector3(0, 0, 0));
		AssetManager::shaders[1]->SetShaderAttribut("material.ambient", Vector3(0.529f, 0.808f, 0.922f));
		AssetManager::shaders[1]->SetShaderAttribut("material.shininess", 32.0f);

		AssetManager::shaders[5]->SetShaderAttribut("ambiantLightColor", Vector3(1, 1, 1));
		AssetManager::shaders[5]->SetShaderAttribut("cameraPos", Graphics::usedCamera->gameObject->transform.GetPosition());
		AssetManager::shaders[5]->SetShaderAttribut("offsetPosition", Vector3(0, 0, 0));
		AssetManager::shaders[5]->SetShaderAttribut("material.ambient", Vector3(0.529f, 0.808f, 0.922f));
		AssetManager::shaders[5]->SetShaderAttribut("material.shininess", 32.0f);

		
		int gameObjectCount = gameObjects.size();
		for (int gIndex = 0; gIndex < gameObjectCount; gIndex++)
		{
			if (gameObjects[gIndex]->parent == nullptr)
				gameObjects[gIndex]->SetChildrenWorldPositions();
		}

		//Set wireframe
		if (EngineSettings::isWireframe) {
			glLineWidth(2);
			glPolygonMode(GL_FRONT, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT, GL_FILL);
		}


		Graphics::DrawAllDrawable();

		glPolygonMode(GL_FRONT, GL_FILL);

		std::string debugText = std::string("Wireframe (A): ") + (EngineSettings::isWireframe ? "True" : "False");
		debugText += std::string(", Delta Time: ") + std::to_string(EngineSettings::deltaTime);
		debugText += std::string(" ") + std::to_string(Graphics::usedCamera->gameObject->transform.GetRotation().x) + " " + std::to_string(Graphics::usedCamera->gameObject->transform.GetRotation().y) + " " + std::to_string(Graphics::usedCamera->gameObject->transform.GetRotation().z);
		UiManager::RenderText(*AssetManager::shaders[3], debugText, 0.0f, 24, 0.5f, glm::vec3(0.5f, 0.0f, 0.2f));
		Window::UpdateScreen();
	}
}

/// <summary>
/// 
/// </summary>
/// <param name="gameObject"></param>
void Engine::AddGameObject(GameObject* gameObject) 
{
	gameObjects.push_back(gameObject);
}

/// <summary>
/// 
/// </summary>
/// <returns></returns>
std::vector<GameObject*> Engine::GetGameObjects()
{
	return std::vector<GameObject*>(gameObjects);
}
