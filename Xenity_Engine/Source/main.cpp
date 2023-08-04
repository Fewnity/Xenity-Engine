#include "main.h"

#define STB_IMAGE_IMPLEMENTATION

#include "xenity.h"

// PSP
#ifdef __PSP__
#include "pspkernel.h"
PSP_MODULE_INFO("XENITY ENGINE", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
#endif

/*
Add Drag and Drop for GameObjects
Add Drag and Drop for Components
Add unselect button
Add unbind file variable button
Add right click on files to remove files
Improve file explorer code
Automatic file unload system
Re add delete texture function in renderer

Check how to use WSL
Add a system to copy editor files and game files to wsl
Add a system to Create Makefile folder and Makefile file
Add play mode
Have more than one camera
Fix debug warning and Error in file

Add materials
Add async texture loading
Add shaders
Add sounds on windows
Add unit tests
Fix uniqueID when loading a new scene (set id to the max find in the file)
Tilemap: Reduce memory usage, fix tilemap when removing a texture (remove all id of a texture)
Touch : Add input state : Begin, Hold (and End?)
Text scale
*/

#undef main

int main(int argc, char *argv[])
{
	// Init engine
	// std::string exePath = argv[0];
	std::string exePath = "";
	if (Engine::Init(exePath) != 0)
	{
		Debug::PrintError("Engine failed to init");
		return -1;
	}

	// Engine and game loop
	Engine::Loop();
	Debug::Print("-------- Game loop ended --------");
	Engine::Stop();

	return 0;
}