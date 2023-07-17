#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include "../iDrawable.h"

class MeshData;
class Texture;

class API MeshRenderer : public IDrawable
{
public:
    MeshRenderer();
    ~MeshRenderer();
    void SetReflection();
    int GetDrawPriority() const;

    MeshData *meshData = nullptr;
    Texture *texture = nullptr;

private:
    void Draw();
};