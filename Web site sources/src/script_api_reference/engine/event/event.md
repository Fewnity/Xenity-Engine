# Event

Add this in your code:
```cpp
#include <engine/event_system/event_system.h>
```

## Description

This class gives the possibility to bind a function to an event.<br>
When a event object is destroyed, all bound functions are unbound.

This class is using templates.

```cpp
template<typename... Args>
class Event
```

Event without argument:
```cpp
Event<> myEvent;
```

Event with arguments:
```cpp
Event<int, const std::string&> myEvent;
```

## Public methods

---
### Bind
Bind a simple function

Parameters:
- `function`:  Pointer to the function to bind
```cpp
void Bind(void(*function)(Args...))
```
Code sample:
```cpp
void MyFunction(int a, const std::string& b) { }

class MyClass
{
public:
    static void MyStaticFunction(int a, const std::string& b) { }
};

Event<int, const std::string&> myEvent;
myEvent.Bind(&MyFunction);
myEvent.Bind(&MyClass::MyStaticFunction);
```

---
### Bind
Bind a function linked to an object

Parameters:
- `function`:  Pointer to the function to bind
- `obj`:  Pointer to the object
```cpp
template<typename ObjType>
void Bind(void(ObjType::* function)(Args...), ObjType* obj)
```
Code sample:
```cpp
class MyClass
{
public:
    void MyFunction(int a, const std::string& b) { }
};

MyClass myClassInstance;

Event<int, const std::string&> myEvent;
myEvent.Bind(&MyClass::MyFunction, &myClassInstance);
```

---
### Unbind
Unbind a simple function

Parameters:
- `function`:  Pointer to the function to unbind
```cpp
void Unbind(void(*function)(Args...))
```
Code sample:
```cpp
void MyFunction(int a, const std::string& b) { }

class MyClass
{
public:
    static void MyStaticFunction(int a, const std::string& b) { }
};

Event<int, const std::string&> myEvent;
myEvent.Bind(&MyFunction);
myEvent.Bind(&MyClass::MyStaticFunction);
//...
myEvent.Unbind(&MyFunction);
myEvent.Unbind(&MyClass::MyStaticFunction);
```

---
### Unbind
Unbind a function linked to an object

Parameters:
- `function`:  Pointer to the function to unbind
- `obj`:  Pointer to the object
```cpp
template<typename ObjType>
void Unbind(void(ObjType::* function)(Args...), ObjType* obj)
```
Code sample:
```cpp
class MyClass
{
public:
    void MyFunction(int a, const std::string& b) { }
};

MyClass myClassInstance;

Event<int, const std::string&> myEvent;
myEvent.Bind(&MyClass::MyFunction, &myClassInstance);
//...
myEvent.Unbind(&MyClass::MyFunction, &myClassInstance);
```

---
### UnbindAll
Unbind all bound function
```cpp
void UnbindAll()
```
Code sample:
```cpp
void MyFunction(int a, const std::string b) { }

Event<int, const std::string&> myEvent;
myEvent.Bind(&MyFunction);
//...
myEvent.UnbindAll();
```

---
### Trigger
Call all bound functions

Parameters:
- `args`:  All arguments to send
```cpp
void Trigger(Args... args)
```
Code sample:
```cpp
void MyFunction(int a, const std::string b) { }

Event<int, const std::string&> myEvent;
myEvent.Bind(&MyFunction);
myEvent.Trigger(1, "Hello World!");
```

---
### GetBoundFunctionCount
Get the number of listener
```cpp
size_t GetBoundFunctionCount()
```
Code sample:
```cpp
void MyFunction(int a, const std::string b) { }
void MyFunction2(int a, const std::string b) { }

Event<int, const std::string&> myEvent;
myEvent.Bind(&MyFunction);
myEvent.Bind(&MyFunction2);

size_t count = myEvent.GetBoundFunctionCount(); // count = 2
```