#include "window.h"
#if defined(_WIN32) || defined(_WIN64)
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl3.h>
#include <dwmapi.h>
#include <windows.h>
//#include <SDL3/SDL_syswm.h>
#endif

#include <engine/engine.h>
#include <engine/debug/debug.h>

#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>

#include <engine/scene_management/scene_manager.h>
#include <engine/scene_management/scene.h>

#include <engine/file_system/file_system.h>
#include <engine/file_system/file.h>

#include <engine/asset_management/project_manager.h>


int Window::width = 0;
int Window::height = 0;
float Window::aspect = 0;
const char* ENGINE_NAME = "Xenity Engine"; //TODO : To move
#if defined(_WIN32) || defined(_WIN64)
SDL_Window* Window::window = nullptr;
#endif

void Window::SetResolution(const int width_, const int height_)
{
	width = width_;
	height = height_;
#if defined(_WIN32) || defined(_WIN64)
	if (window != nullptr)
#endif
		OnResize();
}

void Window::OnResize()
{
	UpdateAspectRatio();

#if !defined(EDITOR)
	const int cameraCount = Graphics::cameras.size();
	for (int i = 0; i < cameraCount; i++)
	{
		Graphics::cameras[i].lock()->ChangeFrameBufferSize(Vector2Int(width, height));
	}
#endif
}

int Window::GetWidth()
{
	return width;
}

int Window::GetHeight()
{
	return height;
}

int Window::GetTitleBarHeight()
{
	int size = 0;
#if defined(_WIN32) || defined(_WIN64)
	SDL_GetWindowBordersSize(window, &size, 0, 0, 0);
#endif
	return size;
}

float Window::GetAspectRatio()
{
	return aspect;
}

int Window::Init()
{
#if defined(_WIN32) || defined(_WIN64)
	//  Init SDL
	const int sdlInitResult = SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS);
	if (sdlInitResult != 0)
	{
		return (int)WindowError::WND_ERROR_SDL_INIT;
	}

	// Create SDL Window
	window = SDL_CreateWindow(ENGINE_NAME, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
	if (window == nullptr)
	{
		return (int)WindowError::WND_ERROR_SDL_CREATE_WINDOW;
	}

	// Create OpenGL Context
	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (context == nullptr)
	{
		return (int)WindowError::WND_ERROR_SDL_GL_CONTEXT;
	}

	gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
	SDL_GL_SetSwapInterval(1);
	OnResize();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform 
	ImGui::StyleColorsDark();
	ImGui_ImplSDL3_InitForOpenGL(Window::window, context);
	ImGui_ImplOpenGL3_Init();

	UpdateWindowTitle();
#endif
	Debug::Print("-------- Window initiated --------", true);
	return 0;
}

void Window::UpdateScreen()
{
#if defined(_WIN32) || defined(_WIN64)
	SDL_GL_SwapWindow(window);
#endif
}

void Window::UpdateWindowTitle()
{
#if defined(_WIN32) || defined(_WIN64)
	std::string newTitle = "";
	if (ProjectManager::GetIsProjectLoaded())
	{
		newTitle += ProjectManager::GetProjectName() + " - ";
		if (SceneManager::GetOpenedScene())
		{
			newTitle += SceneManager::GetOpenedScene()->file->GetFileName();
			if (SceneManager::GetSceneModified())
				newTitle += "*";
		}
		else
		{
			newTitle += "Empty Scene *";
		}
		newTitle += std::string(" - ");
	}
	else 
	{

	}
	newTitle += std::string(ENGINE_NAME) + " 0.1";
	SDL_SetWindowTitle(window, newTitle.c_str());
#endif
}

void Window::SetFullScreenMode(bool enable)
{
#if defined(_WIN32) || defined(_WIN64)
	SDL_SetWindowFullscreen(window, enable);
	//if (enable)
	//	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	//else
	//	SDL_SetWindowFullscreen(window, 0);
#endif
}

void Window::UpdateAspectRatio()
{
	aspect = (float)width / (float)height;
}