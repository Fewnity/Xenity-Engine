# VertexDescriptor

Add this in your code:
```cpp
#include <engine/graphics/3d_graphics/vertex_descriptor.h>
```

## Description

Class that store the data structure of a vertex.

## Static methods

---
### GetVertexElementSize
Get the size in bytes of a vertex element.

Parameters:
- `vertexElement`: Vertex element to check
```cpp
uint16_t GetVertexElementSize(VertexElement vertexElement)
```
Code sample:
```cpp
uint16_t elementSize = VertexDescriptor::GetVertexElementSize(VertexElement::POSITION_32_BITS);
```

## Public methods

---
### AddVertexElement
Add a vertex element to the descriptor.

Parameters:
- `vertexElement`: Vertex element to add
```cpp
void AddVertexElement(VertexElement vertexElement)
```
Code sample:
```cpp
VertexDescriptor descriptor;
descriptor.AddVertexElement(VertexElement::POSITION_32_BITS);
```

---
### GetElementFromIndex
Add a vertex element to the descriptor.

Parameters:
- `index`: Index of the vertex element to get
```cpp
VertexElement GetElementFromIndex(int32_t index) const
```
Code sample:
```cpp
VertexDescriptor descriptor;
descriptor.AddVertexElement(VertexElement::NORMAL_32_BITS);
descriptor.AddVertexElement(VertexElement::POSITION_32_BITS);

VertexElement normalElement = descriptor.GetElementFromIndex(0);
VertexElement positionElement = descriptor.GetElementFromIndex(1);
```

---
### GetOffsetFromIndex
Get the offset in memory of the vertex element from the index.

Parameters:
- `index`: Index of the vertex element
```cpp
uint32_t GetOffsetFromIndex(int32_t index) const
```

---
### GetPositionIndex
Get the index of the position vertex element.<br>
Return -1 if there is no position vertex element.
```cpp
```cpp
uint32_t GetPositionIndex() const
```

---
### GetNormalIndex
Get the index of the normal vertex element.<br>
Return -1 if there is no normal vertex element.
```cpp
```cpp
uint32_t GetNormalIndex() const
```

---
### GetUvIndex
Get the index of the UV vertex element.<br>
Return -1 if there is no UV vertex element.
```cpp
```cpp
uint32_t GetUvIndex() const
```

---
### GetColorIndex
Get the index of the color vertex element.<br>
Return -1 if there is no color vertex element.
```cpp
uint32_t GetColorIndex() const
```

---
### GetPositionOffset
Get the offset in memory of the position vertex element.<br>
Do not use if there is no position vertex element.
```cpp
uint32_t GetPositionOffset() const
```

---
### GetNormalOffset
Get the offset in memory of the normal vertex element.<br>
Do not use if there is no normal vertex element.
```cpp
uint32_t GetNormalOffset() const
```

---
### GetUvOffset
Get the offset in memory of the UV vertex element.<br>
Do not use if there is no UV vertex element.
```cpp
uint32_t GetUvOffset() const
```

---
### GetColorOffset
Get the offset in memory of the color vertex element.<br>
Do not use if there is no color vertex element.
```cpp
uint32_t GetColorOffset() const
```

---
### GetVertexElementList
Get the list of all vertex elements.
```cpp
const std::vector<VertexElementInfo>& GetVertexElementList() const
```

---
### GetVertexSize
Get the size in byte of a vertex.
```cpp
uint16_t GetVertexSize() const
```