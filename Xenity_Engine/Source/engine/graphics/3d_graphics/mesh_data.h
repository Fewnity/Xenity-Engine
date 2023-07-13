#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <string>

#include "../color/color.h"

struct Vertex
{
    float u, v;
#ifdef __PSP__
    unsigned int color;
#else
    float r, g, b, a;
#endif
    float x, y, z;
};

struct VertexNoColor
{
    float u, v;
    float x, y, z;
};

struct VertexNormalsNoColor
{
    float u, v;
    float normX, normY, normZ;
    float x, y, z;
};

class API MeshData
{
public:
    MeshData() = delete;
    MeshData(unsigned int vcount, unsigned int index_count, bool useVertexColor, bool useNormals);

    ~MeshData();


    void AddVertex(float u, float v, Color color, float x, float y, float z, int index);
    void AddVertex(float u, float v, float x, float y, float z, int index);
    void AddVertex(float u, float v, float nx, float ny, float nz, float x, float y, float z, int index);

    void *data = nullptr;

    unsigned short *indices = nullptr;

    unsigned int vertice_count = 0;
    unsigned int index_count = 0;
    bool hasUv = false;
    bool hasNormal = false;
    bool hasColor = true;
    bool hasIndices = true;
    bool isQuad = false;
    Color unifiedColor = Color::CreateFromRGBA(255, 255, 255, 255);
};