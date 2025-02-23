// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include <engine/engine.h>
#include <engine/debug/debug.h>

// PSP
#if defined(__PSP__)
#include <pspkernel.h>
PSP_HEAP_THRESHOLD_SIZE_KB(1024); // Reduce heap size to give the memory to threads
PSP_MODULE_INFO("XENITY ENGINE", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
#elif defined(__PS3__)
#include <sys/thread.h>
#include <unistd.h>
// #include <sys/process.h>
// SYS_PROCESS_PARAM(1001, 0x100000); // Crash on real PS3, why?
#endif

//------------------------------- Link to the documentation: https://github.com/Fewnity/Xenity-Engine/tree/crossplatform/Doc

// For the Alpha Release
/*
--- Needed
Rework the documentation
Implement alpha testing on shaders
Check if force inline for GLM is better or not
Add unit tests for commands
Add unit tests for Components Creation/Deletion and Awake/Start/Update/
Try to make a scene with a dynamic mesh (see how it works in Unity/Godot) (Make a simple API that convert vertex data for the current platform)
Try to make a dynamic texture
Implement CreateFolder on console
Do not forget to readd init component ordering in LoadScene
Try to use SubMesh object instead of pointer
Fix PSP sleep mode when networking is enabled
Add the ability to control the game from the editor with network (load scene, quit...)
Add prefab system
Use new indice function for mesh (safer)
Add a update mesh fonction when a mesh is done
Add a function to resize a submesh (resize only if size is different, usefull for text)
Revisit AddVertex params
Fix lighting settings save
Fix delete gameobject and undo with parent
Show if file is selected
Cannot deselect gameobject in multiple selection
Store chunk ptr in mesh renderer
Add more shader uniform check on PS3
Try to build dll with debug symbols
Try to add an option on mesh renderer to disable lights

Try to skip some frame (2 or 3) at the beginning to stabilize the delta time

To make lighting usable on PS3:
- Use vectors instead of single floats in the light structure (spot light missing)
- Try to make directional lighting in the vertex shader
- Try to detect light per draw to use with sphere test (filter light first by camera frustum)
- Try to sort by material and distance

--- Can be done for the next release
Reduce cache size by deleting executable file
Add an error if game.cpp and game.h are not found
Disable the ability to move and delete game.cpp and game.h
Add CI support
Add a way to create a hash from a file (and cross platform)
Add an option to cook assets only
Separate audio platform specific code
Separate mesh platform specific code
Separate socket platform specific code
Separate time platform specific code?
Do not create a new mesh data in text renderer
Disable update function call on many components
Check peekinput for PSP
Reorder if needed the vertex descriptor for platforms like PSP in the cooker
Use VAO for lines (create one sphere model for all gizmo, create one vao for the grid when camera moves)
Fix light in fixed pipeline with 2 cameras
Create a GetFileSize function in File class
New Input system
Implement AudioManager::Stop for PSP and PsVita
Pre-open audio file for faster loading
Compress scene json files in builds
Add assert if project settings file is not found
Add a debug option to show what assets are loaded in vram for psp
Increase PsVita newlib memory?
*/


/*
--- Check if working:
Add ability to move folders into a folder
Check File::GetFolderPath() for psvita, remove ux0:

--- To do quick:
Add ImageRenderer for canvas
Add buttons for canvas
Finish rect transform support
Add the ability to change values with mouse when sliding
Clean Component::SetGameObject code

Add event for input system for the screen size and mouse position
Add event in SceneManager for UpdateWindowTitle?
Move/Store engine name and version somewhere

--- To do soon:
Reload texture/mesh/audio if modified (only if scene is stopped)
Add filter to file dialog
Multiple gameobject selection

--- To do in a long time
Finish UploadMeshData for fixed pipeline or shader mode with currently unused vertex structures
Add a property window on components to have values in a new window
Add network profiler
Add map reflection support
Add more unit tests
Add support for mesh material loading
Fix thread problem when trying to load in different threads the same file at the same time
Add transparency setting on textures
Preview part in inspector : Add mesh preview
Add find function for child only in a gameobject
Tilemap: Reduce memory usage, fix tilemap when removing a texture (remove all id of a texture)
Touch : Add  input state : Begin, Hold (and End?)
Create one shader per material to avoid reupdate shader when using the same shader on multiple material?
Check memory leak when removing an item from vector in the editorUI
Change rightClickedElement and use selected gameobjects if clicked on selected gameobjects
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

	// Weird fix for PS3 to avoid crash on exit
#if defined(__PS3__)
	sleep(3);
#endif

	return 0;
}
