#include <engine/engine.h>
#include <engine/debug/debug.h>

// PSP
#if defined(__PSP__)
#include <pspkernel.h>
PSP_HEAP_THRESHOLD_SIZE_KB(1024);
PSP_MODULE_INFO("XENITY ENGINE", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
#endif

//------------------------------- Link to the documentation: https://github.com/Fewnity/Xenity-Engine/tree/crossplatform/Doc

// For the Alpha Release
/*
--- Needed
Show submesh count somewhere
New material System
Use white texture if no texture on material
New Input system
Fix variables order
Text scale
Fix rotation tool texture uv
Fix crash if rotation values are infinite
Fix duplication crash with missing script
Multiple gameobject selection
Clean folders
Create crash dump file
Remove engine location settings
Rework the documentation
Clean code
Update read me

--- Can be for next release

--- Done
Fix error when loading mesh
Add try catch to all json parse
Clear scene if fail loading scene
*/


/*
--- Check if working:
Add ability to move folders into a folder
Check File::GetFolderPath() for psvita, remove ux0:

--- To do quick:
Fix crash if rotation values are infinite
Add ImageRenderer for canvas
Add buttons for canvas
Finish rect transform support
Add Windows controllers support
Add the ability to change values with mouse when sliding
Find a way to avoid duplicating missing script
Build and run on real harward for psp
Fix duplication crash with missing script
Clean Component::SetGameObject code

Add event for input system for the screen size and mouse position
Add event in SceneManager for UpdateWindowTitle?
Move/Store engine name and version somewhere

--- To do soon:
Reload texture/mesh/audio if modified (only if scene is stopped)
Fix Awake : Call awake when a gameobject was disabled and then enabled
Add filter to file dialog
Text scale
Disable scene save if the project is not compiled

--- To do in a long time
Add try catch to all json parse
Regenerate an id the a meta file is corrupted
Add a property window on components to have values in a new window
Fix engine assets system
Add network profiler
Add map reflection support
Add more unit tests
RGBA or RGB in vertex color?
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
Create one shader per material to avoid reupdate shader when using the same shader on multiple material?
*/

#undef main

int main(int argc, char* argv[])
{
	// Init engine
	const int engineInitResult = Engine::Init();
	if (engineInitResult != 0)
	{
		Debug::PrintError("-------- Engine failed to init --------", true);
		return -1;
	}

	// Engine and game loop
	Engine::Loop();
	Debug::Print("-------- Game loop ended --------", true);
	Engine::Stop();

	return 0;
}