# Component

The component class gives you a list of function that you can override in your components.

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
virtual void Awake()
```
Code sample:
```cpp
class API MyComponent : public Component
{
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
virtual void Awake()
```
Code sample:
```cpp
class API MyComponent : public Component
{
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
virtual void Awake()
```
Code sample:
```cpp
class API MyComponent : public Component
{
    void OnTransformPositionUpdated();

    void OnComponentAttached() override
    {
        GetTransformRaw()->GetOnTransformUpdated().Bind(&MyComponent::OnTransformPositionUpdated, this);
    }
}
```