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
--- To do quick:
Check Setlights(camera);
Change Opened scene on save
Texture filter system
Fix psp set transform with matrix
Add map reflection support
Make shader for psvita
Fix all errors of cppcheck
Update shaders projection/position when using new camera and when switching to UI mode
Create one shader per material to avoid reupdate shader when using the same shader on multiple material?

--- To do soon:
Fix Awake : Call awake when a gameobject was disabled and then enabled
Add filter to file dialog
Add abitily to move file into a folder
Add more unit tests
Add delete texture function in renderer for psp
Create console window
RGBA or RGB in vertex color?

--- To do in a long time 
Add LOD
Fix mouse speed at high fps
Add transparency setting on textures
Packing asset system
Add a tool to rotate selected gameobject
Fix tool with child and parent negative scale
Detect the number of cpu core for cmake
Check FileSystem::GetGamePath 
Check if CreateGameObjectEditor's GameObjects can be found with findGameObject
Preview part in inspector : Add mesh preview
Add find function for child only in a gameobject
Change ID system, find a way to have an unique ID for files
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