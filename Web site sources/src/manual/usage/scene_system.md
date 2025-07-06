# How a scene works

In Xenity, a Scene is a container that holds and manages a collection of GameObjects and their associated Components. It represents a specific state or environment in the game, such as a level, a menu, or a cutscene.

Each GameObject in the scene acts as an entity in the world and can hold one or more Components, which define its behavior or appearance (e.g., a Transform, MeshRenderer, Collider, or Script).

When a scene is loaded, Xenity initializes all the GameObjects and their Components, and the engine begins updating them each frame. Scenes can be loaded, unloaded, or switched during runtime, allowing for smooth transitions between different parts of the game.

If a component or gameobject is destroyed using the Destroy command (see [Object Management](../../script_api_reference/engine/object_management/object_management.md)), the object is destroyed from the memory at the end of the frame.

All assets referenced in the scene will be loaded while the scene is loading. The asset loading is asynchrone on the editor, so textures may look wrong for a small amount of time.

A scene always should have an active camera, you should not add more cameras.

You can load scenes using the [SceneManager](../../script_api_reference/engine/scene_manager/scene_manager.md) class.
See the [Scene](../../script_api_reference/engine/assets/scene.md) asset class.