#include "main.h"

#define STB_IMAGE_IMPLEMENTATION

#include "xenity.h"

// PSP
#if defined(__PSP__)
#include "pspkernel.h"
PSP_MODULE_INFO("XENITY ENGINE", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);
#endif

/*
For tomorrow
Preview part in inspector : Add mesh preview
Add find function for child only in a gameobject
Finish optimise LoadScene
Fix Awake : Call awake when a gameobject was disabled and the enabled
Apply texture filter before drawing imgui images
Add filter to file dialog
Use weak instead of Shared for files
Add Create Skybox/Scene functions
Add ability to rename folder and files
Add abitily to move file into a folder
Add LOD
Add fog

Add more unit tests

Add delete texture function in renderer for psp
Fix mouse speed at high fps
Add transparency setting on textures
Packing asset system
Add a tool to rotate selected gameobject
Fix tool with child and parent negative scale
Find why ./ point to another location when lauched from a cmd without "cd"
Detect the number of cpu core for cmake
Create console window
Check FileSystem::GetGamePath 
Check if CreateGameObjectEditor's GameObjects can be found with findGameObject

Add materials
Add shaders
Fix uniqueID when loading a new scene (set id to the max find in the file)
Tilemap: Reduce memory usage, fix tilemap when removing a texture (remove all id of a texture)
Touch : Add input state : Begin, Hold (and End?)
Text scale
nextPow2 and previousPow2 fix with negative number
*/

#undef main

int main(int argc, char *argv[])
{
	// Init engine
	if (Engine::Init() != 0)
	{
		Debug::PrintError("-------- Engine failed to init --------");
		return -1;
	}

	// Engine and game loop
	Engine::Loop();
	Debug::Print("-------- Game loop ended --------");
	Engine::Stop();

	return 0;
}