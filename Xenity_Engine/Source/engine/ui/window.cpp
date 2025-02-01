// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "window.h"

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
#include <glad/gl.h>
#endif

#if defined(EDITOR)
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl3.h>
#include <implot/implot.h>
#endif

#include <engine/debug/debug.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>
#include <engine/scene_management/scene_manager.h>
#include <engine/scene_management/scene.h>
#include <engine/file_system/file.h>
#include <engine/asset_management/project_manager.h>
#include "screen.h"
#include <engine/debug/stack_debug_object.h>

int Window::s_width = 0;
int Window::s_height = 0;
float Window::s_aspect = 0;
const char* ENGINE_NAME = "Xenity Engine"; //TODO : To move
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
SDL_Window* Window::s_window = nullptr;
#endif

void Window::SetResolution(const int width_, const int height_)
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
	s_width = width_;
	s_height = height_;
	Screen::s_width = s_width;
	Screen::s_height = s_height;
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	if (s_window != nullptr)
#endif
		OnResize();
}

void Window::OnResize()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
	UpdateAspectRatio();

#if !defined(EDITOR)
	const int cameraCount = Graphics::cameras.size();
	for (int i = 0; i < cameraCount; i++)
	{
		Graphics::cameras[i].lock()->ChangeFrameBufferSize(Vector2Int(s_width, s_height));
	}
#endif
}

int Window::GetWidth()
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);
	return s_width;
}

int Window::GetHeight()
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);
	return s_height;
}

int Window::GetTitleBarHeight()
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);
	int size = 0;
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	SDL_GetWindowBordersSize(s_window, &size, nullptr, nullptr, nullptr);
#endif
	return size;
}

float Window::GetAspectRatio()
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);
	return s_aspect;
}

int Window::Init()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	//  Init SDL
	const bool sdlInitResult = SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS);
	if (!sdlInitResult)
	{
		std::string sdlError = SDL_GetError();
		Debug::PrintError("[Window::Init] SDL_Init Error: " + sdlError);
		return static_cast<int>(WindowError::WND_ERROR_SDL_INIT);
	}

	// Create SDL Window
	s_window = SDL_CreateWindow(ENGINE_NAME, s_width, s_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
	if (s_window == nullptr)
	{
		std::string sdlError = SDL_GetError();
		Debug::PrintError("[Window::Init] SDL_Init Error: " + sdlError);
		return static_cast<int>(WindowError::WND_ERROR_SDL_CREATE_WINDOW);
	}

	// Create OpenGL Context
	SDL_GLContext context = SDL_GL_CreateContext(s_window);
	if (context == nullptr)
	{
		std::string sdlError = SDL_GetError();
		Debug::PrintError("[Window::Init] SDL_Init Error: " + sdlError);
		return static_cast<int>(WindowError::WND_ERROR_SDL_GL_CONTEXT);
	}

	gladLoadGL(reinterpret_cast<GLADloadfunc>(SDL_GL_GetProcAddress));
	SDL_GL_SetSwapInterval(1);
	OnResize();

#if defined(EDITOR)
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform 
	ImGui::StyleColorsDark();
	ImGui_ImplSDL3_InitForOpenGL(Window::s_window, context);
	ImGui_ImplOpenGL3_Init();
#endif

	UpdateWindowTitle();
#endif
	Debug::Print("-------- Window initiated --------", true);
	return 0;
}

void Window::UpdateScreen()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	SDL_GL_SwapWindow(s_window);
#endif
}

void Window::UpdateWindowTitle()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	std::string newTitle = "";
	if (ProjectManager::IsProjectLoaded())
	{
#if defined(EDITOR)
		newTitle += ProjectManager::GetProjectName() + " - ";
		if (SceneManager::GetOpenedScene())
		{
			newTitle += SceneManager::GetOpenedScene()->m_file->GetFileName();
			if (SceneManager::GetSceneModified())
			{
				newTitle += "*";
			}
		}
		else
		{
			newTitle += "Empty Scene *";
		}
		newTitle += std::string(" - ");
#else
		newTitle += ProjectManager::GetProjectName();
#endif
	}
	else 
	{

	}
	newTitle += std::string(ENGINE_NAME) + " " + ENGINE_VERSION;
	SDL_SetWindowTitle(s_window, newTitle.c_str());
#endif
}

void Window::SetFullScreenMode(bool enable)
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	SDL_SetWindowFullscreen(s_window, enable);
	//if (enable)
	//	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	//else
	//	SDL_SetWindowFullscreen(window, 0);
#endif
}

void Window::UpdateAspectRatio()
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);
	s_aspect = static_cast<float>(s_width) / static_cast<float>(s_height);
}