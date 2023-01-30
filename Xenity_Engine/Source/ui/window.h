#pragma once

#include <SDL2/SDL.h>
#include <string>

class Window
{
public:
	static int InitWindow();
	static void UpdateScreen();
	static void Resize(int newWidth, int newHeight);
	static void SetTitle(const std::string title);
	static SDL_Window* window;
	static void SetWidth(int width_);
	static void SetHeight(int height_);
	static int GetWidth();
	static int GetHeight();
private:
	static int width, height;
};