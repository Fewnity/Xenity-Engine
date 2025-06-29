// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "screen.h"

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
#include <SDL3/SDL_video.h>
#endif
#if !defined(EDITOR)
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include <stb_image_write.h>

#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>
#include "window.h"

#if defined(EDITOR)
#include <editor/editor.h>
#include <editor/ui/menus/basic/game_menu.h>
#endif
int Screen::s_height = 0;
int Screen::s_width = 0;
bool Screen::s_useVSync = true;

int Screen::GetWidth()
{
#if defined(EDITOR)
	if (Editor::s_lastFocusedGameMenu.lock() != nullptr)
	{
		const Vector2 windowsSize = std::dynamic_pointer_cast<GameMenu>(Editor::s_lastFocusedGameMenu.lock())->lastSize;
		return static_cast<int>(windowsSize.x);
	}
#endif
	return s_width;
}

int Screen::GetHeight() 
{ 
#if defined(EDITOR)
	if (Editor::s_lastFocusedGameMenu.lock() != nullptr)
	{
		const Vector2 windowsSize = std::dynamic_pointer_cast<GameMenu>(Editor::s_lastFocusedGameMenu.lock())->lastSize;
		return static_cast<int>(windowsSize.y);
	}
#endif
	return s_height; 
}

void Screen::SetFullScreen(bool useFullScreenMode)
{
	// Do not change the fullscreen mode in the editor
#if !defined(EDITOR)
	Window::SetFullScreenMode(useFullScreenMode);
#endif
}

void Screen::SetVSync(bool _useVSync)
{
	s_useVSync = _useVSync;
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
	if (_useVSync) 
	{
		SDL_GL_SetSwapInterval(1);
	}
	else 
	{
		SDL_GL_SetSwapInterval(0);
	}
#endif
}

bool Screen::IsVSyncEnabled()
{
	return s_useVSync;
}

bool Screen::MakeScreenshot(const std::string& fileName)
{
	if (Graphics::usedCamera)
	{
		std::unique_ptr<uint8_t[]> frameBufferData = Graphics::usedCamera->GetRawFrameBuffer();
		if (!frameBufferData)
		{
			Debug::PrintError("Failed to get the framebuffer data");
			return false;
		}

		std::string path = fileName + ".png";

#if defined(__vita__)
		path = std::string(PSVITA_DEBUG_LOG_FOLDER) + "screenshots/" + path;
#elif defined(__PS3__)
		path = Application::GetGameFolder() + path;
#endif

		// On some platforms, the framebuffer is flipped vertically
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__) || defined(__vita__)
		stbi_flip_vertically_on_write(true); // Windows / PsVita
#else // PSP, PS3
		stbi_flip_vertically_on_write(false); // PSP / PS3
#endif

		if (stbi_write_png(path.c_str(), Graphics::usedCamera->GetWidth(), Graphics::usedCamera->GetHeight(), 3, frameBufferData.get(), 0) == 0)
		{
			Debug::PrintError("Failed to export the framebuffer data");
			return false;
		}

		return true;
	}

	return false;
}
