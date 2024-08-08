// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "screen.h"

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
#include <SDL3/SDL_video.h>
#endif

#include "window.h"

int Screen::height = 0;
int Screen::width = 0;
bool Screen::useVSync = true;

void Screen::SetFullScreen(bool useFullScreenMode)
{
	// Do not change the fullscreen mode in the editor
#if !defined(EDITOR)
	Window::SetFullScreenMode(useFullScreenMode);
#endif
}

void Screen::SetVSync(bool _useVSync)
{
	useVSync = _useVSync;
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
	return useVSync;
}
