#include "screen.h"
#include <SDL3/SDL_video.h>
#include "window.h"

int Screen::height = 0;
int Screen::width = 0;
bool Screen::useVSync = 0;

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
#if defined(_WIN32) || defined(_WIN64)
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
