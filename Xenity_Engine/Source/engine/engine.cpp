#include "engine.h"

// Settings
#include "engine_settings.h"

// Renderers
#include <engine/graphics/renderer/renderer_opengl.h>
#include <engine/graphics/renderer/renderer_gskit.h>
#include <engine/graphics/renderer/renderer_ge.h>

// Audio
#include <engine/audio/audio_manager.h>

// Network
#include <engine/network/network.h>

// Gameplay
#include <engine/game_elements/gameplay_manager.h>

// Game core
#include "game_interface.h"

// Class registry
#include <engine/class_registry/class_registry.h>

// Editor
#if defined(EDITOR)
#include <imgui/imgui_impl_sdl2.h>
#include <xenity_editor.h>
#include <glad/glad.h>
#include <editor/ui/menus/scene_menu.h>
#include <engine/scene_management/scene_manager.h>
#endif

// Other platforms
#include <engine/cpu.h>
#if defined(__PSP__)
#include <psp/gu2gl.h>
#include <psp/callbacks.h>
#elif defined(__vita__)
#include <psp2/kernel/processmgr.h>
#endif

// Files & Assets
#include <engine/file_system/file_system.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/asset_management/project_manager.h>

// Debug, Tests & Profiling
#include <engine/debug/debug.h>
#include <engine/debug/performance.h>
#include <unit_tests/unit_test_manager.h>
#include <engine/tools/profiler_benchmark.h>

// Window
#include <engine/ui/window.h>

// Inputs
#include <engine/inputs/input_system.h>

// Graphics
#include <engine/graphics/graphics.h>

// Time
#include <engine/time/time.h>

// Physics
#include <engine/physics/physics_manager.h>

#include <editor/plugin/plugin_manager.h>
#include <engine/file_system/async_file_loading.h>

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
		// Not a critical module, do not stop the engine
	}

	ClassRegistry::RegisterEngineComponents();

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
	InputSystem::Init();
	AssetManager::Init();
	AudioManager::Init();
	Time::Init();
	PhysicsManager::Init();

	// Init Editor
#if defined(EDITOR)
	PluginManager::Init();
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
#if defined(EDITOR)
			bool cancelQuit = SceneManager::OnQuit();
			if (!cancelQuit)
			{
				isRunning = false;
			}
			else
			{
				isRunning = true;
			}
#else
			isRunning = false;
#endif
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
	bool projectLoaded = ProjectManager::LoadProject(".\\");
	if (!projectLoaded)
	{
		return;
	}
#endif
	Time::UpdateTime();
	canUpdateAudio = true;
	while (isRunning)
	{
		engineLoopBenchmark->Start();

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
		AsyncFileLoading::FinishThreadedFileLoading();
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
			AssetManager::RemoveUnusedFiles();

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

			GameplayManager::ResetTransformStates();
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
	}
}

void Engine::Stop()
{
	isRunning = false;
#if defined(EDITOR)
	ImGui::SaveIniSettingsToDisk("imgui.ini");
#endif
	game.reset();

	renderer->Stop();
	renderer.reset();
	AudioManager::Stop();
#if defined(EDITOR)
	PluginManager::Stop();
#endif

#if defined(__vita__)
	sceKernelExitProcess(0);
#endif
}

void Engine::Quit()
{
	isRunning = false;
}

void Engine::CreateBenchmarks()
{
	engineLoopBenchmark = std::make_shared<ProfilerBenchmark>("Engine loop", "Engine loop");
	// gameLoopBenchmark = new ProfilerBenchmark("Engine loop", "Game update");
	componentsUpdateBenchmark = std::make_shared<ProfilerBenchmark>("Engine loop", "Components update");
	drawIDrawablesBenchmark = std::make_shared<ProfilerBenchmark>("Draw", "Draw");
	editorUpdateBenchmark = std::make_shared<ProfilerBenchmark>("Engine loop", "Editor update");
	editorDrawBenchmark = std::make_shared<ProfilerBenchmark>("Engine loop", "Editor draw");
}