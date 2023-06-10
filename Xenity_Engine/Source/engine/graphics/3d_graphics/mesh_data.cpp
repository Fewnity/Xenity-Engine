#include "mesh_data.h"
#include <malloc.h>
#include "../color/color.h"
#include "../../debug/debug.h"

#ifdef __PSP__
#include <pspkernel.h>
#include <vram.h>
#endif

/**
 * @brief Constructor
 *
 * @param vcount
 * @param index_count
 */
MeshData::MeshData(unsigned int vcount, unsigned int index_count, bool useVertexColor)
{
    // useVertexColor = true;
    // Allocate memory for mesh data
    hasColor = useVertexColor;

    if (useVertexColor)
        data = (Vertex *)memalign(16, sizeof(Vertex) * vcount);
    else
        data = (VertexNoColor *)memalign(16, sizeof(VertexNoColor) * vcount);

    // data = (Vertex *)malloc(sizeof(Vertex) * vcount);
    if (data == nullptr)
    {
        Debug::Print("No memory for Vertex");
        return;
    }

    // indices = (unsigned int *)malloc(sizeof(unsigned int) * index_count);
    // indices = (unsigned int *)memalign(16, sizeof(unsigned int) * index_count);
    indices = (unsigned short *)memalign(16, sizeof(unsigned short) * index_count);

    if (indices == nullptr)
    {
        Debug::Print("No memory for Indices");
        return;
    }

    this->index_count = index_count;
    this->vertice_count = vcount;
}

// MeshData::MeshData(std::string filePath)
// {
// }

/**
 * @brief Add a vertice to the mesh
 *
 * @param u Texture coords U
 * @param v Texture coords V
 * @param color Vertice
 * @param x X position
 * @param y Y position
 * @param z Z position
 * @param index Vertex index
 */
// void MeshData::AddVertex(float u, float v, unsigned int color, float x, float y, float z, int index)
void MeshData::AddVertex(float u, float v, Color color, float x, float y, float z, int index)
{
    RGBA rgba = color.GetRGBA();

    Vertex vert = {
        .u = u,
        .v = v,
#ifdef __PSP__
        .color = color.GetUnsignedIntABGR(),
#else
        .r = rgba.r,
        .g = rgba.g,
        .b = rgba.b,
        .a = rgba.a,
#endif
        .x = x,
        .y = y,
        .z = z};

    ((Vertex *)data)[index] = vert;
}

void MeshData::AddVertex(float u, float v, float x, float y, float z, int index)
{
    VertexNoColor vert = {
        .u = u,
        .v = v,
        .x = x,
        .y = y,
        .z = z};

    ((VertexNoColor *)data)[index] = vert;
}

/**
 * @brief Destructor
 *
 */
MeshData::~MeshData()
{
    if (data)
        free(data);
    if (indices)
        free(indices);
}