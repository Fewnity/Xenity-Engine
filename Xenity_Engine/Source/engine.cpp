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

		//Set wireframe
		if (EngineSettings::isWireframe) 
		{
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
		UiManager::RenderText(*AssetManager::shaders[3], debugText, 0.0f, 24,90, 0.5f, glm::vec3(0.5f, 0.0f, 0.2f), UiManager::fonts[0]);
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
