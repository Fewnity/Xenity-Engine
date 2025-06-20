# Enumerations

Add this in your code:
```cpp
#include <engine/reflection/enum_utils.h>
```

If you want to create an enum visible by the editor inspector, you have to use Xenity's custom enums.

```cpp
ENUM(MyEnum, Value0, Value1, Value2);
```

You can assigan values to each elements:

```cpp
ENUM(MyEnum, Value0 = 1, Value1 = 3, Value2 = 8);
```

The type of value of an enum element is `int`.

You are allowed to create a classic C++ enum class, but you won't be able to use it in the inspector.