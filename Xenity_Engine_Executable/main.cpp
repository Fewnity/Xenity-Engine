// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include <engine/engine.h>

// PSP
#if defined(__PSP__)
#include <pspkernel.h>
PSP_HEAP_THRESHOLD_SIZE_KB(1024); // Reduce heap size to give the memory to threads
PSP_MODULE_INFO("XENITY ENGINE", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
#endif

#include <engine/debug/debug.h>

//------------------------------- Link to the documentation: https://github.com/Fewnity/Xenity-Engine/tree/crossplatform/Doc

// For the Alpha Release
/*
--- Needed
Add undo/redo commands to DrawVector functions
Rework the documentation
Box colliders are not affected by gameobject scale
Do not ask to save scene when playing
Add assert if project settings file is not found
Add an option to enable /disable the profiler in builds
Add trigger event
Add raycast
Fix shader tilling + check on PSP

--- Can be for next release
Remove the old profiler
New Input system
Implement AudioManager::Stop for PSP and PsVita
Pre-open audio file for faster loading
Replace CompatibleFile by FileInfo?

--- Done


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
Build and run on real harward for psp
Clean Component::SetGameObject code

Add event for input system for the screen size and mouse position
Add event in SceneManager for UpdateWindowTitle?
Move/Store engine name and version somewhere

--- To do soon:
Reload texture/mesh/audio if modified (only if scene is stopped)
Fix Awake : Call awake when a gameobject was disabled and then enabled
Add filter to file dialog
First PSP frame doest not use light
Multiple gameobject selection

--- To do in a long time
Regenerate an id if a meta file is corrupted
Finish UploadMeshData for fixed pipeline or shader mode with currently unused vertex structures
Add a property window on components to have values in a new window
Add network profiler
Add map reflection support
Add more unit tests
Add support for mesh material loading
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
Check memory leak when removing an item from vector in the editorUI
multiple directional light not working properly
Change rightClickedElement and use selected gameobjects if clicked on selected gameobjects
Use enum is InspectorCreateGameObjectCommand instead of int
Create OnReflectionUpdated in transform and set isTransformationMatrixDirty = true; and call UpdateWorldValues in the function?
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