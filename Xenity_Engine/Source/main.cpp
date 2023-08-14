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
For tomorrow
Add OnReflectionUpdate function
Improve play mode (Create scene backup before play, restore scene on stop)
Add unit tests
Check Psp light x position
Implement functions of right click file popup
Add Ctrl + S to save scene

Add Drag and Drop for GameObjects
Add Drag and Drop for Components
Add unselect button
Add unbind file variable button
Improve file explorer code
Automatic file unload system
Re add delete texture function in renderer

Add transparency setting on textures
Packing asset system
Multi texture per models obj loader
Add a tool to rotate selected gameobject
Fix tool with child and parent negative scale
Find why ./ point to another location when lauched from a cmd without "cd"
Detect the number of cpu core for cmake

Add materials
Add async texture loading
Add shaders
Add sounds on windows
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