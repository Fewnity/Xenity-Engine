// #define EDITOR
#include "engine.h"
#include "engine_settings.h"
#include "../xenity.h"
#include "graphics/renderer/renderer.h"
#include "graphics/renderer/renderer_opengl.h"
#include "audio/audio_manager.h"
#include "network/network.h"
#include "dynamic_lib/dynamic_lib.h"

#if defined(EDITOR)
#include "dynamic_lib/dynamic_lib.h"
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include "../editor/editor.h"
#include "../xenity_editor.h"
#include <glad/glad.h>
#include "../editor/ui/menus/inspector_menu.h"
#endif

#include "game_interface.h"
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
#include "scene_management/scene_manager.h"
#include "../editor/compiler.h"
#include "../unit_tests/unit_test_manager.h"
#include "graphics/2d_graphics/billboard_renderer.h"

std::vector<std::shared_ptr<FileReference>> Engine::threadLoadedFiles;
std::mutex Engine::threadLoadingMutex;

std::vector<std::shared_ptr<GameObject>> Engine::gameObjects;
std::vector<std::shared_ptr<GameObject>> Engine::gameObjectsEditor;
std::vector<std::weak_ptr<GameObject>> Engine::gameObjectsToDestroy;
std::vector<std::shared_ptr<Component>> Engine::componentsToDestroy;

std::weak_ptr<GameObject> Engine::selectedGameObject;
std::shared_ptr<FileReference> Engine::selectedFileReference = nullptr;
ProjectDirectory* Engine::currentProjectDirectory = nullptr;

int Engine::gameObjectCount = 0;
int Engine::gameObjectEditorCount = 0;

ProfilerBenchmark* engineLoopBenchmark = nullptr;
//ProfilerBenchmark* gameLoopBenchmark = nullptr;
ProfilerBenchmark* componentsUpdateBenchmark = nullptr;
ProfilerBenchmark* drawIDrawablesBenchmark = nullptr;
ProfilerBenchmark* editorUpdateBenchmark = nullptr;
ProfilerBenchmark* editorDrawBenchmark = nullptr;

bool Engine::componentsListDirty = true;
bool Engine::drawOrderListDirty = true;
std::vector<std::weak_ptr<Component>> Engine::orderedComponents;
int Engine::componentsCount = 0;
Renderer* Engine::renderer = nullptr;
bool Engine::canUpdateAudio = false;
bool Engine::isRunning = true;
GameInterface* Engine::game = nullptr;
GameState Engine::gameState = Stopped;

std::shared_ptr <Shader> Engine::shader = nullptr;
std::shared_ptr <Shader> Engine::unlitShader = nullptr;
std::shared_ptr<Material> Engine::standardMaterial = nullptr;
std::shared_ptr<Material> Engine::unlitMaterial = nullptr;

bool Engine::UseOpenGLFixedFunctions = false;

int Engine::Init()
{
#if defined(__PSP__)
	UseOpenGLFixedFunctions = true;
#endif
	// Init random
	srand((unsigned int)time(NULL));

	// Setup game console CPU speed
	SetMaxCpuSpeed();

	//------------------------------------------ Init File System
	new FileSystem();
	int fileSystemInitResult = FileSystem::fileSystem->InitFileSystem();
	if (fileSystemInitResult != 0)
	{
		return -1;
	}

	//------------------------------------------ Init Debug
	int debugInitResult = Debug::Init();
	if (debugInitResult != 0)
	{
		Debug::PrintWarning("-------- Debug init error code: " + std::to_string(debugInitResult) + " --------");
		//Not a critical module, do not stop the engine
	}

	RegisterEngineComponents();

	/* Initialize libraries */
	//NetworkManager::Init();

	Performance::Init();

	//------------------------------------------ Init renderer
	Engine::renderer = new RendererOpengl();
	int rendererInitResult = Engine::renderer->Init();
	if (rendererInitResult != 0)
	{
		Debug::PrintError("-------- Renderer init error code: " + std::to_string(rendererInitResult) + " --------");
		return -1;
	}

	//------------------------------------------ Init Window
#ifdef __PSP__
	Window::SetResolution(PSP_SCR_WIDTH, PSP_SCR_HEIGHT);
#elif __vita__
	Window::SetResolution(960, 544);
#else
	Window::SetResolution(1280, 720);
#endif
	int windowInitResult = Window::Init();
	if (windowInitResult != 0)
	{
		Debug::PrintError("-------- Window init error code: " + std::to_string(windowInitResult) + " --------");
		return -1;
	}
	Engine::renderer->Setup();

	// Not working
	/*#if defined(__PSP__)
		if (sceGeEdramSetSize(0x400000) == 0)
		{
			Debug::Print("-------- sceGeEdramSetSize ok --------");
		}
		else
		{
			Debug::PrintWarnign("-------- sceGeEdramSetSize ERROR --------");
		}
	#endif*/

	//------------------------------------------ Init other things
	Graphics::Init();
	InputSystem::Init();
	SpriteManager::Init();
	MeshManager::Init();
	TextManager::Init();
	AssetManager::Init();
	AudioManager::Init();
	Time::Init();

	//Init Editor
#if defined(EDITOR)
	Gizmo::Init();
	EditorUI::Init();
	Editor::Init();
#endif

	Debug::Print("-------- Engine fully initiated --------\n");

	CreateBenchmarks();

	UnitTestManager::StartAllTests();

	return 0;
}

void Engine::CheckEvents()
{
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
#if defined(EDITOR)
				if (event.window.windowID == SDL_GetWindowID(Window::window))
#endif
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
#endif
}

void Engine::Loop()
{
	Debug::Print("-------- Initiating game --------");

	// Load the game if the executable is not the Editor
#if !defined(EDITOR)
	int projectLoadResult = LoadGame();
	if (projectLoadResult != 0)
	{
		return;
	}
#endif

	canUpdateAudio = true;
	while (isRunning)
	{
		engineLoopBenchmark->Start();

#if defined(EDITOR)
		FinishThreadedFileLoading();
#endif

		// Update time, inputs and network
		Time::UpdateTime();
		InputSystem::ClearInputs();
		NetworkManager::Update();

#if defined(_WIN32) || defined(_WIN64)
		Engine::CheckEvents();
#else
		InputSystem::Read();
#endif

		canUpdateAudio = false;
#if defined(EDITOR)
		editorUpdateBenchmark->Start();
		Editor::Update();
		editorUpdateBenchmark->Stop();
#endif
		if (ProjectManager::GetIsProjectLoaded())
		{
			RemoveUnusedFiles();

			// Update all components
			componentsUpdateBenchmark->Start();
			UpdateComponents();

			// Remove all destroyed gameobjects and components
			RemoveDestroyedGameObjects();
			RemoveDestroyedComponents();

			componentsUpdateBenchmark->Stop();

			canUpdateAudio = true;

			drawIDrawablesBenchmark->Start();
			Graphics::DrawAllDrawable();
			drawIDrawablesBenchmark->Stop();

			ResetTransformsStates();
		}
		else
		{
#if defined(EDITOR)
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			Engine::renderer->Clear();
#endif
		}

#if defined(EDITOR)
		editorDrawBenchmark->Start();
		Editor::Draw();
		editorDrawBenchmark->Stop();
#endif
		Window::UpdateScreen();
		engineLoopBenchmark->Stop();

		Performance::Update();
		Performance::ResetCounters();
	}
#if defined(_WIN32) || defined(_WIN64)
	ImGui::SaveIniSettingsToDisk("imgui.ini");
#endif
	delete game;
}

void Engine::RegisterEngineComponents()
{
	// List all Engine components
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
	ClassRegistry::AddComponentClass("BillboardRenderer", [](std::shared_ptr<GameObject> go)
		{ return go->AddComponent<BillboardRenderer>(); });
	ClassRegistry::AddComponentClass("LineRenderer", [](std::shared_ptr<GameObject> go)
		{ return go->AddComponent<LineRenderer>(); });
	ClassRegistry::AddComponentClass("AudioSource", [](std::shared_ptr<GameObject> go)
		{ return go->AddComponent<AudioSource>(); });
	ClassRegistry::AddComponentClass("TestComponent", [](std::shared_ptr<GameObject> go)
		{ return go->AddComponent<TestComponent>(); });
}

void Engine::SetMaxCpuSpeed()
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
}

void Engine::SetGameState(GameState _gameState)
{
#if defined(EDITOR)
	if (_gameState == Playing)
	{
		gameState = Starting;
		SceneManager::SaveScene(SaveSceneForPlayState);
		SceneManager::RestoreScene();
		gameState = _gameState;
	}
	else if (_gameState == Stopped)
	{
		gameState = _gameState;
		SceneManager::RestoreScene();
	}
#else
	gameState = _gameState;
#endif
}

void Engine::SetCurrentProjectDirectory(ProjectDirectory* dir)
{
	if (currentProjectDirectory)
		currentProjectDirectory->files.clear();
	currentProjectDirectory = dir;
	if (currentProjectDirectory)
	{
		ProjectManager::FillProjectDirectory(currentProjectDirectory);
		int itemCount = currentProjectDirectory->files.size();
		for (int i = 0; i < itemCount; i++)
		{
			currentProjectDirectory->files[i]->LoadFileReference();
		}
	}
}

ProjectDirectory* Engine::GetCurrentProjectDirectory()
{
	return currentProjectDirectory;
}

void Engine::SetSelectedFileReference(std::shared_ptr<FileReference> fileReference)
{
	selectedFileReference = fileReference;
#if  defined(EDITOR)
	Editor::inspector->loadedPreview = nullptr;
#endif
}

std::shared_ptr<FileReference> Engine::GetSelectedFileReference()
{
	return selectedFileReference;
}

void Engine::Stop()
{
	renderer->Stop();
	AudioManager::Stop();
#ifdef __vita__
	sceKernelExitProcess(0);
#endif
}

void Engine::Quit()
{
	isRunning = false;
}

void Engine::SetSelectedGameObject(std::weak_ptr<GameObject> newSelected)
{
	selectedGameObject = newSelected;
}

int Engine::LoadGame()
{
	ProjectManager::LoadProject(".\\");
	return 0;
}

void Engine::CreateBenchmarks()
{
	engineLoopBenchmark = new ProfilerBenchmark("Engine loop", "Engine loop");
	//gameLoopBenchmark = new ProfilerBenchmark("Engine loop", "Game update");
	componentsUpdateBenchmark = new ProfilerBenchmark("Engine loop", "Components update");
	drawIDrawablesBenchmark = new ProfilerBenchmark("Engine loop", "Draw");
	editorUpdateBenchmark = new ProfilerBenchmark("Engine loop", "Editor update");
	editorDrawBenchmark = new ProfilerBenchmark("Engine loop", "Editor draw");
}

void Engine::UpdateComponents()
{
	// Order components and initialise new components
	if (componentsListDirty)
	{
		componentsListDirty = false;
		orderedComponents.clear();

		componentsCount = 0;
		OrderComponents();
		InitialiseComponents();
	}

	if (gameState == GameState::Playing)
	{
		// Update components
		for (int i = 0; i < componentsCount; i++)
		{
			if (auto component = orderedComponents[i].lock())
			{
				if (component->GetGameObject()->GetLocalActive() && component->GetIsEnabled())
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
	}
}

void Engine::OrderComponents()
{
	for (int gIndex = 0; gIndex < gameObjectCount; gIndex++)
	{
		std::shared_ptr<GameObject>& gameObjectToCheck = gameObjects[gIndex];
		if (gameObjectToCheck)
		{
			if (gameObjectToCheck->GetActive())
			{
				int goComponentCount = (int)gameObjectToCheck->GetComponentCount();
				bool placeFound = false;
				for (int cIndex = 0; cIndex < goComponentCount; cIndex++)
				{
					std::shared_ptr<Component>& componentToCheck = gameObjectToCheck->components[cIndex];
					if (componentToCheck)
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

void Engine::InitialiseComponents()
{
	int componentsToInitCount = 0;
	std::vector<std::shared_ptr<Component>> orderedComponentsToInit;
	// Find uninitiated components and order them
	for (int i = 0; i < componentsCount; i++)
	{
		if (auto componentToCheck = orderedComponents[i].lock())
		{
			if (!componentToCheck->initiated && componentToCheck->GetIsEnabled())
			{
				orderedComponentsToInit.push_back(componentToCheck);
				componentsToInitCount++;
			}
		}
	}

	// Init components
	for (int i = 0; i < componentsToInitCount; i++)
	{
		orderedComponentsToInit[i]->Start();
		orderedComponentsToInit[i]->initiated = true;
	}
}

void Engine::RemoveDestroyedGameObjects()
{
	// Remove destroyed GameObjects from the Engine's GameObjects list
	int gameObjectToDestroyCount = (int)gameObjectsToDestroy.size();
	for (int i = 0; i < gameObjectToDestroyCount; i++)
	{
		for (int gIndex = 0; gIndex < gameObjectCount; gIndex++)
		{
			std::shared_ptr<GameObject> gameObjectToCheck = gameObjects[gIndex];
			if (gameObjectToCheck == gameObjectsToDestroy[i].lock())
			{
				gameObjects.erase(gameObjects.begin() + gIndex);
				break;
			}
		}
		gameObjectCount--;
	}
	gameObjectsToDestroy.clear();
}

void Engine::RemoveDestroyedComponents()
{
	int componentToDestroyCount = (int)componentsToDestroy.size();
	for (int i = 0; i < componentToDestroyCount; i++)
	{
		std::shared_ptr<Component> component = componentsToDestroy[i];
		if (component)
		{
			Engine::RemoveComponentReferences(component);
		}
	}
	componentsToDestroy.clear();
}

void Engine::RemoveComponentReferences(std::weak_ptr <Component> weakComponent)
{
	// Check if the component is a special class and remove other references
	if (auto component = weakComponent.lock())
	{
		//------------------------------------------------------------------------ Include the component header to compile
		if (auto drawable = std::dynamic_pointer_cast<IDrawable>(component))
		{
			Graphics::RemoveDrawable(std::dynamic_pointer_cast<IDrawable>(component));
			AssetManager::RemoveDrawable(std::dynamic_pointer_cast<IDrawable>(component));
		}
		else if (auto light = std::dynamic_pointer_cast<Light>(component))
		{
			AssetManager::RemoveLight(light);
		}
		else if (auto audioSource = std::dynamic_pointer_cast<AudioSource>(component))
		{
			AudioManager::RemoveAudioSource(audioSource);
		}
		else if (auto camera = std::dynamic_pointer_cast<Camera>(component))
		{
			int cameraCount = Graphics::cameras.size();
			for (int cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++)
			{
				auto cam = Graphics::cameras[cameraIndex].lock();
				if (cam && cam == camera)
				{
					Graphics::cameras.erase(Graphics::cameras.begin() + cameraIndex);
					break;
				}
			}
		}
	}
}

void Engine::RemoveUnusedFiles()
{
	int fileRefCount = AssetManager::GetFileReferenceCount();
	for (int i = 0; i < fileRefCount; i++)
	{
		std::shared_ptr<FileReference> fileRef = AssetManager::GetFileReference(i);
		int refCount = fileRef.use_count();
		// If the reference count is 2 (fileRef variable and the reference in the asset manager)
		if (refCount == 2)
		{
			// Free the file 
			AssetManager::RemoveFileReference(fileRef);
			fileRef.reset();
			i--;
			fileRefCount--;
		}
	}
}

void Engine::FinishThreadedFileLoading()
{
	threadLoadingMutex.lock();
	int threadFileCount = threadLoadedFiles.size();
	for (int i = 0; i < threadFileCount; i++)
	{
		threadLoadedFiles[i]->OnLoadFileReferenceFinished();
	}
	threadLoadedFiles.clear();
	threadLoadingMutex.unlock();
}

void Engine::ResetTransformsStates()
{
	// Reset moved state of all transforms
	for (int i = 0; i < Engine::gameObjectCount; i++)
	{
		std::shared_ptr<GameObject> gameObject = gameObjects[i];
		if (gameObject)
		{
			gameObject->GetTransform()->movedLastFrame = false;
		}
	}
}

void Engine::AddGameObject(std::shared_ptr<GameObject> gameObject)
{
	gameObjects.push_back(gameObject);
	gameObjectCount++;
}

void Engine::AddGameObjectEditor(std::shared_ptr<GameObject> gameObject)
{
	gameObjectsEditor.push_back(gameObject);
	gameObjectEditorCount++;
}

std::vector<std::shared_ptr<GameObject>> Engine::GetGameObjects()
{
	return std::vector<std::shared_ptr<GameObject>>(Engine::gameObjects);
}

void DestroyGameObjectAndChild(std::weak_ptr<GameObject> gameObject)
{
	std::shared_ptr<GameObject> gameObjectLock = gameObject.lock();
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
	// Remove the component from the his parent's components list
	if (auto component = weakComponent.lock())
		component->GetGameObject()->RemoveComponent(weakComponent);
}
