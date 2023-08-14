#pragma once
#if defined(_WIN32) || defined(_WIN64)
#include <SDL2/SDL.h>
#endif

class Window
{
public:
    static void SetResolution(const int width_, const int height_);
    static int GetWidth();
    static int GetHeight();
    static float GetAspectRatio();
    static void Init();
    static void UpdateScreen();
    static void UpdateWindowTitle();
#if defined(_WIN32) || defined(_WIN64)
    static SDL_Window *window;
#endif
private:
    static void UpdateAspectRatio();
    static int width, height;
    static float aspect;
    static void OnResize();
};