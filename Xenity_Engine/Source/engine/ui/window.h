#pragma once
#if defined(_WIN32) || defined(_WIN64)
#include <SDL2/SDL.h>
#endif

enum WindowError
{
    WND_ERROR_SDL_INIT = -1,
    WND_ERROR_SDL_CREATE_WINDOW = -2,
    WND_ERROR_SDL_GL_CONTEXT = -3
};

class Window
{
public:
    static void SetResolution(const int width_, const int height_);
    static int GetWidth();
    static int GetHeight();
    static float GetAspectRatio();
    static int Init();
    static void UpdateScreen();
    static void UpdateWindowTitle();
#if defined(_WIN32) || defined(_WIN64)
    static SDL_Window *window;
#endif
    static void SetFullScreenMode(bool enable);

private:
    static void UpdateAspectRatio();
    static int width, height;
    static float aspect;
    static void OnResize();
};