#include "engine.h"
#include "engine_settings.h"
#include "../xenity.h"
#include "graphics/renderer/renderer_opengl.h"
#include "graphics/renderer/renderer_gskit.h"
#include "graphics/renderer/renderer_ge.h"

#include "audio/audio_manager.h"
#include "network/network.h"
#include "game_elements/gameplay_manager.h"
#include "cpu.h"

#if defined(EDITOR)
#include <imgui/imgui_impl_sdl2.h>
#include "../xenity_editor.h"
#include <glad/glad.h>
#include "../editor/ui/menus/scene_menu.h"
#endif

#include "game_interface.h"
#include "class_registry/class_registry.h"

#if defined(__PSP__)
#include "../psp/gu2gl.h"
#include "../psp/callbacks.h"
#elif defined(__vita__)
#include <psp2/kernel/processmgr.h>
#endif

#include "asset_management/project_manager.h"
#include "scene_management/scene_manager.h"
#include "../unit_tests/unit_test_manager.h"
#include "physics/physics_manager.h"

std::vector<std::shared_ptr<FileReference>> Engine::threadLoadedFiles;
std::mutex Engine::threadLoadingMutex;

std::shared_ptr<ProfilerBenchmark> engineLoopBenchmark = nullptr;
std::shared_ptr<ProfilerBenchmark> componentsUpdateBenchmark = nullptr;
std::shared_ptr<ProfilerBenchmark> drawIDrawablesBenchmark = nullptr;
std::shared_ptr<ProfilerBenchmark> editorUpdateBenchmark = nullptr;
std::shared_ptr<ProfilerBenchmark> editorDrawBenchmark = nullptr;

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
	FileSystem::fileSystem = new FileSystem();
	int fileSystemInitResult = FileSystem::fileSystem->InitFileSystem();
	if (fileSystemInitResult != 0)
	{
		return -1;
	}

	EngineSettings::LoadEngineSettings();
	EngineSettings::SaveEngineSettings();

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
#if defined(_EE)
	renderer = std::make_unique<RendererGsKit>();
#elif defined(__PSP__)
	renderer = std::make_unique<RendererGE>();
#else
	renderer = std::make_unique<RendererOpengl>();
#endif
	int rendererInitResult = renderer->Init();
	if (rendererInitResult != 0)
	{
		Debug::PrintError("-------- Renderer init error code: " + std::to_string(rendererInitResult) + " --------");
		return -1;
	}

	//------------------------------------------ Init Window
#if defined(__PSP__)
	Window::SetResolution(PSP_SCR_WIDTH, PSP_SCR_HEIGHT);
#elif defined(__vita__)
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
		{
			bool cancelQuit = SceneManager::OnQuit();
			if (!cancelQuit) 
			{
				isRunning = false;
			}
			else 
			{
				isRunning = true;
			}
			break;
		}
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
	Time::UpdateTime();
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
		if (Editor::sceneMenu->isFocused) 
		{
			InputSystem::blockGameInput = true;
		}
		else 
		{
			InputSystem::blockGameInput = false;
		}
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
		InputSystem::blockGameInput = false;
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
}

void Engine::RegisterEngineComponents()
{
	// List all Engine components
	ClassRegistry::AddComponentClass<Light>("Light");
	ClassRegistry::AddComponentClass<Camera>("Camera");
	ClassRegistry::AddComponentClass<TextRendererCanvas>("TextRendererCanvas");
	ClassRegistry::AddComponentClass<TextRenderer>("TextRenderer");
	ClassRegistry::AddComponentClass<MeshRenderer>("MeshRenderer");
	ClassRegistry::AddComponentClass<Tilemap>("Tilemap");
	ClassRegistry::AddComponentClass<SpriteRenderer>("SpriteRenderer");
	ClassRegistry::AddComponentClass<BillboardRenderer>("BillboardRenderer");
	ClassRegistry::AddComponentClass<LineRenderer>("LineRenderer");
	ClassRegistry::AddComponentClass<AudioSource>("AudioSource");
	ClassRegistry::AddComponentClass<TestComponent>("TestComponent");
	ClassRegistry::AddComponentClass<RigidBody>("RigidBody");
	ClassRegistry::AddComponentClass<BoxCollider>("BoxCollider");
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
	engineLoopBenchmark = std::make_shared<ProfilerBenchmark>("Engine loop", "Engine loop");
	//gameLoopBenchmark = new ProfilerBenchmark("Engine loop", "Game update");
	componentsUpdateBenchmark = std::make_shared <ProfilerBenchmark>("Engine loop", "Components update");
	drawIDrawablesBenchmark = std::make_shared <ProfilerBenchmark>("Draw", "Draw");
	editorUpdateBenchmark = std::make_shared <ProfilerBenchmark>("Engine loop", "Editor update");
	editorDrawBenchmark = std::make_shared <ProfilerBenchmark>("Engine loop", "Editor draw");
}


void Engine::RemoveComponentReferences(const std::shared_ptr <Component>& component)
{
	// Check if the component is a special class and remove other references
	if (component)
	{
		//------------------------------------------------------------------------ Include the component header to compile
		if (auto drawable = std::dynamic_pointer_cast<IDrawable>(component))
		{
			Graphics::RemoveDrawable(drawable);
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
			size_t cameraCount = Graphics::cameras.size();
			for (size_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++)
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
	size_t threadFileCount = threadLoadedFiles.size();
	for (size_t i = 0; i < threadFileCount; i++)
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
		DestroyGameObjectAndChild(gameObject->children[0].lock());
		i--;
		childCount--;
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
