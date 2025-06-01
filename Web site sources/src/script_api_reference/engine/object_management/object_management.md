# Object management

Add this in your code:
```cpp
#include <engine/tools/gameplay_utility.h>
```

## Description

Class to manage Objects (GameObject and Components).<br>
You can Instantiate, Check and Destroy Objects.

## Methods

---
### IsValid
Check if a GameObject or a Component is valid to use.<br>
If an object is waiting to be destroyed, the object is not valid anymore.<br>
Only takes Component, GameObject and Transform.

Parameters:
- `pointer`:  Pointer to the object to check
```cpp
template <typename T>
bool IsValid(const std::shared_ptr<T>& pointer)
bool IsValid(const std::weak_ptr<T>& pointer)
```
Code sample:
```cpp
std::shared_ptr<GameObject> myGameObject = CreateGameObject("MyGO");

if(IsValid(myGameObject))
{
    // This object is valid
}

Destroy(myGameObject);

if(IsValid(myGameObject))
{
    // This object is not valid anymore, this if won't be executed
}

```

---
### Instantiate
Create a new GameObject from another
(Not very recommended, can be buggy, use prefabs instead)

Parameters:
- `gameObject`:  GameObject to duplicate
```cpp
std::shared_ptr<GameObject> Instantiate(const std::shared_ptr<GameObject>& gameObject)
```
Code sample:
```cpp
std::shared_ptr<GameObject> myGameObject = CreateGameObject("MyGO");

std::shared_ptr<GameObject> myGameObject2 = Instantiate(myGameObject);
```

---
### Instantiate
Create a new GameObject from a prefab

Parameters:
- `prefab`:  Prefab to instanciate
```cpp
std::shared_ptr<GameObject> Instantiate(const std::shared_ptr<Prefab>& prefab)
```
Code sample:
```cpp
std::shared_ptr<GameObject> myGameObject = Instantiate(myPrefab);
```

---
### Destroy
Destroy a GameObject or component
> [!WARNING]
> The pointer won't be null right after the Destroy call.<br>
> Do not use the object after calling Destroy.<br>
> Objects are fully destroy after the begin of the next frame.

Parameters:
- `x`:  Object to destroy
```cpp
void Destroy(const std::shared_ptr<Component>& component) 
void Destroy(const std::shared_ptr<GameObject>& gameObject);
void Destroy(const std::weak_ptr<Component>& component) 
void Destroy(const std::weak_ptr<GameObject>& gameObject);
```
Code sample:
```cpp
std::shared_ptr<GameObject> myGameObject = CreateGameObject("MyGO");

Destroy(myGameObject);
```