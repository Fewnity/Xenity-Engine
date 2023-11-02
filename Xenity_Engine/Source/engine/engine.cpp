// #define EDITOR
#include "engine.h"
#include "engine_settings.h"
#include "../xenity.h"
#include "graphics/renderer/renderer.h"
#include "graphics/renderer/renderer_opengl.h"
#include "audio/audio_manager.h"
#include "network/network.h"
#include "dynamic_lib/dynamic_lib.h"
#include "game_elements/gameplay_manager.h"
#include "cpu.h"

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

#if defined(__PSP__)
#include "../psp/gu2gl.h"
#include "../psp/callbacks.h"
#include <psppower.h>
#include <pspge.h>
#elif defined(__vita__)
#include <psp2/kernel/processmgr.h>
#include <psp2/power.h>
#endif

#include "asset_management/project_manager.h"
#include "scene_management/scene_manager.h"
#include "../editor/compiler.h"
#include "../unit_tests/unit_test_manager.h"
#include "graphics/2d_graphics/billboard_renderer.h"
#include "physics/physics_manager.h"

std::vector<std::shared_ptr<FileReference>> Engine::threadLoadedFiles;
std::mutex Engine::threadLoadingMutex;

ProjectDirectory* Engine::currentProjectDirectory = nullptr;

ProfilerBenchmark* engineLoopBenchmark = nullptr;
ProfilerBenchmark* componentsUpdateBenchmark = nullptr;
ProfilerBenchmark* drawIDrawablesBenchmark = nullptr;
ProfilerBenchmark* editorUpdateBenchmark = nullptr;
ProfilerBenchmark* editorDrawBenchmark = nullptr;

std::unique_ptr<Renderer> Engine::renderer = nullptr;
bool Engine::canUpdateAudio = false;
bool Engine::isRunning = true;
std::unique_ptr<GameInterface> Engine::game = nullptr;

int Engine::Init()
{
	// Init random
	srand((unsigned int)time(NULL));

#if defined(__PSP__)
	SetupCallbacks();
#endif

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
	NetworkManager::Init();
	NetworkManager::needDrawMenu = false;

	Performance::Init();

	//------------------------------------------ Init renderer
	renderer = std::make_unique<RendererOpengl>();
	int rendererInitResult = renderer->Init();
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
	renderer->Setup();

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
	Shader::Init();
	InputSystem::Init();
	SpriteManager::Init();
	MeshManager::Init();
	TextManager::Init();
	AssetManager::Init();
	AudioManager::Init();
	Time::Init();
	PhysicsManager::Init();

	//Init Editor
#if defined(EDITOR)
	Gizmo::Init();
	int editorUiInitResult = EditorUI::Init();
	if (editorUiInitResult != 0) 
	{
		Debug::PrintError("-------- Editor UI init error code: " + std::to_string(editorUiInitResult) + " --------");
		return -1;
	}
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
	// Check SDL event
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
			if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) // Update viewport resolution on resize
			{
#if defined(EDITOR)
				if (event.window.windowID == SDL_GetWindowID(Window::window))
#endif
					Window::SetResolution(event.window.data1, event.window.data2);
			}
			else if (event.window.event == SDL_WINDOWEVENT_CLOSE) // Stop the engine on window close
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
			GameplayManager::UpdateComponents();

			if (GameplayManager::GetGameState() == GameState::Playing)
				PhysicsManager::Update();

			// Remove all destroyed gameobjects and components
			GameplayManager::RemoveDestroyedGameObjects();
			GameplayManager::RemoveDestroyedComponents();

			componentsUpdateBenchmark->Stop();

			canUpdateAudio = true;
			
			// Draw
			drawIDrawablesBenchmark->Start();
			Graphics::Draw();
			drawIDrawablesBenchmark->Stop();

			ResetTransformsStates();
		}
		else
		{
#if defined(EDITOR)
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			renderer->Clear();
#endif
		}
#if defined(EDITOR)
		editorDrawBenchmark->Start();
		Editor::Draw();
		editorDrawBenchmark->Stop();
#endif
		Debug::SendProfilerDataToServer();
		Window::UpdateScreen();
		engineLoopBenchmark->Stop();
		Performance::Update();
		Performance::ResetCounters();
	}
#if defined(EDITOR)
	ImGui::SaveIniSettingsToDisk("imgui.ini");
#endif
	game.reset();
	//delete game;
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
	ClassRegistry::AddComponentClass("RigidBody", [](std::shared_ptr<GameObject> go)
		{ return go->AddComponent<RigidBody>(); });
	ClassRegistry::AddComponentClass("BoxCollider", [](std::shared_ptr<GameObject> go)
		{ return go->AddComponent<BoxCollider>(); });
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


void Engine::Stop()
{
	renderer->Stop();
	AudioManager::Stop();
#if defined(__vita__)
	sceKernelExitProcess(0);
#endif
}

void Engine::Quit()
{
	isRunning = false;
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
	drawIDrawablesBenchmark = new ProfilerBenchmark("Draw", "Draw");
	editorUpdateBenchmark = new ProfilerBenchmark("Engine loop", "Editor update");
	editorDrawBenchmark = new ProfilerBenchmark("Engine loop", "Editor draw");
}


void Engine::RemoveComponentReferences(const std::shared_ptr <Component>& component)
{
	// Check if the component is a special class and remove other references
	if (component)
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
		if (!threadLoadedFiles[i]->isLoading)
		{
			threadLoadedFiles[i]->OnLoadFileReferenceFinished();
			threadLoadedFiles.erase(threadLoadedFiles.begin() + i);
			threadFileCount--;
			i--;
		}
	}

	threadLoadingMutex.unlock();
}

void Engine::ResetTransformsStates()
{
	// Reset moved state of all transforms
	for (int i = 0; i < GameplayManager::gameObjectCount; i++)
	{
		std::shared_ptr<GameObject> gameObject = GameplayManager::gameObjects[i];
		if (gameObject)
		{
			gameObject->GetTransform()->movedLastFrame = false;
		}
	}
}


void DestroyGameObjectAndChild(const std::shared_ptr<GameObject>& gameObject)
{
	GameplayManager::gameObjectsToDestroy.push_back(gameObject);
	gameObject->waitingForDestroy = true;

	// Remove the destroyed gameobject from his parent's children list
	if (auto parent = gameObject->parent.lock())
	{
		int parentChildCount = parent->GetChildrenCount();
		for (int i = 0; i < parentChildCount; i++)
		{
			if (parent->children[i].lock() == gameObject)
			{
				parent->children.erase(parent->children.begin() + i);
				parent->childCount--;
				break;
			}
		}
	}

	int childCount = gameObject->GetChildrenCount();
	for (int i = 0; i < childCount; i++)
	{
		DestroyGameObjectAndChild(gameObject->children[i].lock());
	}
}

void Destroy(const std::weak_ptr<GameObject>& gameObject)
{
	Destroy(gameObject.lock());
}

void Destroy(const std::weak_ptr<Component>& weakComponent)
{
	Destroy(weakComponent.lock());
}

void Destroy(const std::shared_ptr<GameObject>& gameObject)
{
	if (!gameObject->waitingForDestroy)
	{
		DestroyGameObjectAndChild(gameObject);
	}
}

void Destroy(const std::shared_ptr<Component>& component)
{
	// Remove the component from the his parent's components list
	if (component)
		component->GetGameObject()->RemoveComponent(component);
}
