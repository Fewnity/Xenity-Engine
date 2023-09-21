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
#include <memory>
class MeshData;
class Texture;
class Material;

class API MeshRenderer : public IDrawable
{
public:
    MeshRenderer();
    ~MeshRenderer();
    //void SetReflection();
    std::unordered_map<std::string, ReflectionEntry> GetReflection();
    int GetDrawPriority() const;

    std::shared_ptr <MeshData> meshData = nullptr;
    std::vector< std::shared_ptr<Texture>> textures;
    Material* material;
private:
    void Draw();
};