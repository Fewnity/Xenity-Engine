#include "main.h"

#define STB_IMAGE_IMPLEMENTATION

#include "xenity.h"

// PSP
#ifdef __PSP__
PSP_MODULE_INFO("XENITY ENGINE", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
#endif

/*
Add materials
Add shaders
Add sounds
Add line renderer
Tilemap: Reduce memory usage, fix tilemap when removing a texture (remove all id of a texture)
Touch : Add input state : Begin, Hold and End
Text scale
*/

int main(int argc, char *argv[])
{
	// std::string exePath = argv[0];
	std::string exePath = "";
	if (Engine::Init(exePath) != 0)
	{
		Debug::PrintError("Engine failed to init");
		return -1;
	}

	Engine::Loop();
	Debug::Print("-------- Game loop ended --------");
	Engine::Stop();

	return 0;
}