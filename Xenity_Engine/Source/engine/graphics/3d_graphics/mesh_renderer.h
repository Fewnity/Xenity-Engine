#pragma once

#include "../iDrawable.h"

class MeshData;
class Texture;

class MeshRenderer : public IDrawable
{
public:
    MeshRenderer();
    ~MeshRenderer();
    int GetDrawPriority() const;

    MeshData *meshData = nullptr;
    Texture *texture = nullptr;

private:
    void Draw();
};