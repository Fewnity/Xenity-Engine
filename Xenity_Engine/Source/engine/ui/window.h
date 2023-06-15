#pragma once

#include <SDL2/SDL.h>

class Window
{
public:
    static void SetResolution(const int width_, const int height_);
    static int GetWidth();
    static int GetHeight();
    static float GetAspectRatio();
    static void Init();
    static void UpdateScreen();
    static SDL_Window* window;

private:
    static void UpdateAspectRatio();
    static int width, height;
    static float aspect;
};