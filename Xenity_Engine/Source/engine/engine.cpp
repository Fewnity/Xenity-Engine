#include "engine.h"
#include "../xenity.h"
// #include "../xenity_editor.h"
//  #include <SDL2/SDL.h>
//  #include "../rts_game/game.h"
#include "../game_test/game.h"
//  #include "../pathfinding_test/pathfinding.h"
//  #include "../3d_scene/gameOld.h"
//  #include "../rendering_test/rendering_test_2d.h"
//  #include <imgui/imgui_impl_sdl2.h>
//  #include <imgui/imgui_impl_opengl3.h>
#include "graphics/renderer/renderer.h"
#include "graphics/renderer/renderer_opengl.h"
#include "file_system/mesh_loader/wavefront_loader.h"

#ifdef __PSP__
#include "../psp/gu2gl.h"
#include "../psp/callbacks.h"
#elif __vita__
#include <psp2/kernel/processmgr.h>
#endif

std::vector<GameObject *> Engine::gameObjects;
GameObject *Engine::selectedGameObject = nullptr;

int Engine::gameObjectCount = 0;
float lastTick = 0;

ProfilerBenchmark *engineLoopBenchmark = nullptr;
ProfilerBenchmark *gameLoopBenchmark = nullptr;
ProfilerBenchmark *componentsUpdateBenchmark = nullptr;
ProfilerBenchmark *drawIDrawablesBenchmark = nullptr;

bool Engine::componentsListDirty = true;
bool Engine::drawOrderListDirty = true;
std::vector<Component *> Engine::orderedComponents;
int Engine::componentsCount = 0;
Renderer *Engine::renderer = nullptr;

/// <summary>
/// Init engine
/// </summary>
/// <returns></returns>
int Engine::Init(const std::string exePath)
{
#ifdef __PSP__
	SetupCallbacks();
#endif

	/* Initialize libraries */
	Debug::Init();
	Performance::Init();
	new FileSystem();
	FileSystem::fileSystem->InitFileSystem(exePath);
	// FileSystem::InitFileSystem(exePath);

	renderer = new RendererOpengl();
	renderer->Init();

	Window::Init();

#ifdef __PSP__
	Window::SetResolution(PSP_SCR_WIDTH, PSP_SCR_HEIGHT);
#elif __vita__
	Window::SetResolution(960, 544);
#else
	Debug::Print("-------- RESOLUTION Not implemented --------");
	//Window::SetResolution(-1, -1);
	Window::SetResolution(1280, 720);
#endif

	Debug::Print("-------- Audio Not implemented --------");

	// if (Window::InitWindow() != 0 || UiManager::Init() != 0 || Audio::Init() != 0)
	// {
	// 	return -1;
	// }
	Graphics::Init();
	InputSystem::Init();
	SpriteManager::Init();
	MeshManager::Init();
	TextManager::Init();
	TextManager::CreateFont("Roboto-Regular.ttf");
	// TextManager::CreateFont("Minecraftia-Regular.ttf");
	AssetManager::Init();
	Debug::Print("-------- Editor UI Not implemented --------");
	// EditorUI::Init();
	Time::Init();

	// Init random
	srand((unsigned int)time(NULL));

	Debug::Print("-------- Engine fully initiated --------");

	renderer->SetClearColor(Color::CreateFromRGBAFloat(0.529f, 0.808f, 0.922f, 1));

	engineLoopBenchmark = new ProfilerBenchmark("Engine loop");
	gameLoopBenchmark = new ProfilerBenchmark("Game loop");
	componentsUpdateBenchmark = new ProfilerBenchmark("Components update");
	drawIDrawablesBenchmark = new ProfilerBenchmark("Draw");

	return 0;
}

void Engine::Stop()
{
	renderer->Stop();
#ifdef __vita__
	sceKernelExitProcess(0);
#endif
}

/// <summary>
/// Update all components
/// </summary>
void Engine::UpdateComponents()
{
	// Update all gameobjects components
	if (componentsListDirty)
	{
		componentsListDirty = false;
		orderedComponents.clear();

		std::vector<Component *> orderedComponentsToInit;
		componentsCount = 0;
		int componentsToInitCount = 0;
		for (int gIndex = 0; gIndex < gameObjectCount; gIndex++)
		{
			GameObject *gameObjectToCheck = gameObjects[gIndex];
			if (gameObjectToCheck->GetActive())
			{
				int componentCount = (int)gameObjectToCheck->components.size();
				bool placeFound = false;
				for (int cIndex = 0; cIndex < componentCount; cIndex++)
				{
					Component *componentToCheck = gameObjectToCheck->components[cIndex];

					if (componentToCheck->GetIsEnabled())
					{
						for (int i = 0; i < componentsCount; i++)
						{
							// Check if the checked has a higher priority (lower value) than the component in the list
							if (componentToCheck->updatePriority <= orderedComponents[i]->updatePriority)
							{
								orderedComponents.insert(orderedComponents.begin() + i, componentToCheck);
								placeFound = true;
								break;
							}
						}
						// if the priority is lower than all components's priorities in the list, add it the end of the list
						if (!placeFound)
						{
							orderedComponents.push_back(componentToCheck);
						}
						componentsCount++;
					}
				}
			}
		}

		// Find uninitiated components and order them
		for (int i = 0; i < componentsCount; i++)
		{
			Component *componentToCheck = orderedComponents[i];
			if (!orderedComponents[i]->initiated)
			{
				bool placeFound = false;
				for (int componentToInitIndex = 0; componentToInitIndex < componentsToInitCount; componentToInitIndex++)
				{
					// Check if the checked has a higher priority (lower value) than the component in the list
					if (componentToCheck->updatePriority <= orderedComponentsToInit[componentToInitIndex]->updatePriority)
					{
						orderedComponentsToInit.insert(orderedComponentsToInit.begin() + componentToInitIndex, componentToCheck);
						placeFound = true;
						break;
					}
				}
				// if the priority is lower than all components's priorities in the list, add it the end of the list
				if (!placeFound)
				{
					orderedComponentsToInit.push_back(componentToCheck);
				}
				componentsToInitCount++;
			}
		}

		// Init components
		for (int i = 0; i < componentsToInitCount; i++)
		{
			orderedComponentsToInit[i]->Start();
			orderedComponentsToInit[i]->initiated = true;
		}
	}

	// Update components
	for (int i = 0; i < componentsCount; i++)
	{
		orderedComponents[i]->Update();
	}
}

void Engine::SetSelectedGameObject(GameObject *newSelected)
{
	selectedGameObject = newSelected;
}

/// <summary>
/// Engine loop
/// </summary>
void Engine::Loop()
{
	Debug::Print("Initiating game...");
	Game *game = new Game();

	game->Start();

	Debug::Print("-------- Game initiated --------");

	bool running = true;

	while (running)
	{
		engineLoopBenchmark->Start();
		// Update time and inputs
		Time::UpdateTime();
		InputSystem::ClearInputs();
		//InputSystem::Read();

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			//ImGui_ImplSDL2_ProcessEvent(&event);
			InputSystem::Read(event);
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					//Window::OnResize(event.window.data1, event.window.data2);
				}
				break;
			default:
				break;
			}
		}

		//EditorUI::NewFrame();

		gameLoopBenchmark->Start();
		// Game loop
		game->Update();
		gameLoopBenchmark->Stop();

		componentsUpdateBenchmark->Start();
		// Update all components
		UpdateComponents();
		componentsUpdateBenchmark->Stop();

		drawIDrawablesBenchmark->Start();
		Window::UpdateScreen();
		//Debug::Print("UPDATE");
		Graphics::DrawAllDrawable();
		drawIDrawablesBenchmark->Stop();

		// Reset moved state of all transforms
		for (int i = 0; i < Engine::gameObjectCount; i++)
		{
			GameObject *go = gameObjects[i];
			if (go->GetTransform()->movedLastFrame)
			{
				go->GetTransform()->movedLastFrame = false;
			}
		}
		engineLoopBenchmark->Stop();

		// 	EditorUI::DrawProfiler();
		// 	EditorUI::DrawInspector();
		// 	EditorUI::DrawHierarchy();

		// 	ImGui::Render();
		// 	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		Performance::Update();
		Performance::ResetCounters();
	}

	// delete game;
}

/// <summary>
///
/// </summary>
/// <param name="gameObject"></param>
void Engine::AddGameObject(GameObject *gameObject)
{
	gameObjects.push_back(gameObject);
	gameObjectCount++;
	// componentsListDirty = true;
}

/// <summary>
///
/// </summary>
/// <returns></returns>
std::vector<GameObject *> Engine::GetGameObjects()
{
	return std::vector<GameObject *>(gameObjects);
}

void Destroy(GameObject *gameObject)
{
	delete gameObject;
}

void Destroy(Component *component)
{
	component->GetGameObject()->RemoveComponent(component);
}
