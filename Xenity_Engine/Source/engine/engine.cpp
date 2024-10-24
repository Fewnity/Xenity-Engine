// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "engine.h"

// Other platforms
#if defined(__PSP__)
#include <psp/callbacks.h>
#elif defined(__vita__)
#include <psp2/kernel/processmgr.h>
#elif defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
#include <csignal>
#endif

// Editor
#if defined(EDITOR)
#include <imgui/imgui_impl_sdl3.h>
#include <glad/glad.h>

#include <editor/ui/menus/game_menu.h>
#include <editor/plugin/plugin_manager.h>
#include <editor/file_handler.h>
#include <editor/compiler.h>
#include <editor/gizmo.h>
#include <editor/editor.h>
#include <editor/ui/editor_ui.h>

#include <engine/scene_management/scene_manager.h>
#endif

#include <engine/cpu.h>

// Settings
#include "engine_settings.h"

// Renderers
#include <engine/graphics/renderer/renderer_opengl.h>
#include <engine/graphics/renderer/renderer_gskit.h>
#include <engine/graphics/renderer/renderer_vu1.h>
#include <engine/graphics/renderer/renderer_gu.h>
#include <engine/graphics/renderer/renderer_rsx.h>

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

#include <engine/file_system/async_file_loading.h>
#include <engine/debug/crash_handler.h>
#include <engine/tools/scope_benchmark.h>
#include <engine/tools/math.h>
#include <engine/vectors/quaternion.h>
#include <engine/vectors/vector3.h>
#include "debug/stack_debug_object.h"

std::shared_ptr<ProfilerBenchmark> engineLoopBenchmark = nullptr;
std::shared_ptr<ProfilerBenchmark> componentsUpdateBenchmark = nullptr;
std::shared_ptr<ProfilerBenchmark> drawIDrawablesBenchmark = nullptr;
std::shared_ptr<ProfilerBenchmark> editorUpdateBenchmark = nullptr;
std::shared_ptr<ProfilerBenchmark> editorDrawBenchmark = nullptr;

std::unique_ptr<Renderer> Engine::s_renderer = nullptr;
bool Engine::s_canUpdateAudio = false;
bool Engine::s_isRunning = true;
bool Engine::s_isInitialized = false;

std::unique_ptr<GameInterface> Engine::s_game = nullptr;
Event<>* Engine::s_onWindowFocusEvent = new Event<>();

void Engine::OnCloseSignal([[maybe_unused]] int s)
{
	s_isRunning = false;
}

int Engine::Init()
{
#if defined(_WIN32) || defined(_WIN64)
	signal(SIGBREAK, Engine::OnCloseSignal);
#endif

	//  Init random
	srand(static_cast<unsigned int>(time(nullptr)));

#if defined(__PSP__)
	SetupCallbacks();
#endif

	// Setup game console CPU speed
	SetMaxCpuSpeed();

	//------------------------------------------ Init File System
	FileSystem::s_fileSystem = new FileSystem();
	const int fileSystemInitResult = FileSystem::s_fileSystem->InitFileSystem();
	if (fileSystemInitResult != 0)
	{
		return -1;
	}

	EngineSettings::LoadEngineSettings();
#if defined(EDITOR)
	EngineSettings::SaveEngineSettings();
#endif

	//------------------------------------------ Init Debug
	const int debugInitResult = Debug::Init();
	if (debugInitResult != 0)
	{
		Debug::PrintWarning("-------- Debug init error code: " + std::to_string(debugInitResult) + " --------", true);
		// Not a critical module, do not stop the engine
	}

	CrashHandler::Init();

#if defined(DEBUG)
#if defined(EDITOR)
	Debug::PrintWarning("-------- The editor is running in debug mode --------", true);
#else
	Debug::PrintWarning("-------- The game is running in debug mode --------", true);
#endif
#endif

	ClassRegistry::RegisterEngineComponents();
	ClassRegistry::RegisterEngineFileClasses();

	/* Initialize libraries */
	NetworkManager::Init();
	NetworkManager::s_needDrawMenu = false;

	Performance::Init();

	//------------------------------------------ Init renderer
#if defined(_EE)
	// renderer = std::make_unique<RendererGsKit>();
	s_renderer = std::make_unique<RendererVU1>();
#elif defined(__PSP__)
	s_renderer = std::make_unique<RendererGU>();
#elif defined(_WIN32) | defined(_WIN64) || defined(__vita__) || defined(__LINUX__)
	s_renderer = std::make_unique<RendererOpengl>();
#elif defined(__PS3__)
	s_renderer = std::make_unique<RendererRSX>();
#else
#error "No renderer defined for this platform" 
#endif

	if(s_renderer)
	{
		const int rendererInitResult = s_renderer->Init();
		if (rendererInitResult != 0)
		{
			Debug::PrintError("-------- Renderer init error code: " + std::to_string(rendererInitResult) + " --------", true);
			return -1;
		}
	}
	else
	{
		Debug::PrintError("-------- No Renderer created --------", true);
	}

	//------------------------------------------ Init Window
	const int windowInitResult = Window::Init();
	if (windowInitResult != 0)
	{
		Debug::PrintError("-------- Window init error code: " + std::to_string(windowInitResult) + " --------", true);
		return -1;
	}
	if(s_renderer)
	{
		s_renderer->Setup();
	}

	//------------------------------------------ Init other things
	InputSystem::Init();
	ProjectManager::Init();
	Graphics::Init();
	AssetManager::Init();
	AudioManager::Init();
	Time::Init();
	PhysicsManager::Init();

	//  Init Editor
#if defined(EDITOR)
#if defined(_WIN32) || defined(_WIN64)
	PluginManager::Init();
#endif // #if defined(_WIN32) || defined(_WIN64)
	Gizmo::Init();
	const int editorUiInitResult = EditorUI::Init();
	if (editorUiInitResult != 0)
	{
		Debug::PrintError("-------- Editor UI init error code: " + std::to_string(editorUiInitResult) + " --------", true);
		return -1;
	}
	Editor::Init();
	Compiler::Init();
#endif // #if defined(EDITOR)

	s_isInitialized = true;
	Debug::Print("-------- Engine fully initiated --------\n", true);

	CreateBenchmarks();

#if defined(DEBUG)
	UnitTestManager::StartAllTests();
#endif

	return 0;
}

void Engine::CheckEvents()
{
	SCOPED_PROFILER("Engine::CheckEvents", scopeBenchmark);
	int focusCount = 0;
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	// Check SDL event
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
#if defined(EDITOR)
		ImGui_ImplSDL3_ProcessEvent(&event);
#endif
		InputSystem::Read(event);

		switch (event.type)
		{
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			if (event.window.windowID == SDL_GetWindowID(Window::s_window))
			{
				Quit();
			}
			break;

#if defined(EDITOR)
		case (SDL_EVENT_DROP_COMPLETE):
		{
			Editor::OnDragAndDropFileFinished();
			break;
		}

		case (SDL_EVENT_DROP_FILE):
		{
			const char* dropped_filedir = event.drop.data;
			Editor::AddDragAndDrop(dropped_filedir);
			//SDL_free(dropped_filedir); // Free dropped_filedir memory // FIXME TODO memory leak here! Crash if used since updated to SDL3
			break;
		}
#endif

		case SDL_EVENT_WINDOW_FOCUS_GAINED:
			focusCount++;
			break;
		case SDL_EVENT_WINDOW_FOCUS_LOST:
			focusCount--;
			break;

		case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
			if (event.window.windowID == SDL_GetWindowID(Window::s_window))
			{
				Window::SetResolution(event.window.data1, event.window.data2);
			}
			break;

		default:
			break;
		}
	}

	if (focusCount == 1)
	{
#if defined(EDITOR)
		if (!EditorUI::IsEditingElement())
#endif
			s_onWindowFocusEvent->Trigger();
	}
#endif
}

void Engine::Loop()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	Debug::Print("-------- Initiating game --------", true);

	// Load the game if the executable is not the Editor
#if !defined(EDITOR)
#if defined(_EE) || defined(__PS3__)
	const ProjectLoadingErrors projectLoaded = ProjectManager::LoadProject("");
#else
	const ProjectLoadingErrors projectLoaded = ProjectManager::LoadProject("./");
#endif
	if (projectLoaded != ProjectLoadingErrors::Success)
	{
		Debug::Print("-------- Failed to load the game -------- Error code: " + std::to_string(static_cast<int>(projectLoaded)), true);
		return;
	}
#endif

	Time::UpdateTime();
	s_canUpdateAudio = true;
	while (s_isRunning)
	{
		{
			SCOPED_PROFILER("Engine::Loop", scopeBenchmark);
			engineLoopBenchmark->Start();

			// Update time, inputs and network
			Time::UpdateTime();
			InputSystem::ClearInputs();
			NetworkManager::Update();
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
			Engine::CheckEvents();
#else
			InputSystem::Read();
#endif

			s_canUpdateAudio = false;
#if defined(EDITOR)
			AsyncFileLoading::FinishThreadedFileLoading();

			editorUpdateBenchmark->Start();
			Editor::Update();

			const std::shared_ptr<Menu> gameMenu = Editor::GetMenu<GameMenu>();
			if (gameMenu)
				InputSystem::s_blockGameInput = !gameMenu->IsFocused();
			else
				InputSystem::s_blockGameInput = true;

			editorUpdateBenchmark->Stop();
#endif

			if (ProjectManager::IsProjectLoaded())
			{
				AssetManager::RemoveUnusedFiles();

				// Update all components
				componentsUpdateBenchmark->Start();
#if defined(EDITOR)
				// Catch game's code error to prevent the editor to crash
				const bool tryResult = CrashHandler::CallInTry(GameplayManager::UpdateComponents);
				if (tryResult)
				{
					std::string lastComponentMessage = "Error in game's code! Stopping the game...\n";
					const std::shared_ptr<Component> lastComponent = GameplayManager::GetLastUpdatedComponent().lock();
					if (lastComponent)
					{
						lastComponentMessage += "Component name: " + lastComponent->GetComponentName();
						if (lastComponent->GetGameObjectRaw())
						{
							lastComponentMessage += "\nThis component was on the gameobject: " + lastComponent->GetGameObjectRaw()->GetName();
						}
					}
					Debug::PrintError(lastComponentMessage);

					GameplayManager::SetGameState(GameState::Stopped, true);
				}
#else
				GameplayManager::UpdateComponents();
#endif

				if (GameplayManager::GetGameState() == GameState::Playing)
				{
					PhysicsManager::Update();
				}

				// Remove all destroyed gameobjects and components
				GameplayManager::RemoveDestroyedGameObjects();
				GameplayManager::RemoveDestroyedComponents();

				componentsUpdateBenchmark->Stop();

				s_canUpdateAudio = true;

				// Draw
				drawIDrawablesBenchmark->Start();
				Graphics::Draw();
				drawIDrawablesBenchmark->Stop();
			}
			else
			{
#if defined(EDITOR)
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				s_renderer->Clear();
#endif
			}
			InputSystem::s_blockGameInput = false;
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
	}
}

void Engine::Stop()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	if (!s_isInitialized)
		return;

	s_isInitialized = false;
	s_isRunning = false;

#if defined(EDITOR)
	SceneManager::ClearScene();
	ImGui::SaveIniSettingsToDisk("imgui.ini");
#endif
	s_game.reset();
	if(s_renderer)
	{
		s_renderer->Stop();
		s_renderer.reset();
	}
	AudioManager::Stop();
	PhysicsManager::Stop();
	Graphics::Stop();
#if defined(EDITOR) && (defined(_WIN32) || defined(_WIN64))
	PluginManager::Stop();
#endif

#if defined(__vita__)
	sceKernelExitProcess(0);
#endif
}

void Engine::Quit()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

#if defined(EDITOR)
	if (s_isRunning)
	{
		const bool cancelQuit = SceneManager::OnQuit();
		s_isRunning = cancelQuit;
	}
#else
	s_isRunning = false;
#endif
}

void Engine::CreateBenchmarks()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	engineLoopBenchmark = std::make_shared<ProfilerBenchmark>("Engine loop", "Engine loop");
	// gameLoopBenchmark = new ProfilerBenchmark("Engine loop", "Game update");
	componentsUpdateBenchmark = std::make_shared<ProfilerBenchmark>("Engine loop", "Components update");
	drawIDrawablesBenchmark = std::make_shared<ProfilerBenchmark>("Draw", "Draw");
	editorUpdateBenchmark = std::make_shared<ProfilerBenchmark>("Engine loop", "Editor update");
	editorDrawBenchmark = std::make_shared<ProfilerBenchmark>("Engine loop", "Editor draw");
}