#include "window.h"
#if defined(_WIN32) || defined(_WIN64)
#include <glad/glad.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl2.h"
#include <dwmapi.h>
#include <SDL2/SDL_syswm.h>
#endif

#include "../engine.h"
#include "../graphics/renderer/renderer.h"
#include "../debug/debug.h"
#include "../graphics/graphics.h"
#include "../graphics/camera.h"
#include "../scene_management/scene_manager.h"
#include "../scene_management/scene.h"
#include "../file_system/file_system.h"
#include "../asset_management/project_manager.h"

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
	int cameraCount = Graphics::cameras.size();
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

float Window::GetAspectRatio()
{
	return aspect;
}

int Window::Init()
{
#if defined(_WIN32) || defined(_WIN64)
	//  Init SDL
	int sdlInitResult = SDL_Init(SDL_INIT_EVERYTHING);
	if (sdlInitResult != 0)
	{
		return WND_ERROR_SDL_INIT;
	}

	// Create SDL Window
	unsigned int center = SDL_WINDOWPOS_CENTERED;
	window = SDL_CreateWindow(ENGINE_NAME, center, center, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
	if (window == nullptr)
	{
		return WND_ERROR_SDL_CREATE_WINDOW;
	}

	// Create OpenGL Context
	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (context == nullptr)
	{
		return WND_ERROR_SDL_GL_CONTEXT;
	}

	gladLoadGLLoader(SDL_GL_GetProcAddress);
	SDL_GL_SetSwapInterval(1);
	OnResize();

	//Set dark title bar
	try
	{
		SDL_SysWMinfo wmInfo;
		SDL_VERSION(&wmInfo.version);
		SDL_GetWindowWMInfo(window, &wmInfo);
		HWND hwnd = wmInfo.info.win.window;

		BOOL USE_DARK_MODE = true;
		if (SUCCEEDED(DwmSetWindowAttribute(
			hwnd, DWMWINDOWATTRIBUTE::DWMWA_USE_IMMERSIVE_DARK_MODE,
			&USE_DARK_MODE, sizeof(USE_DARK_MODE))))
		{

		}
	}
	catch (const std::exception&)
	{
		Debug::PrintWarning("Failed to set dark title bar");
	}


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform 
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForOpenGL(Window::window, context);
	ImGui_ImplOpenGL3_Init("#version 460");

	UpdateWindowTitle();
#endif
	Debug::Print("-------- Window initiated --------");
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
	std::string newTitle = "" + ProjectManager::GetProjectName() + " - ";
	if (SceneManager::GetOpenedScene())
	{
		newTitle += SceneManager::GetOpenedScene()->file->GetFileName();
	}
	newTitle += std::string(" - ") + ENGINE_NAME + " 0.1";
	SDL_SetWindowTitle(window, newTitle.c_str());
#endif
}

void Window::SetFullScreenMode(bool enable)
{
#if defined(_WIN32) || defined(_WIN64)
	if (enable)
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(window, 0);
#endif
}

void Window::UpdateAspectRatio()
{
	aspect = (float)width / (float)height;
}