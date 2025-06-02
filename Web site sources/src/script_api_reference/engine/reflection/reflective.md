# Reflective

Add this in your code:
```cpp
#include <engine/reflection/reflection.h>
```

## Description
Class to inherit if you want to access child's variables to fill variables from json or save them to json.<br>
It's used to see variables of a class in the [Inspector](../../../manual/editor_ui/inspector.md).

GameObject, Transform and Component implement this class.

The GetReflectiveData function is very important and is mandatory to implement in every class that implement Reflective.

## Typedef

```cpp
ReflectiveData = std::vector<ReflectiveEntry>
```

## Macros

Macros have been added to simplify the usage of the reflection system.

`BEGIN_REFLECTION();` is used at the beginning of the GetReflectiveData function to prepare the reflection.

`END_REFLECTION();` is used at the end of the GetReflectiveData function to end the reflection.

`ADD_VARIABLE(variable, isPublic);` is used to make a variable visible for the reflection system. `isPublic` is used to hide or show the variable from the Inspector view.

`ADD_VARIABLE_CUSTOM(variable, name, isPublic);` is used to make a variable visible for the reflection system with a custom name. `isPublic` is used to hide or show the variable from the Inspector view.

## Public methods

---
### GetReflectiveData
Get all child class variables references (Needs to be implemented).
```cpp
virtual ReflectiveData GetReflectiveData() = 0
```
Code sample:
```cpp
class MyComponent : public Reflective
{
public:
    ReflectiveData GetReflectiveData() override;

public:
    float publicVariable = 0;
private:
    float m_privateVariable = 1;
};

ReflectiveData MyComponent::GetReflectiveData()
{
	BEGIN_REFLECTION();
	ADD_VARIABLE(publicVariable, true);
     // false is used to hide the variable from the Inspector view
	ADD_VARIABLE(m_privateVariable, false);
	END_REFLECTION();
}
```

---
### OnReflectionUpdated
Called when one or more variables have been updated by the inspector or by the scene manager.
```cpp
virtual void OnReflectionUpdated()
```
Code sample:
```cpp
class MyComponent : public Reflective
{
public:
    ReflectiveData GetReflectiveData() override;
    void OnReflectionUpdated() override;

public:
    float publicVariable = 0;
private:
    float m_privateVariable = 1;
};

ReflectiveData MyComponent::GetReflectiveData()
{
    //...
}

void MyComponent::OnReflectionUpdated()
{
    Debug::Print("One or more variables have been updated!");
}

```