#include "engine.h"
#include "../xenity.h"
// #include "../xenity_editor.h"
//  #include <SDL2/SDL.h>
//  #include "../rts_game/game.h"
//  #include "../pathfinding_test/pathfinding.h"
//  #include "../3d_scene/gameOld.h"
//  #include "../rendering_test/rendering_test_2d.h"
//  #include <imgui/imgui_impl_sdl2.h>
//  #include <imgui/imgui_impl_opengl3.h>
#include "graphics/renderer/renderer.h"
#include "graphics/renderer/renderer_opengl.h"

#ifdef __PSP__
#include "../psp/gu2gl.h"
#endif

std::vector<GameObject *> Engine::gameObjects;
GameObject *Engine::selectedGameObject = nullptr;

int Engine::gameObjectCount = 0;
float lastTick = 0;

// ProfilerBenchmark *engineLoopBenchmark = new ProfilerBenchmark("Engine loop");
// ProfilerBenchmark *gameLoopBenchmark = new ProfilerBenchmark("Game loop");
// ProfilerBenchmark *componentsUpdateBenchmark = new ProfilerBenchmark("Components update");
// ProfilerBenchmark *drawIDrawablesBenchmark = new ProfilerBenchmark("Draw");
bool Engine::componentsListDirty = true;
bool Engine::drawOrderListDirty = true;
std::vector<Component *> Engine::orderedComponents;
int Engine::componentsCount = 0;
Renderer *Engine::renderer = nullptr;

GameObject *cameraGO = nullptr;

/// <summary>
/// Init engine
/// </summary>
/// <returns></returns>
int Engine::Init(const std::string exePath)
{
	/* Initialize libraries */
	Debug::Init();
	FileSystem::InitFileSystem(exePath);

	renderer = new RendererOpengl();

	renderer->Init();

#ifdef __PSP__
	Window::SetResolution(PSP_SCR_WIDTH, PSP_SCR_HEIGHT);
#elif __vita__
	Window::SetResolution(960, 544);
#else
	Debug::Print("-------- RESOLUTION Not implemented --------");
	Window::SetResolution(-1, -1);
#endif

	Debug::Print("-------- Window Not implemented --------");
	Debug::Print("-------- UI Manager Not implemented --------");
	Debug::Print("-------- Audio Not implemented --------");

	// if (Window::InitWindow() != 0 || UiManager::Init() != 0 || Audio::Init() != 0)
	// {
	// 	return -1;
	// }
	InputSystem::Init();
	// Debug::Print("-------- Sprite Manager Not implemented --------");
	SpriteManager::Init();
	UiManager::CreateFont("Roboto-Regular.ttf");
	// Debug::Print("-------- Asset Manager Not implemented --------");
	AssetManager::Init();
	Debug::Print("-------- Editor UI Not implemented --------");
	// EditorUI::Init();
	Time::Init();

	// Init random
	srand((unsigned int)time(NULL));

	Debug::Print("-------- Engine fully initiated --------");

	renderer->SetClearColor(Color::CreateFromRGBAFloat(0.529f, 0.808f, 0.922f, 1));

	return 0;
}

void Engine::Stop()
{
	renderer->Stop();
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

void GameInit()
{
	Texture *texture = new Texture();
	texture->Load("container.jpg", 0);
	Texture *texture2 = new Texture();
	texture2->Load("Stone512.jpg", 0);
	Texture *texture3 = new Texture();
	texture3->Load("Stone512.jpg", 0);
	Texture *texture4 = new Texture();
	texture4->Load("Stone512.jpg", 0);
	Texture *texture5 = new Texture();
	texture5->Load("Stone512.jpg", 0);
	Texture *texture6 = new Texture();
	texture6->Load("Stone512.jpg", 0);
	Texture *texture7 = new Texture();
	texture7->Load("Stone512.jpg", 0);

	// texture2->Load("Stone256.jpg", 1);
	cameraGO = GameObject::FindGameObjectByName("Camera");
	cameraGO->GetTransform()->SetPosition(Vector3(0, 2.81f, -10));
	cameraGO->GetTransform()->SetRotation(Vector3(0, 0, 0));

	GameObject *spriteGo0 = new GameObject();
	GameObject *spriteGo1 = new GameObject();
	GameObject *spriteGo2 = new GameObject();
	spriteGo0->GetTransform()->SetPosition(Vector3(0, 0, 0));
	spriteGo1->GetTransform()->SetPosition(Vector3(2.56f, 2.56f, 0));
	spriteGo2->GetTransform()->SetPosition(Vector3(0, 2.81f, -4.36f));

	SpriteRenderer *ps0 = spriteGo0->AddComponent<SpriteRenderer>();
	SpriteRenderer *ps1 = spriteGo1->AddComponent<SpriteRenderer>();
	SpriteRenderer *ps2 = spriteGo2->AddComponent<SpriteRenderer>();
	ps0->texture = texture7;
	ps1->texture = texture2;
	ps2->texture = texture;
}

void GameLoop()
{
#ifdef __PSP__
	// Rotate camera
	Vector3 rot = cameraGO->GetTransform()->GetRotation();
	rot.x += InputSystem::leftJoystick.y;
	rot.y += InputSystem::leftJoystick.x;
	cameraGO->GetTransform()->SetRotation(rot);
#else
	// Rotate camera
	Vector3 rot = cameraGO->GetTransform()->GetRotation();
	rot.x += InputSystem::rightJoystick.y * 1.5f;
	rot.y += InputSystem::rightJoystick.x * 1.5f;
	cameraGO->GetTransform()->SetRotation(rot);

	// Move camera
	Vector3 pos = cameraGO->GetTransform()->GetPosition();
	pos -= cameraGO->GetTransform()->GetForward() * (InputSystem::leftJoystick.y / 7.0f);
	pos -= cameraGO->GetTransform()->GetLeft() * (InputSystem::leftJoystick.x / 7.0f);
	cameraGO->GetTransform()->SetPosition(pos);
#endif
}

/// <summary>
/// Engine loop
/// </summary>
void Engine::Loop()
{
	Debug::Print("Initiating game...");
	// Game *game = new Game();
	// Game3D* game = new Game3D();
	// PathFinding* game = new PathFinding();
	// RenderingTest2D *game = new RenderingTest2D();

	// game->Init();
	GameInit();
	Debug::Print("-------- Game initiated --------");

	bool running = true;

	int lastTime = 0;

	while (running)
	{
		Time::UpdateTime();
		InputSystem::ClearInputs();
		InputSystem::Read();

		GameLoop();
		UpdateComponents();

		Graphics::DrawAllDrawable();

		for (int i = 0; i < Engine::gameObjectCount; i++)
		{
			GameObject *go = gameObjects[i];
			if (go->GetTransform()->movedLastFrame)
			{
				go->GetTransform()->movedLastFrame = false;
			}
		}

		if ((int)Time::GetTime() % 2 == 0 && (int)Time::GetTime() != lastTime)
		{
			lastTime = (int)Time::GetTime();
			Debug::Print("FPS: " + std::to_string(1.0f / Time::GetUnscaledDeltaTime()));
		}

		// Performance::Update();
		// 	Performance::ResetCounters();
	}

	// while (running)
	// {
	// 	engineLoopBenchmark->Start();

	// 	Time::UpdateTime();

	// 	SDL_Event event;
	// 	InputSystem::ClearInputs();
	// 	while (SDL_PollEvent(&event))
	// 	{
	// 		ImGui_ImplSDL2_ProcessEvent(&event);
	// 		InputSystem::Read(event);
	// 		switch (event.type)
	// 		{
	// 		case SDL_QUIT:
	// 			running = false;
	// 			break;
	// 		case SDL_WINDOWEVENT:
	// 			if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
	// 			{
	// 				Window::OnResize(event.window.data1, event.window.data2);
	// 			}
	// 			break;
	// 		default:
	// 			break;
	// 		}
	// 	}

	// 	// Clear the OpenGL window

	// 	renderer->Clear();

	// 	// Set wireframe
	// 	if (EngineSettings::isWireframe)
	// 	{
	// 		renderer->SetLineWidth(2);
	// 		renderer->SetPolygoneMode(true, Line);
	// 	}
	// 	else
	// 	{
	// 		renderer->SetPolygoneMode(true, Fill);
	// 	}

	// 	EditorUI::NewFrame();

	// 	gameLoopBenchmark->Start();
	// 	game->Loop();
	// 	gameLoopBenchmark->Stop();

	// 	componentsUpdateBenchmark->Start();
	// 	UpdateComponents();
	// 	componentsUpdateBenchmark->Stop();

	// 	AssetManager::ResetMaterialsUpdates();

	// 	drawIDrawablesBenchmark->Start();
	// 	Graphics::DrawAllDrawable();
	// 	drawIDrawablesBenchmark->Stop();
	// 	renderer->SetPolygoneMode(true, Fill);

	// 	for (int i = 0; i < Engine::gameObjectCount; i++)
	// 	{
	// 		GameObject *go = gameObjects[i];
	// 		if (go->GetTransform()->movedLastFrame)
	// 		{
	// 			go->GetTransform()->movedLastFrame = false;
	// 		}
	// 	}

	// 	if (InputSystem::GetKeyDown(A))
	// 	{
	// 		EngineSettings::isWireframe = !EngineSettings::isWireframe;
	// 	}

	// 	std::string debugText = std::string("Wireframe (A): ") + (EngineSettings::isWireframe ? "True" : "False");

	// 	// Draw screen tester
	// 	/*UiManager::RenderTextCanvas("Left", 0, 0.5, 0, 0.5f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Right, V_Center, *AssetManager::GetShader(7));
	// 	UiManager::RenderTextCanvas("Right", 1, 0.5, 0, 0.5f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Left, V_Center, *AssetManager::GetShader(7));
	// 	UiManager::RenderTextCanvas("Top Left", 0, 0, 0, 0.5f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Right, V_Bottom, *AssetManager::GetShader(7));
	// 	UiManager::RenderTextCanvas("Top Right", 1, 0, 0, 0.5f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Left, V_Bottom, *AssetManager::GetShader(7));
	// 	UiManager::RenderTextCanvas("Bottom Left", 0, 1, 0, 0.5f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Right, V_Top, *AssetManager::GetShader(7));
	// 	UiManager::RenderTextCanvas("Bottom Right", 1, 1, 0, 0.5f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Left, V_Top, *AssetManager::GetShader(7));
	// 	UiManager::RenderTextCanvas("Center", 0.5, 0.5, 0, 0.5f, 0, Vector3(0.5f, 0.0f, 0.2f), UiManager::fonts[0], H_Center, V_Center, *AssetManager::GetShader(7));*/

	// 	EditorUI::DrawProfiler();
	// 	EditorUI::DrawInspector();
	// 	EditorUI::DrawHierarchy();

	// 	ImGui::Render();
	// 	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// 	engineLoopBenchmark->Stop();
	// 	Window::UpdateScreen();
	// 	Performance::Update();
	// 	Performance::ResetCounters();
	// }
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
