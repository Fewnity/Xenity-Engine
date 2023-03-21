#pragma once

#include <SDL2/SDL.h>
#include <string>

class Window
{
public:
	static int InitWindow();
	static void UpdateScreen();
	static void OnResize(const int newWidth, const int newHeight);
	static void SetTitle(const std::string title);
	static SDL_Window* window;
	static void SetWidth(const int width_);
	static void SetHeight(const int height_);
	static int GetWidth();
	static int GetHeight();
	static float GetAspectRatio();

private:
	static int width, height;
	static float aspect;
};