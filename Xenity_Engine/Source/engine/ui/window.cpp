#include "window.h"
#include <glad/glad.h>
#include "../engine.h"
#include "../graphics/renderer/renderer.h"

int Window::width = 1280;
int Window::height = 720;
float Window::aspect = 0;

const char* ENGINE_NAME = "Xenity Engine";
SDL_Window* Window::window = nullptr;

void Window::SetResolution(const int width_, const int height_)
{
    width = width_;
    height = height_;
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
    SDL_Init(SDL_INIT_EVERYTHING);
    unsigned int center = SDL_WINDOWPOS_CENTERED;
    window = SDL_CreateWindow(ENGINE_NAME, center, center, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    SDL_GL_SetSwapInterval(1);
}

void Window::UpdateScreen()
{
    SDL_GL_SwapWindow(window);
}

void Window::UpdateAspectRatio()
{
    aspect = (float)width / (float)height;
}