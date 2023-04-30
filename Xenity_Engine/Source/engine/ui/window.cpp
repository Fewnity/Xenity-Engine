#include "window.h"
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <stb_image.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl2.h"

#include "../debug/debug.h"
#include "../graphics/graphics.h"
#include "../graphics/camera.h"

SDL_Window* Window::window = nullptr;

int Window::width = 1280;
int Window::height = 720;

float Window::aspect = 0;

const char * ENGINE_NAME = "Xenity Engine";

/// <summary>
/// Init window
/// </summary>
/// <returns></returns>
int Window::InitWindow()
{
	/* Initialize the library */
	if (!glfwInit() && SDL_Init(SDL_INIT_EVERYTHING) != 0)
		return -1;

	unsigned int center = SDL_WINDOWPOS_CENTERED;

	/*SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);*/

	//SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);

	window = SDL_CreateWindow(ENGINE_NAME, center, center, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	SDL_GLContext context = SDL_GL_CreateContext(window);


	gladLoadGLLoader(SDL_GL_GetProcAddress);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForOpenGL(Window::window, context);
	ImGui_ImplOpenGL3_Init("#version 460");
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	//Use depth management
	glEnable(GL_DEPTH_TEST);

	//0 is our origin, the higher the z, the farther the object
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND); //Enable blending.
	//glEnable(GL_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set blending function.

	glEnable(GL_CULL_FACE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	float borderColor[] = { 1.0f, 1.0f, 0.0f, 0.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); //TODO Move to texture init?

	SDL_GL_SetSwapInterval(1);

	OnResize(width, height);

	Debug::Print("---- Window initiated ----");

	return 0;
}

/// <summary>
/// On windows resized
/// </summary>
/// <param name="newWidth"></param>
/// <param name="newHeight"></param>
void Window::OnResize(const int newWidth, const int newHeight)
{
	glViewport(0, 0, newWidth, newHeight);
	width = newWidth;
	height = newHeight;
	aspect = static_cast<float>((width) / static_cast<float>(height));
	if(Graphics::usedCamera)
	Graphics::usedCamera->UpdateProjection();
}

/// <summary>
/// Update window screen
/// </summary>
void Window::UpdateScreen()
{
	SDL_GL_SwapWindow(window);
}

#pragma region Accessors

/// <summary>
/// Set window title
/// </summary>
/// <param name="title">String title</param>
void Window::SetTitle(const std::string title) 
{
	SDL_SetWindowTitle(window, title.c_str());
}

/// <summary>
/// Set window width value (Do not change UI size)
/// </summary>
/// <param name="width_"></param>
void Window::SetWidth(const int width_)
{
	width = width_;
}

/// <summary>
/// Set window height value (Do not change UI size)
/// </summary>
/// <param name="height_"></param>
void Window::SetHeight(const int height_)
{
	height = height_;
}

/// <summary>
/// Get window width in pixel
/// </summary>
/// <returns></returns>
int Window::GetWidth()
{
	return width;
}

/// <summary>
/// Get window height in pixel
/// </summary>
/// <returns></returns>
int Window::GetHeight()
{
	return height;
}

float Window::GetAspectRatio()
{
	return aspect;
}

#pragma endregion