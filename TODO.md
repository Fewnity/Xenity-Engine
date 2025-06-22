## For the next release

Rework the documentation
Add unit tests for modify commands and Awake/Start/Update/
Rework the Screen::MakeScreenshot to be able to use in game loop

Fix PSP sleep mode when networking is enabled

To make lighting usable on PS3:
- Try to detect light per draw to use with sphere test (filter light first by camera frustum)

## Can be done for later release
Implement alpha testing on shaders
Add a function to resize a submesh (resize only if size is different, usefull for text)
Try to remove some enable_shared_from_this (GameObject, Transform, Component, Filereference)
Try to remove rotation and local rotation from transform, maybe remove position
Use flags for gameobject
Use flags for components
Try fix include on Linux in asset selection menu and EditorUI
Do not forget to re-add init component ordering in LoadScene
Use octree for light culling
Check why the tv texture does not load on PSP
Check how referenced files are handled when in a missing_script
Try to build dll with debug symbols
Check if force inline for GLM is better or not
Add the ability to control the game from the editor with network (load scene, quit...)
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
Check peekinput for PSP
Reorder if needed the vertex descriptor for platforms like PSP in the cooker
Use VAO for lines (create one sphere model for all gizmo, create one vao for the grid when camera moves)
Fix light in fixed pipeline with 2 cameras
Create a GetFileSize function in File class
New Input system
Pre-open audio file for faster loading
Compress scene json files in builds
Add assert if project settings file is not found
Add a debug option to show what assets are loaded in vram for psp
Increase PsVita newlib memory?


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

--- To do soon:
Reload texture/mesh/audio if modified (only if scene is stopped)
Add filter to file dialog
Multiple gameobject selection

--- To do in a long time
Add a property window on components to have values in a new window
Add network profiler
Add map reflection support
Add support for mesh material loading
Fix thread problem when trying to load in different threads the same file at the same time
Add transparency setting on textures
Preview part in inspector : Add mesh preview
Add find function for child only in a gameobject
Tilemap: Reduce memory usage, fix tilemap when removing a texture (remove all id of a texture)
Touch : Add  input state : Begin, Hold (and End?)
Check memory leak when removing an item from vector in the editorUI
Change rightClickedElement and use selected gameobjects if clicked on selected gameobjects
Create OnReflectionUpdated in transform and set isTransformationMatrixDirty = true; and call UpdateWorldValues in the function?
