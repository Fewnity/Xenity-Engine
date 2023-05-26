#pragma once

class Vector3;
class Texture;

struct Vertex
{
    float u, v;
    unsigned int color;
    float x, y, z;
};

class MeshData
{
public:
    MeshData() = delete;
    MeshData(unsigned int vcount, unsigned int index_count);
    ~MeshData();

    void AddVertice(float u, float v, unsigned int color, float x, float y, float z, int indice);

    Vertex *data = nullptr;

    unsigned int *indices = nullptr;
    unsigned int index_count = 0;
};

void StartDraw();
void EndDraw();
void DrawSprite(Vector3 position, Vector3 rotation, Vector3 scale, Texture *texture);
void InitSpriteManager();