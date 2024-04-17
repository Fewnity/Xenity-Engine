#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include <SDL3/SDL.h>
#endif

enum class WindowError
{
	WND_ERROR_SDL_INIT = -1,
	WND_ERROR_SDL_CREATE_WINDOW = -2,
	WND_ERROR_SDL_GL_CONTEXT = -3
};

class Window
{
public:
	/**
	* @brief Set window resolution
	* @param width_ Width of the window
	* @param height_ Height of the window
	*/
	static void SetResolution(const int width_, const int height_);

	/**
	* @brief Get window width
	*/
	static int GetWidth();

	/**
	* @brief Get window height
	*/
	static int GetHeight();

	/**
	* @brief Get window's title bar height
	*/
	static int GetTitleBarHeight();

	/**
	* @brief Get window's aspect ratio
	*/
	static float GetAspectRatio();

	/**
	 * @brief [Internal] Initialize the window
	 */
	static int Init();

	/**
	 * @brief [Internal] Update the window
	 */
	static void UpdateScreen();

	/**
	* @brief [Internal] Update the window title
	*/
	static void UpdateWindowTitle();

	/**
	* @brief Set the window title
	*/
	static void SetFullScreenMode(bool enable);

#if defined(_WIN32) || defined(_WIN64)
	static SDL_Window* window;
#endif

private:
	/**
	* @brief [Internal] Update the aspect ratio
	*/
	static void UpdateAspectRatio();

	/**
	* @brief [Internal] Function called when the window is resized
	*/
	static void OnResize();

	static int width, height;
	static float aspect;
};