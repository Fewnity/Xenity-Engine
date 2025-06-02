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
Create a GameObject with the default name.
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
Create a GameObject.

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
Find a GameObject with a name.

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
Find GameObjects with a name.

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
Find a GameObject with an id.

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
Find a component with an id.

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

---
### AddChild
Add a child to the GameObject.

Parameters:
- `gameObject`: Child to add
```cpp
void AddChild(const std::shared_ptr<GameObject>& gameObject)
```
Code sample:
```cpp
std::shared_ptr<GameObject> myGameObject = CreateGameObject("Player");
std::shared_ptr<GameObject> myChild = CreateGameObject("Gun");
myGameObject->AddChild(myChild);
```

---
### SetParent
Set GameObject's parent.

Parameters:
- `gameObject`: New parent
```cpp
void SetParent(const std::shared_ptr<GameObject>& gameObject)
```
Code sample:
```cpp
std::shared_ptr<GameObject> player0 = CreateGameObject("Player");
std::shared_ptr<GameObject> player1 = CreateGameObject("Player");
std::shared_ptr<GameObject> parent = CreateGameObject("Players");
player0->SetParent(parent);
player1->SetParent(parent);
```

---
### AddComponent
Add a component.
```cpp
template <typename T>
AddComponent()
```
Code sample:
```cpp
std::shared_ptr<GameObject> gameObject = CreateGameObject();
std::shared_ptr<Light> lightComp = gameObject->AddComponent<Light>();
std::shared_ptr<AudioSource> audioSourceComp = gameObject->AddComponent<AudioSource>();
```

---
### GetComponent
Get a component
```cpp
template <typename T>
GetComponent()
```
Code sample:
```cpp
std::shared_ptr<GameObject> gameObject = CreateGameObject();
std::shared_ptr<Light> lightComp = gameObject->AddComponent<Light>();
std::shared_ptr<Light> comp = gameObject->GetComponent<Light>();
```

---
### GetComponents
Get components
```cpp
template <typename T>
GetComponents()
```
Code sample:
```cpp
std::shared_ptr<GameObject> gameObject = CreateGameObject();
std::shared_ptr<Light> lightComp = gameObject->AddComponent<Light>();
std::shared_ptr<Light> lightComp2 = gameObject->AddComponent<Light>();
std::vector<std::shared_ptr<Light>> lightsComp = gameObject->GetComponents<Light>();
```

---
### IsActive
Get if the GameObject is marked as active.
```cpp
bool IsActive() const
```
Code sample:
```cpp
std::shared_ptr<GameObject> gameObject = CreateGameObject();
gameObject->SetActive(false);
bool isActive = gameObject->IsActive(); // = false

//....

std::shared_ptr<GameObject> gameObject = CreateGameObject();
std::shared_ptr<GameObject> child = CreateGameObject();
child->SetParent(child);
gameObject->SetActive(false);
bool isActive = child->IsActive(); // = true
```

---
### IsLocalActive
Get if the GameObject is active based on his parents active state.
```cpp
bool IsLocalActive() const
```
Code sample:
```cpp
std::shared_ptr<GameObject> gameObject = CreateGameObject();
gameObject->SetActive(false);
bool isActive = gameObject->IsLocalActive(); // = false

//....

std::shared_ptr<GameObject> gameObject = CreateGameObject();
std::shared_ptr<GameObject> child = CreateGameObject();
child->SetParent(child);
gameObject->SetActive(false);
bool isActive = child->IsLocalActive(); // = false
```

---
### SetActive
Set GameObject as active or not.

Parameters:
- `active`: Active value
```cpp
void SetActive(const bool active)
```
Code sample:
```cpp
std::shared_ptr<GameObject> gameObject = CreateGameObject();
gameObject->SetActive(false);
bool isActive = gameObject->IsActive(); // = false
```

---
### GetChildrenCount
Get children count.
```cpp
uint32_t GetChildrenCount() const
```
Code sample:
```cpp
std::shared_ptr<GameObject> gameObject = CreateGameObject();
std::shared_ptr<GameObject> child0 = CreateGameObject();
std::shared_ptr<GameObject> child1 = CreateGameObject();

gameObject->AddChild(child0);
gameObject->GetChildrenCount(false);
bool isActive = gameObject->IsActive(); // = false
```