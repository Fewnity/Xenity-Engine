#include "main.h"

//#define STB_IMAGE_IMPLEMENTATION

#include "xenity.h"

// PSP
#if defined(__PSP__)
#include "pspkernel.h"
PSP_MODULE_INFO("XENITY ENGINE", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
#endif

/*
--- To do quick:
Reopen scene on renaming
Fix editor camera when loading a scene whitout camera in game
Add map reflection support
Update shaders projection/position when using new camera and when switching to UI mode
Create one shader per material to avoid reupdate shader when using the same shader on multiple material?
Material::SetAttribut check emplace, try_emplace?
Fix camera when mouse leaves the scene window
Check File::GetFolderPath() for psvita, remove ux0:
Fix camera when multisampled is disabled on windows build
Add network profiler
Check why files are deleted (RemoveUnusedFiles) on scene save

useLighting not working
Light still here when disabling component/gameobject
Fix destroy

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
*/

#undef main

int main(int argc, char* argv[])
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