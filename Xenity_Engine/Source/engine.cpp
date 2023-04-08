#include "engine.h"
#include "ui/ui_manager.h"
#include "file_system/file.h"
#include "inputs/input_system.h"
#include "scene_manager/scene.h"
#include "ui/window.h"
#include "gameobject.h"
#include "graphics/graphics.h"
#include <SDL2/SDL.h>
#include "asset_manager.h"
#include "engine_settings.h"
#include "graphics/sprite_manager.h"
#include "debug/debug.h"
#include "tools/benchmark.h"
#include "time/time.h"
#include "debug/performance.h"
#include "ui/TextAlignments.h"
#include "rts_game/game.h"
#include "pathfinding/pathfinding.h"
#include "tools/profiler_benchmark.h"

std::vector<GameObject*> Engine::gameObjects;
int Engine::gameObjectCount = 0;
float lastTick = 0;

ProfilerBenchmark* engineLoopBenchmark = new ProfilerBenchmark("Engine loop");
ProfilerBenchmark* gameLoopBenchmark = new ProfilerBenchmark("Game loop");
ProfilerBenchmark* componentsUpdateBenchmark = new ProfilerBenchmark("Components update");
ProfilerBenchmark* drawIDrawablesBenchmark = new ProfilerBenchmark("Draw");
bool Engine::componentsListDirty = true;
std::vector<Component*> Engine::orderedComponents;
int Engine::componentsCount = 0;

/// <summary>
/// Init engine
/// </summary>
/// <returns></returns>
int Engine::Init(const std::string exePath)
{
	/* Initialize libraries */
	Debug::Init();
	File::InitFileSystem(exePath);
	if (Window::InitWindow() != 0 || UiManager::Init() != 0 || Audio::Init() != 0) {
		return -1;
	}
	InputSystem::Init();
	SpriteManager::Init();
	AssetManager::Init();

	//Init random
	srand(time(NULL));

	return 0;
}

/// <summary>
/// Update all components
/// </summary>
void Engine::UpdateComponents()
{
	//Update all gameobjects components
	if (componentsListDirty)
	{
		componentsListDirty = false;
		orderedComponents.clear();

		std::vector<Component*> orderedComponentsToInit;
		componentsCount = 0;
		int componentsToInitCount = 0;
		for (int gIndex = 0; gIndex < gameObjectCount; gIndex++)
		{
			GameObject* gameObjectToCheck = gameObjects[gIndex];
			if (gameObjectToCheck->GetActive())
			{
				int componentCount = gameObjectToCheck->components.size();
				bool placeFound = false;
				for (int cIndex = 0; cIndex < componentCount; cIndex++)
				{
					Component* componentToCheck = gameObjectToCheck->components[cIndex];

					if (componentToCheck->GetIsEnabled())
					{
						for (int i = 0; i < componentsCount; i++)
						{
							//Check if the checked has a higher priority (lower value) than the component in the list
							if (componentToCheck->updatePriority <= orderedComponents[i]->updatePriority)
							{
								orderedComponents.insert(orderedComponents.begin() + i, componentToCheck);
								placeFound = true;
								break;
							}
						}
						//if the priority is lower than all components's priorities in the list, add it the end of the list
						if (!placeFound)
						{
							orderedComponents.push_back(componentToCheck);
						}
						componentsCount++;
					}
				}
			}
		}
		//return;
		//Find uninitiated components and order them
		for (int i = 0; i < componentsCount; i++)
		{
			Component* componentToCheck = orderedComponents[i];
			if (!orderedComponents[i]->initiated)
			{
				bool placeFound = false;
				for (int componentToInitIndex = 0; componentToInitIndex < componentsToInitCount; componentToInitIndex++)
				{
					//Check if the checked has a higher priority (lower value) than the component in the list
					if (componentToCheck->updatePriority <= orderedComponentsToInit[componentToInitIndex]->updatePriority)
					{
						orderedComponentsToInit.insert(orderedComponentsToInit.begin() + componentToInitIndex, componentToCheck);
						placeFound = true;
						break;
					}
				}
				//if the priority is lower than all components's priorities in the list, add it the end of the list
				if (!placeFound)
				{
					orderedComponentsToInit.push_back(componentToCheck);
				}
				componentsToInitCount++;
			}
		}

		//Init components
		for (int i = 0; i < componentsToInitCount; i++)
		{
			orderedComponentsToInit[i]->Awake();
		}

		for (int i = 0; i < componentsToInitCount; i++)
		{
			orderedComponentsToInit[i]->Start();
			orderedComponentsToInit[i]->initiated = true;
		}
	}
	//Update components
	for (int i = 0; i < componentsCount; i++)
	{
		orderedComponents[i]->Update();
	}
}

/// <summary>
/// Engine loop
/// </summary>
void Engine::Loop()
{
	Debug::Print("Initiating game...");
	//Game* game = new Game();
	PathFinding* game = new PathFinding();
	game->Init();
	Debug::Print("---- Game initiated ----");

	bool running = true;

	while (running)
	{
		engineLoopBenchmark->Start();

		Time::UpdateTime();

		SDL_Event event;
		InputSystem::ClearInputs();
		while (SDL_PollEvent(&event))
		{
			InputSystem::Read(event);
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					Window::OnResize(event.window.data1, event.window.data2);
				}
				break;
			default:
				break;
			}
		}

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

		gameLoopBenchmark->Start();
		game->Loop();
		gameLoopBenchmark->Stop();

		componentsUpdateBenchmark->Start();
		UpdateComponents();
		componentsUpdateBenchmark->Stop();

		AssetManager::ResetMaterialsUpdates();

		drawIDrawablesBenchmark->Start();
		Graphics::DrawAllDrawable();
		drawIDrawablesBenchmark->Stop();
		glPolygonMode(GL_FRONT, GL_FILL);

		/*if (InputSystem::GetKeyDown(A))
		{
			EngineSettings::isWireframe = !EngineSettings::isWireframe;
		}*/

		std::string debugText = std::string("Wireframe (A): ") + (EngineSettings::isWireframe ? "True" : "False");

		std::string fpsText = std::to_string((int)(1 / Time::GetUnscaledDeltaTime())) + " fps";

		//Draw screen tester
		/*UiManager::RenderTextCanvas("Left", 0, 0.5, angleT, 0.5f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Right, V_Center, *AssetManager::GetShader(7));
		UiManager::RenderTextCanvas("Right", 1, 0.5, angleT, 0.5f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Left, V_Center, *AssetManager::GetShader(7));
		UiManager::RenderTextCanvas("Top Left", 0, 0, angleT, 0.5f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Right, V_Bottom, *AssetManager::GetShader(7));
		UiManager::RenderTextCanvas("Top Right", 1, 0, angleT, 0.5f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Left, V_Bottom, *AssetManager::GetShader(7));
		UiManager::RenderTextCanvas("Bottom Left", 0, 1, angleT, 0.5f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Right, V_Top, *AssetManager::GetShader(7));
		UiManager::RenderTextCanvas("Bottom Right", 1, 1, angleT, 0.5f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Left, V_Top, *AssetManager::GetShader(7));
		UiManager::RenderTextCanvas("Center", 0.5, 0.5, angleT, 0.5f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Center, V_Center, *AssetManager::GetShader(7));*/

		//Draw performance data
		std::string performanceDebugText = "DrawCall Count: " + std::to_string(Performance::GetDrawCallCount()) + "\n";
		performanceDebugText += std::string("Updated Materials: ") + std::to_string(Performance::GetUpdatedMaterialCount()) + "\n";

		if (Performance::IsProfilerEnabled())
		{
			performanceDebugText += std::string("\n--- Profiler (microseconds) --- ") + "\n";
			//Add profiler text
			for (const auto& kv : Performance::profilerList)
			{
				performanceDebugText += kv.first + ": " + std::to_string(kv.second->GetValue()) + ", avg: " + std::to_string(kv.second->average) + "\n";
			}
		}

		//UiManager::RenderTextCanvas(debugText, 1, 0, 0, 0.7f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Left, V_Bottom, *AssetManager::GetShader(7));
		//UiManager::RenderTextCanvas(fpsText, 0.5, 0, 0, 0.7f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Center, V_Bottom, *AssetManager::GetShader(7));
		//UiManager::RenderTextCanvas(performanceDebugText, 0, 0, 0, 0.7f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Right, V_Bottom, *AssetManager::GetShader(7));
		//std::cout << performanceDebugText << std::endl;
		Vector4 lineColor = Vector4(0.5f, 1.0f, 0.2f, 1.0f);

		/*Vector2 arrowStart = Vector2(2, 0);
		Vector2 arrowEnd = Vector2(0, -2);
		Vector2 dir = (arrowEnd - arrowStart).normalize();
		float angle = atan2(dir.x, dir.y);
		Vector2 leftEnd = arrowEnd - dir / 2 + Vector2(-cos(angle) /4, sin(angle) / 4);
		Vector2 rightEnd = arrowEnd - dir / 2 + Vector2(cos(angle) / 4, -sin(angle) / 4);

		SpriteManager::Render2DLine(arrowStart, arrowEnd, 0.05f, lineColor, AssetManager::GetMaterialByName("2D Standard")); //Line
		SpriteManager::Render2DLine(arrowEnd, leftEnd, 0.05f, lineColor, AssetManager::GetMaterialByName("2D Standard"));//Left
		SpriteManager::Render2DLine(arrowEnd, rightEnd, 0.05f, lineColor, AssetManager::GetMaterialByName("2D Standard"));//Right*/

		//SpriteManager::Render2DLine(arrowEnd, arrowEnd, 0.05f, lineColor, AssetManager::GetMaterialByName("2D Standard"));

		/*SpriteManager::Render2DLine(Vector2(0, 0), Vector2(2, 2), 0.2f, lineColor, AssetManager::GetMaterialByName("2D Standard"));
		SpriteManager::Render2DLine(Vector2(0, 0), Vector2(2, 0), 0.2f, lineColor, AssetManager::GetMaterialByName("2D Standard"));
		SpriteManager::Render2DLine(Vector2(0, 0), Vector2(2, -2), 0.2f, lineColor, AssetManager::GetMaterialByName("2D Standard"));

		SpriteManager::Render2DLine(Vector2(0, 0), Vector2(-2, 2), 0.2f, lineColor, AssetManager::GetMaterialByName("2D Standard"));
		SpriteManager::Render2DLine(Vector2(0, 0), Vector2(-2, 0), 0.2f, lineColor, AssetManager::GetMaterialByName("2D Standard"));
		SpriteManager::Render2DLine(Vector2(0, 0), Vector2(-2, -2), 0.2f, lineColor, AssetManager::GetMaterialByName("2D Standard"));

		SpriteManager::Render2DLine(Vector2(0, 0), Vector2(0, -2), 0.2f, lineColor, AssetManager::GetMaterialByName("2D Standard"));
		SpriteManager::Render2DLine(Vector2(0, 0), Vector2(0, 2), 0.2f, lineColor, AssetManager::GetMaterialByName("2D Standard"));*/

		engineLoopBenchmark->Stop();
		Window::UpdateScreen();
		Performance::Update();
		Performance::ResetCounters();
	}
	delete game;
}

/// <summary>
/// 
/// </summary>
/// <param name="gameObject"></param>
void Engine::AddGameObject(GameObject* gameObject)
{
	gameObjects.push_back(gameObject);
	gameObjectCount++;
	//componentsListDirty = true;
}

/// <summary>
/// 
/// </summary>
/// <returns></returns>
std::vector<GameObject*> Engine::GetGameObjects()
{
	return std::vector<GameObject*>(gameObjects);
}
