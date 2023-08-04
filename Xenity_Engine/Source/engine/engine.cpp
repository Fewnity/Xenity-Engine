// #define EDITOR
#include "engine.h"
#include "engine_settings.h"
#include "../xenity.h"
#include "graphics/renderer/renderer.h"
#include "graphics/renderer/renderer_opengl.h"
#include "file_system/mesh_loader/wavefront_loader.h"
#include "audio/audio_manager.h"
#include "network/network.h"
#include "dynamic_lib/dynamic_lib.h"

#if defined(EDITOR)
#include "dynamic_lib/dynamic_lib.h"
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include "../editor/editor.h"
#include "../xenity_editor.h"
#else
#include "../game_test/game.h"
#endif
#include "game_interface.h"
// #include "../game_test/game.h"
#include "class_registry/class_registry.h"

#ifdef __PSP__
#include "../psp/gu2gl.h"
#include "../psp/callbacks.h"
#include <psppower.h>
#include <pspge.h>
#elif __vita__
#include <psp2/kernel/processmgr.h>
#include <psp2/power.h>
#endif

#include "asset_management/project_manager.h"

std::vector<std::shared_ptr<GameObject>> Engine::gameObjects;
std::vector<std::shared_ptr<GameObject>> Engine::gameObjectsEditor;
std::vector<std::weak_ptr<GameObject>> Engine::gameObjectsToDestroy;
std::vector<std::weak_ptr<Component>> Engine::componentsToDestroy;

std::weak_ptr<GameObject> Engine::selectedGameObject;
FileReference* Engine::selectedFileReference = nullptr;
ProjectDirectory* Engine::currentProjectDirectory = nullptr;

int Engine::gameObjectCount = 0;
int Engine::gameObjectEditorCount = 0;

ProfilerBenchmark *engineLoopBenchmark = nullptr;
ProfilerBenchmark *gameLoopBenchmark = nullptr;
ProfilerBenchmark *componentsUpdateBenchmark = nullptr;
ProfilerBenchmark *drawIDrawablesBenchmark = nullptr;

bool Engine::componentsListDirty = true;
bool Engine::drawOrderListDirty = true;
std::vector<std::weak_ptr<Component>> Engine::orderedComponents;
int Engine::componentsCount = 0;
Renderer *Engine::renderer = nullptr;
bool Engine::valueFree = true;
bool Engine::isRunning = true;
GameInterface* Engine::game = nullptr;
GameState Engine::gameState = Stopped;

/// <summary>
/// Init engine
/// </summary>
/// <returns></returns>
int Engine::Init(const std::string exePath)
{
#if defined(__PSP__)
	SetupCallbacks();
	scePowerSetClockFrequency(333, 333, 166);

#elif defined(__vita__)
	scePowerSetArmClockFrequency(444);
	scePowerSetBusClockFrequency(222);
	scePowerSetGpuClockFrequency(222);
	scePowerSetGpuXbarClockFrequency(166);
#endif
	new FileSystem();
	FileSystem::fileSystem->InitFileSystem(exePath);
	Debug::Init();

	RegisterEngineComponents();

	/* Initialize libraries */
	NetworkManager::Init();

	Performance::Init();
	// FileSystem::InitFileSystem(exePath);

	Engine::renderer = new RendererOpengl();
	Engine::renderer->Init();

#ifdef __PSP__
	Window::SetResolution(PSP_SCR_WIDTH, PSP_SCR_HEIGHT);
#elif __vita__
	Window::SetResolution(960, 544);
#else
	Window::SetResolution(1280, 720);
#endif
	Window::Init();
	Engine::renderer->Setup();
#if defined(__PSP__)
	if (sceGeEdramSetSize(0x400000) == 0)
	{
		Debug::Print("---------------- sceGeEdramSetSize ok --------");
	}
	else
	{
		Debug::Print("---------------- sceGeEdramSetSize ERROR --------");
	}
#endif
	Graphics::Init();
	Debug::Print("-------- Audio Not implemented --------");

	InputSystem::Init();
	SpriteManager::Init();
	MeshManager::Init();
	TextManager::Init();
	TextManager::CreateFont("Roboto Regular.ttf");
	// TextManager::CreateFont("Minecraftia-Regular.ttf");
	AssetManager::Init();
	AudioManager::Init();
	Debug::Print("-------- Editor UI Not implemented --------");
#if defined(EDITOR)
	EditorUI::Init();
#endif
	Time::Init();

	// Init random
	srand((unsigned int)time(NULL));
#if defined(EDITOR)
	Editor::Start();
#endif
	Debug::Print("-------- Engine fully initiated --------\n");

	renderer->SetClearColor(Color::CreateFromRGBAFloat(0.529f, 0.808f, 0.922f, 1));

	engineLoopBenchmark = new ProfilerBenchmark("Engine loop");
	gameLoopBenchmark = new ProfilerBenchmark("Game loop");
	componentsUpdateBenchmark = new ProfilerBenchmark("Components update");
	drawIDrawablesBenchmark = new ProfilerBenchmark("Draw");

	return 0;
}

void Engine::RegisterEngineComponents()
{
	ClassRegistry::AddComponentClass("Light", [](std::shared_ptr<GameObject> go)
		{ return go->AddComponent<Light>(); });
	ClassRegistry::AddComponentClass("Camera", [](std::shared_ptr<GameObject> go)
		{ return go->AddComponent<Camera>(); });
	ClassRegistry::AddComponentClass("TextRendererCanvas", [](std::shared_ptr<GameObject> go)
		{ return go->AddComponent<TextRendererCanvas>(); });
	ClassRegistry::AddComponentClass("TextRenderer", [](std::shared_ptr<GameObject> go)
		{ return go->AddComponent<TextRenderer>(); });
	ClassRegistry::AddComponentClass("MeshRenderer", [](std::shared_ptr<GameObject> go)
		{ return go->AddComponent<MeshRenderer>(); });
	ClassRegistry::AddComponentClass("Tilemap", [](std::shared_ptr<GameObject> go)
		{ return go->AddComponent<Tilemap>(); });
	ClassRegistry::AddComponentClass("SpriteRenderer", [](std::shared_ptr<GameObject> go)
		{ return go->AddComponent<SpriteRenderer>(); });
	ClassRegistry::AddComponentClass("LineRenderer", [](std::shared_ptr<GameObject> go)
		{ return go->AddComponent<LineRenderer>(); });
	ClassRegistry::AddComponentClass("AudioSource", [](std::shared_ptr<GameObject> go)
		{ return go->AddComponent<AudioSource>(); });
	ClassRegistry::AddComponentClass("TestComponent", [](std::shared_ptr<GameObject> go)
		{ return go->AddComponent<TestComponent>(); });
}

void Engine::SetGameState(GameState _gameState)
{
	gameState = _gameState;
}

void Engine::Stop()
{
	renderer->Stop();
#ifdef __vita__
	sceKernelExitProcess(0);
#endif
}

void Engine::Quit()
{
	isRunning = false;
}

/// <summary>
/// Update all components
/// </summary>
void Engine::UpdateComponents()
{
	if (gameState == GameState::Playing) 
	{
		// Update all gameobjects components
		if (componentsListDirty)
		{
			componentsListDirty = false;
			orderedComponents.clear();

			std::vector<std::weak_ptr<Component>> orderedComponentsToInit;
			componentsCount = 0;
			int componentsToInitCount = 0;
			for (int gIndex = 0; gIndex < gameObjectCount; gIndex++)
			{
				std::weak_ptr<GameObject> weakGameObjectToCheck = gameObjects[gIndex];
				if (auto gameObjectToCheck = weakGameObjectToCheck.lock())
				{
					if (gameObjectToCheck->GetActive())
					{
						int componentCount = (int)gameObjectToCheck->components.size();
						bool placeFound = false;
						for (int cIndex = 0; cIndex < componentCount; cIndex++)
						{
							std::weak_ptr<Component> weakComponentToCheck = gameObjectToCheck->components[cIndex];
							if (auto componentToCheck = weakComponentToCheck.lock())
							{
								if (componentToCheck->GetIsEnabled())
								{
									for (int i = 0; i < componentsCount; i++)
									{
										// Check if the checked has a higher priority (lower value) than the component in the list
										if (componentToCheck->updatePriority <= orderedComponents[i].lock()->updatePriority)
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
				}
			}

			// Find uninitiated components and order them
			for (int i = 0; i < componentsCount; i++)
			{
				if (auto componentToCheck = orderedComponents[i].lock())
				{
					if (!componentToCheck->initiated)
					{
						bool placeFound = false;
						for (int componentToInitIndex = 0; componentToInitIndex < componentsToInitCount; componentToInitIndex++)
						{
							// Check if the checked has a higher priority (lower value) than the component in the list
							if (componentToCheck->updatePriority <= orderedComponentsToInit[componentToInitIndex].lock()->updatePriority)
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
			}

			// Init components
			for (int i = 0; i < componentsToInitCount; i++)
			{
				orderedComponentsToInit[i].lock()->Start();
				orderedComponentsToInit[i].lock()->initiated = true;
			}
		}

		// Update components
		for (int i = 0; i < componentsCount; i++)
		{
			if (auto component = orderedComponents[i].lock())
			{
				if (component->GetGameObject()->GetLocalActive())
				{
					component->Update();
				}
			}
			else
			{
				orderedComponents.erase(orderedComponents.begin() + i);
				i--;
				componentsCount--;
			}
		}

		int gameObjectToDestroyCount = (int)gameObjectsToDestroy.size();
		for (int i = 0; i < gameObjectToDestroyCount; i++)
		{
			for (int gIndex = 0; gIndex < gameObjectCount; gIndex++)
			{
				std::weak_ptr<GameObject> gameObjectToCheck = gameObjects[gIndex];
				if (gameObjectToCheck.lock() == gameObjectsToDestroy[i].lock())
				{
					gameObjects.erase(gameObjects.begin() + gIndex);
					break;
				}
			}
			gameObjectCount--;
		}
		gameObjectsToDestroy.clear();

		int componentToDestroyCount = (int)componentsToDestroy.size();
		for (int i = 0; i < componentToDestroyCount; i++)
		{
			if (auto component = componentsToDestroy[i].lock())
			{
				component->GetGameObject()->InternalDestroyComponent(component);
			}
		}
		componentsToDestroy.clear();
	}
}

void Engine::SetSelectedGameObject(std::weak_ptr<GameObject> newSelected)
{
	selectedGameObject = newSelected;
}

/// <summary>
/// Engine loop
/// </summary>
void Engine::Loop()
{
	Debug::Print("-------- Initiating game --------");
#if defined(_WIN32) || defined(_WIN64)
#if defined(EDITOR)
	DynamicLibrary::LoadGameLibrary("game_editor");
#else
	DynamicLibrary::LoadGameLibrary("game");
#endif
	game = DynamicLibrary::CreateGame();
#else
	//game = new Game();
#endif
	//game = new Game();

	valueFree = false;
	if (game)
		game->Start();
	valueFree = true;

	Debug::Print("-------- Game initiated --------");


	while (isRunning)
	{
		engineLoopBenchmark->Start();
		// Update time and inputs
		Time::UpdateTime();
		InputSystem::ClearInputs();
		NetworkManager::Update();
		Debug::Update();

#if defined(_WIN32) || defined(_WIN64)
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
#if defined(EDITOR)
			ImGui_ImplSDL2_ProcessEvent(&event);
#endif
			InputSystem::Read(event);
			switch (event.type)
			{
			case SDL_QUIT:
				isRunning = false;
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					if(event.window.windowID == SDL_GetWindowID(Window::window))
						Window::SetResolution(event.window.data1, event.window.data2);
				}
				else if (event.window.event == SDL_WINDOWEVENT_CLOSE) 
				{
					isRunning = false;
				}
				break;
			default:
				break;
			}
		}
#else
		InputSystem::Read();
#endif

		// Game loop
		gameLoopBenchmark->Start();

		valueFree = false;
#if defined(EDITOR)
		Editor::Update();
#endif
		if (game)
			game->Update();
		valueFree = true;

		gameLoopBenchmark->Stop();

		// Update all components
		componentsUpdateBenchmark->Start();

		valueFree = false;
		UpdateComponents();
		valueFree = true;

		componentsUpdateBenchmark->Stop();

		AudioManager::Update();

		drawIDrawablesBenchmark->Start();
		Window::UpdateScreen();

		Graphics::DrawAllDrawable();
		drawIDrawablesBenchmark->Stop();

		// Reset moved state of all transforms
		for (int i = 0; i < Engine::gameObjectCount; i++)
		{
			std::weak_ptr<GameObject> weakGO = gameObjects[i];
			if (auto go = weakGO.lock())
			{
				if (go->GetTransform()->movedLastFrame)
				{
					go->GetTransform()->movedLastFrame = false;
				}
			}
		}
		engineLoopBenchmark->Stop();

#if defined(EDITOR)
		Editor::Draw();
#endif

		Performance::Update();
		Performance::ResetCounters();
	}

	delete game;
}

void Engine::BuildGame() 
{
#if defined(_WIN32) || defined(_WIN64)
	DynamicLibrary::CompileGame(BuildType::BuildGame);
#endif
}

void Engine::CompileGame() 
{
#if defined(_WIN32) || defined(_WIN64)
	delete game;
	game = nullptr;
	EmptyScene();

	ClassRegistry::Reset();
	RegisterEngineComponents();
	DynamicLibrary::UnloadGameLibrary();
	DynamicLibrary::CompileGame(EditorHotReloading);
	DynamicLibrary::LoadGameLibrary("game_editor");
	game = DynamicLibrary::CreateGame();
	if (game)
	{
		Debug::Print("Game compilation done");
		game->Start();
	}
	else
	{
		Debug::PrintError("Game compilation failed");
	}
#endif
}

void Engine::EmptyScene()
{
	Graphics::orderedIDrawable.clear();
	Graphics::usedCamera.reset(); //TODO RE ENABLE THIS

	orderedComponents.clear();
	gameObjectsToDestroy.clear();
	componentsToDestroy.clear();
	gameObjects.clear();
	componentsCount = 0;
	gameObjectCount = 0;
	Engine::selectedGameObject.reset();
}

/// <summary>
///
/// </summary>
/// <param name="gameObject"></param>
void Engine::AddGameObject(std::shared_ptr<GameObject> gameObject)
{
	gameObjects.push_back(gameObject);
	gameObjectCount++;
}

/// <summary>
///
/// </summary>
/// <param name="gameObject"></param>
void Engine::AddGameObjectEditor(std::shared_ptr<GameObject> gameObject)
{
	gameObjectsEditor.push_back(gameObject);
	gameObjectEditorCount++;
}

/// <summary>
///
/// </summary>
/// <returns></returns>
std::vector<std::shared_ptr<GameObject>> Engine::GetGameObjects()
{
	return std::vector<std::shared_ptr<GameObject>>(Engine::gameObjects);
}

void DestroyGameObjectAndChild(std::weak_ptr<GameObject> gameObject)
{
	auto gameObjectLock = gameObject.lock();
	Engine::gameObjectsToDestroy.push_back(gameObject);
	gameObjectLock->waitingForDestroy = true;
	int childCount = gameObjectLock->GetChildrenCount();
	for (int i = 0; i < childCount; i++)
	{
		DestroyGameObjectAndChild(gameObjectLock->children[i]);
	}
}

void Destroy(std::weak_ptr<GameObject> gameObject)
{
	if (!gameObject.lock()->waitingForDestroy)
	{
		DestroyGameObjectAndChild(gameObject);
	}
}

void Destroy(std::weak_ptr<Component> weakComponent)
{
	if (auto component = weakComponent.lock())
		component->GetGameObject()->RemoveComponent(weakComponent);
}
