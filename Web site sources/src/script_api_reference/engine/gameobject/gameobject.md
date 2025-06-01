# GameObject

Add this in your code:
```cpp
#include <engine/game_elements/gameobject.h>
```

## Description

A GameObject is a class that contains some properties and a list of components.<br>
Includes functions to create GameObjects.

## Global Methods

---
### CreateGameObject
Create a GameObject with the default name
```cpp
std::shared_ptr<GameObject> CreateGameObject()
```
Code sample:
```cpp
std::shared_ptr<GameObject> myGameObject = CreateGameObject();
Debug::Print(myGameObject->GetName()); // Prints "GameObject"
```

---
### CreateGameObject
Create a GameObject

Parameters:
- `name`: Name of the GameObject
```cpp
std::shared_ptr<GameObject> CreateGameObject(const std::string& name)
```
Code sample:
```cpp
std::shared_ptr<GameObject> myGameObject = CreateGameObject("Player");
Debug::Print(myGameObject->GetName()); // Prints "Player"
```

---
### FindGameObjectByName
Find a GameObject with a name

Parameters:
- `name`: Name of the GameObject
```cpp
std::shared_ptr<GameObject> FindGameObjectByName(const std::string& name)
```
Code sample:
```cpp
std::shared_ptr<GameObject> myGameObject = CreateGameObject("Player");
std::shared_ptr<GameObject> result = FindGameObjectByName("Player");
if(myGameObject == result)
{
    Debug::Print("GameObject found!");
}
```

---
### FindGameObjectsByName
Find GameObjects with a name

Parameters:
- `name`: Name of the GameObject
```cpp
std::vector<std::shared_ptr<GameObject>> FindGameObjectsByName(const std::string& name)
```
Code sample:
```cpp
std::shared_ptr<GameObject> myGameObject = CreateGameObject("Player");
std::shared_ptr<GameObject> myGameObject2 = CreateGameObject("Player");
std::vector<std::shared_ptr<GameObject>> results = FindGameObjectsByName("Player");
if(results.size() == 2)
{
    Debug::Print("Players found!");
}
```

---
### FindGameObjectById
Find a GameObject with an id

Parameters:
- `id`: Unique id of the GameObject
```cpp
std::shared_ptr<GameObject> FindGameObjectById(const uint64_t id)
```
Code sample:
```cpp
std::shared_ptr<GameObject> myGameObject = CreateGameObject("Player");
std::shared_ptr<GameObject> result = FindGameObjectById(myGameObject->GetUniqueId());
if(myGameObject == result)
{
    Debug::Print("GameObject found!");
}
```

---
### FindComponentById
Find a component with an id

Parameters:
- `id`: Unique id of the component
```cpp
std::shared_ptr<GameObject> FindComponentById(const uint64_t id)
```
Code sample:
```cpp
std::shared_ptr<GameObject> myGameObject = CreateGameObject("Player");
std::shared_ptr<AudioSource> myAudioSource = myGameObject->AddComponent<AudioSource>();
std::shared_ptr<Component> result = FindComponentById(myAudioSource->GetUniqueId());
if(myAudioSource == result)
{
    std::shared_ptr<AudioSource> resultAudioSource = std::dynamic_pointer_cast<AudioSource>(result);
    Debug::Print("Component found!");
}
```

## Pubic Methods