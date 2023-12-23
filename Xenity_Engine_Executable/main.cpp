#include <engine/engine.h>
#include <engine/debug/debug.h>

// PSP
#if defined(__PSP__)
#include <pspkernel.h>
PSP_MODULE_INFO("XENITY ENGINE", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
#endif

//------------------------------- Link to the documentation: https://github.com/Fewnity/Xenity-Engine/tree/crossplatform/Doc

/*
--- To do quick:
Add map reflection support
Check File::GetFolderPath() for psvita, remove ux0:
Add network profiler
Fix engine assets system
Reload texture/mesh/audio if modified (only if scene is stopped)
Add abitily to move folders into a folder
Add a property window on components to have values in a new window

--- To do soon:
Fix Awake : Call awake when a gameobject was disabled and then enabled
Add filter to file dialog
Add more unit tests
Add delete texture function in renderer for psp
Create console window
RGBA or RGB in vertex color?

--- To do in a long time
Add LOD
Fix thread problem when trying to load in different threads the same file at the same time
Add transparency setting on textures
Packing asset system
Add a tool to rotate selected gameobject
Fix tool with child and parent negative scale
Preview part in inspector : Add mesh preview
Add find function for child only in a gameobject
Change ID system, find a way to have an unique ID for files
Tilemap: Reduce memory usage, fix tilemap when removing a texture (remove all id of a texture)
Touch : Add  input state : Begin, Hold (and End?)
Text scale
Windows controller support
Create one shader per material to avoid reupdate shader when using the same shader on multiple material?
*/

#undef main

int main(int argc, char* argv[])
{
	// Init engine
	int engineInitResult = Engine::Init();
	if (engineInitResult != 0)
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