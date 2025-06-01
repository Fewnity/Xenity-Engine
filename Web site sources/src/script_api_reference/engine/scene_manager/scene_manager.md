# SceneManager

Add this in your code:
```cpp
#include <engine/scene_manager/scene_manager.h>
```

## Description

Class to load scenes

## Static methods

---
### LoadScene
Load a scene

Parameters:
- `scene`: Scene to load
```cpp
void LoadScene(const std::shared_ptr<Scene>& scene)
```
Code sample:
```cpp
std::shared_ptr<Scene> sceneToLoad; // Filled variable

SceneManager::LoadScene(sceneToLoad);
```

---
### ReloadScene
Reload the current scene
```cpp
void ReloadScene()
```
Code sample:
```cpp
SceneManager::ReloadScene();
```

---
### GetOpenedScene
Get opened scene
```cpp
const std::shared_ptr<Scene>& GetOpenedScene()
```
Code sample:
```cpp
std::shared_ptr<Scene> currentScene = SceneManager::GetOpenedScene();
```