#pragma once

#include <string>

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
    // MeshData(std::string filePath);
    ~MeshData();

    void AddVertice(float u, float v, unsigned int color, float x, float y, float z, int indice);

    Vertex *data = nullptr;
    unsigned int *indices = nullptr;

    // unsigned short *indices = nullptr;
    unsigned int vertice_count = 0;
    unsigned int index_count = 0;
    bool hasUv = false;
    bool hasNormal = false;
    bool isQuad = false;
};