#include "mesh_data.h"
#include <malloc.h>

#ifdef __PSP__
#include <pspkernel.h>
#endif

MeshData::MeshData(unsigned int vcount, unsigned int index_count)
{
    data = (Vertex *)memalign(16, sizeof(Vertex) * vcount);
    // data = (Vertex*)malloc(sizeof(Vertex) * vcount);
    if (data == nullptr)
    {
        return;
    }

    // indices = (unsigned int *)malloc(sizeof(unsigned int) * index_count);
    indices = (unsigned int *)memalign(16, sizeof(unsigned int) * index_count);
    // indices = (unsigned short *)malloc(sizeof(unsigned short) * index_count);

    // indices = (unsigned short *)memalign(16, sizeof(unsigned short) * index_count);
    //  mesh->indices = memalign(16, sizeof(u16) * index_count);
    if (indices == nullptr)
    {
        return;
    }

    this->index_count = index_count;
}

void MeshData::AddVertice(float u, float v, unsigned int color, float x, float y, float z, int indice)
{
    Vertex vert = {
        .u = u,
        .v = v,
        .color = color,
        .x = x,
        .y = y,
        .z = z};

    data[indice] = vert;
}

MeshData::~MeshData()
{
    if (data)
        free(data);
    if (indices)
        free(indices);
}