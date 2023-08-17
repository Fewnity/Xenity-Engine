#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include "../iDrawable.h"
#include <vector>

class MeshData;
class Texture;

class API MeshRenderer : public IDrawable
{
public:
    MeshRenderer();
    ~MeshRenderer();
    //void SetReflection();
    std::unordered_map<std::string, Variable> GetReflection();
    int GetDrawPriority() const;

    MeshData *meshData = nullptr;
    Texture *texture = nullptr;
    std::vector<Texture*> textures;

private:
    void Draw();
};