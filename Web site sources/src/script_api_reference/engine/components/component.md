# Component

Add this in your code:
```cpp
#include <engine/component.h>
```

## Description

The component class gives you a list of function that you can override in your components.

## Protected variables

---
| Name | Type | Description |
|-|-|-|
m_componentName | `const std::string*` | Pointer to the component name

## Virtual methods

---
### Awake
This function is called once before Start() at the creation of the component.

The engine calls the function when:
- The parent GameObject is active when a scene just loaded
- The parent GameObject goes from inactive to active

The parent GameObject and the component should be enabled.
```cpp
virtual void Awake()
```
Code sample:
```cpp
class API MyComponent : public Component
{
public:
    void Awake() override
    {
        Debug::Print("Awake called!");
    }
}
```

---
### Start
This function is called once after Awake() at the creation of the component.

The engine calls the function when:
- The parent GameObject is active when a scene just loaded
- The parent GameObject goes from inactive to active

The parent GameObject and the component should be enabled.
```cpp
virtual void Start()
```
Code sample:
```cpp
class API MyComponent : public Component
{
public:
    void Start() override
    {
        Debug::Print("Start called!");
    }
}
```

---
### Update
This function called every frame.

The parent GameObject and the component should be enabled.
```cpp
virtual void Update()
```
Code sample:
```cpp
class API MyComponent : public Component
{
public:
    void Update() override
    {
        Debug::Print("Update called!");
    }
}
```

---
### OnComponentAttached
This function called when a component is attached to a component.<br>
It's usefull when you need to setup the component before the Awake being called with an access to the parent GameObject.<br>
This function is called even if the GameObject or the component are disabled compared to the Awake method.<br>
In the constructor of your component if the component you can't get a reference to the parent GameObject.
```cpp
virtual void OnComponentAttached()
```
Code sample:
```cpp
class API MyComponent : public Component
{
public:
    void OnTransformPositionUpdated();

    void OnComponentAttached() override
    {
        GetTransformRaw()->GetOnTransformUpdated().Bind(&MyComponent::OnTransformPositionUpdated, this);
    }
}
```

---
### OnDisabled
This function called when a component is disabled.
```cpp
virtual void OnDisabled()
```
Code sample:
```cpp
class API MyComponent : public Component
{
public:
    void OnDisabled() override
    {
        Debug::Print("Component disabled!");
    }
}
```

---
### OnEnabled
This function called when a component is enabled.
```cpp
virtual void OnEnabled()
```
Code sample:
```cpp
class API MyComponent : public Component
{
public:
    void OnEnabled() override
    {
        Debug::Print("Component enabled!");
    }
}
```

---
### OnDrawGizmos
Called each frame to draw gizmos.

The parent GameObject and the component should be enabled.
```cpp
virtual void OnDrawGizmos()
```
Code sample:
```cpp
class API MyComponent : public Component
{
public:
    void OnDrawGizmos() override
    {
#if defined(EDITOR)
        Gizmo::DrawLine(Vector3(0, 0, 0), Vector3(1, 1, 1));
#endif
    }
}
```

---
### OnDrawGizmosSelected
Called each frame to draw gizmos if the object is selected.

The parent GameObject and the component should be enabled.
```cpp
virtual void OnDrawGizmosSelected()
```
Code sample:
```cpp
class API MyComponent : public Component
{
public:
    void OnDrawGizmosSelected() override
    {
#if defined(EDITOR)
        Gizmo::DrawLine(Vector3(0, 0, 0), Vector3(1, 1, 1));
#endif
    }
}
```

---
### ToString
Return a string representation of the component.
```cpp
virtual std::string ToString()
```
Code sample:
```cpp
class API MyComponent : public Component
{
public:
    std::string ToString() override
    {
        return "{" + *m_componentName + ", Value: " + std::to_string(value) + "}";
    }

private:
    int value = 2;
}
```

---
### OnCollisionEnter
Function called when a collision has just occured with another collider.

Parameters:
- `info`: information about the collision
```cpp
virtual void OnCollisionEnter(CollisionEvent info)
```
Code sample:
```cpp
class API MyComponent : public Component
{
public:
    void OnCollisionEnter(CollisionEvent info) override
    {
        Debug::Print("I just hit something!");
    }
}
```

---
### OnCollisionStay
Function called every frame if a collider of the GameObject is still colliding with another collider.

Parameters:
- `info`: information about the collision
```cpp
virtual void OnCollisionStay(CollisionEvent info)
```
Code sample:
```cpp
class API MyComponent : public Component
{
public:
    void OnCollisionStay(CollisionEvent info) override
    {
        Debug::Print("I'm still colliding with something!");
    }
}
```

---
### OnCollisionExit
Function called when the collision has ended.

Parameters:
- `info`: information about the collision
```cpp
virtual void OnCollisionExit(CollisionEvent info)
```
Code sample:
```cpp
class API MyComponent : public Component
{
public:
    void OnCollisionExit(CollisionEvent info) override
    {
        Debug::Print("I'm not colliding with something anymore!");
    }
}
```

---
### OnTriggerEnter
Function called when a collider of the GameObject just entered a trigger collider.

Parameters:
- `info`: information about the collision
```cpp
virtual void OnTriggerEnter(CollisionEvent info)
```
Code sample:
```cpp
class API MyComponent : public Component
{
public:
    void OnTriggerEnter(CollisionEvent info) override
    {
        Debug::Print("I just entered in a trigger!");
    }
}
```

---
### OnTriggerStay
Function called when a collider of the GameObject is still in a trigger collider.

Parameters:
- `info`: information about the collision
```cpp
virtual void OnTriggerStay(CollisionEvent info)
```
Code sample:
```cpp
class API MyComponent : public Component
{
public:
    void OnTriggerStay(CollisionEvent info) override
    {
        Debug::Print("I'm still in a trigger!");
    }
}
```

---
### OnTriggerExit
Function called when a collider of the GameObject just leave a trigger collider.

Parameters:
- `info`: information about the collision
```cpp
virtual void OnTriggerExit(CollisionEvent info)
```
Code sample:
```cpp
class API MyComponent : public Component
{
public:
    void OnTriggerExit(CollisionEvent info) override
    {
        Debug::Print("I'm not in a trigger anymore!");
    }
}
```

## Public methods

---
### IsEnabled
Get if the component is enabled.
```cpp
bool IsEnabled() const
```
Code sample:
```cpp
std::shared_ptr<Component> component; // Filled variable
component->SetIsEnabled(false);
bool isEnabled = component->IsEnabled(); // = false
```

---
### SetIsEnabled
Enable or disable the component.

Parameters:
- `isEnabled`: true to enable, false to disable
```cpp
void SetIsEnabled(bool isEnabled)
```
Code sample:
```cpp
std::shared_ptr<Component> component; // Filled variable
component->SetIsEnabled(false);
bool isEnabled = component->IsLocalActive(); // = false
```

---
### GetGameObject
Get component's GameObject.
```cpp
std::shared_ptr <GameObject> GetGameObject() const
```
Code sample:
```cpp
std::shared_ptr<Component> component; // Filled variable
std::shared_ptr<GameObject> gameObject = component->GetGameObject();
```

---
### GetGameObjectRaw
Get component's GameObject raw pointer for faster access (Not safe, use with caution).
```cpp
GameObject* GetGameObjectRaw() const
```
Code sample:
```cpp
std::shared_ptr<Component> component; // Filled variable
GameObject* gameObject = component->GetGameObjectRaw();
```

---
### GetTransform
Get component's Transform.
```cpp
std::shared_ptr<Transform> GetTransform() const
```
Code sample:
```cpp
std::shared_ptr<Component> component; // Filled variable
std::shared_ptr<Transform> transform = component->GetTransform();
```

---
### GetTransformRaw
Get component's Transform raw pointer for faster access (Not safe, use with caution).
```cpp
Transform* GetTransformRaw() const
```
Code sample:
```cpp
std::shared_ptr<Component> component; // Filled variable
Transform* transform = component->GetTransformRaw();
```

---
### GetComponentName
Get component's name.
```cpp
const std::string& GetComponentName() const
```
Code sample:
```cpp
std::shared_ptr<Component> component; // Filled variable
const std::string& name = component->GetComponentName();
```