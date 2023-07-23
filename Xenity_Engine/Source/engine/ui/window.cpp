#include "window.h"
#if defined(_WIN32) || defined(_WIN64)
#include <glad/glad.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl2.h"
#endif

#include "../engine.h"
#include "../graphics/renderer/renderer.h"
#include "../debug/debug.h"

int Window::width = 0;
int Window::height = 0;
float Window::aspect = 0;
const char *ENGINE_NAME = "Xenity Engine";
#if defined(_WIN32) || defined(_WIN64)
SDL_Window *Window::window = nullptr;
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
    Engine::renderer->SetViewport(0, 0, width, height);
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

void Window::Init()
{
#if defined(_WIN32) || defined(_WIN64)
    SDL_Init(SDL_INIT_EVERYTHING);
    unsigned int center = SDL_WINDOWPOS_CENTERED;
    window = SDL_CreateWindow(ENGINE_NAME, center, center, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    SDL_GL_SetSwapInterval(1);
    OnResize();
    //SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(Window::window, context);
    ImGui_ImplOpenGL3_Init("#version 460");
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

#endif
    Debug::Print("-------- Window initiated --------");
}

void Window::UpdateScreen()
{
#if defined(_WIN32) || defined(_WIN64)
    SDL_GL_SwapWindow(window);
#endif
}

void Window::UpdateAspectRatio()
{
    aspect = (float)width / (float)height;
}