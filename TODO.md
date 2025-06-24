## For the next release (V1.0)

- Add unit tests for modify commands and Awake/Start/Update/
- Rework the Screen::MakeScreenshot to be able to use in game loop
- Implement alpha testing on PsVita shaders
- Fix PSP sleep mode when networking is enabled
- Add trigger values for gamepads
- Add PS3 multi gamepad support
- Add buttons for canvas
- CTRL+Z does not work on UI elements and gizmo arrows

## Can be done for a later release

### High priority
- Try fix include on Linux in asset selection menu and EditorUI
- Re-add init component ordering in LoadScene and order component update
- Add an error if game.cpp and game.h are not found
- Disable the ability to move and delete game.cpp and game.h
- Finish rect transform support
- Add filter to file dialog

### Medium priority
- Add a function to resize a submesh (resize only if size is different, usefull for text)
- Try to remove rotation and local rotation from transform, maybe remove position
- Check why the tv texture does not load on PSP (Specific to my project)
- Try to build dll with debug symbols
- Check if force inline for GLM is better or not
- Reduce cache size by deleting executable file
- Separate audio platform specific code into more files
- Separate mesh platform specific code into more files
- Separate socket platform specific code into more files
- Separate time platform specific code into more files?
- Create a GetFileSize function in File class
- Add the ability to change values with mouse when sliding in the inspector
- Reload texture/mesh/audio if modified (only if scene is stopped)
- Add a property window on components to have values in a new window
- Add a Find function for child only in a gameobject
- Check memory leak when removing an item from vector in the EditorUI (element is removed from the list but not deleted)

### Low priority (Not important features or complex tasks)
- Multiple gameobject selection (Very complex task)
- Try to remove some enable_shared_from_this (GameObject, Transform, Component, Filereference)
- Use octree for light culling
- Add the ability to control the game from the editor with network (load scene, quit...)
- Add CI support
- Add a way to create a hash from a file (and cross platform)
- Add an option to cook assets only
- Do not create a new mesh data in text renderer
- Check peekinput for PSP instead of sceCtrlReadBufferPositive if it's better for performances
- Reorder if needed the vertex descriptor for platforms like PSP in the cooker
- Use VAO for lines (create one sphere model for all gizmo, create one vao for the grid when camera moves)
- Pre-open audio file for faster loading
- Compress scene json files in builds
- Add a debug option to show what assets are loaded in vram for psp
- Increase PsVita newlib memory?
- Add event for input system for the screen size and mouse position
- Add event in SceneManager for UpdateWindowTitle?
- Clean Component::SetGameObject code
- Add network profiler
- Add std::map reflection support (base on std::vector support)
- Add support for mesh material loading using Assimp
- Fix thread problem when trying to load in different threads the same file at the same time
- Add transparency setting on textures to reduce size in memory if alpha is not needed
- Preview part in inspector : Add mesh preview
- Tilemap: Refactor the whole tilemap system
- Touch : Add  input state : Begin, Hold (and End?)
- Create OnReflectionUpdated in transform and set isTransformationMatrixDirty = true; and call UpdateWorldValues in the function?
- Change rightClickedElement and use selected gameobjects if clicked on selected gameobjects