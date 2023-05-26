#include "sprite_manager.h"
#include "../texture.h"
#include "../../vectors/vector3.h"
#include <cstdlib>
#include <malloc.h>
#include "../camera.h"

#ifdef __PSP__
#include <pspkernel.h>
#include "../../../psp/gu2gl.h"
#include "../../../psp/graphics/graphics.h"
#endif

#ifdef __vita__
#include <vitaGL.h>
#include "../../../psvita/graphics/graphics.h"
#endif

MeshData::MeshData(unsigned int vcount, unsigned int index_count)
{
    // data = (Vertex *)memalign(16, sizeof(Vertex) * vcount);
    data = (Vertex *)malloc(sizeof(Vertex) * vcount);
    if (data == nullptr)
    {
        return;
    }

    indices = (unsigned int *)malloc(sizeof(unsigned int) * index_count);
    // indices = (unsigned short *)malloc(sizeof(unsigned short) * index_count);

    // indices = (unsigned int *)memalign(16, sizeof(unsigned int) * index_count);
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

void DrawMeshData(MeshData *meshData)
{
#ifdef __PSP__
    // glDrawElements(GL_TRIANGLES, GL_INDEX_16BIT | GL_TEXTURE_32BITF | GL_COLOR_8888 | GL_VERTEX_32BITF | GL_TRANSFORM_3D, 6, meshData->indices, meshData->data);
    glDrawElements(GL_TRIANGLES, GL_INDEX_BITS | GL_TEXTURE_32BITF | GL_COLOR_8888 | GL_VERTEX_32BITF | GL_TRANSFORM_3D, 6, meshData->indices, meshData->data);
#endif

#ifdef __vita__
    glEnableClientState(GL_VERTEX_ARRAY);
    // glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    int stride = sizeof(Vertex);
    glTexCoordPointer(2, GL_FLOAT, stride, &meshData->data[0].u);
    // glColorPointer(1, GL_UNSIGNED_INT, stride, &vData->data[0].color);
    glVertexPointer(3, GL_FLOAT, stride, &meshData->data[0].x);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, meshData->indices);
    glDisableClientState(GL_VERTEX_ARRAY);
    // glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
}

void StartDraw()
{
#ifdef __PSP__
    PspStartFrame();
#endif
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT /*| GL_STENCIL_BUFFER_BIT*/);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void EndDraw()
{
    CrossGraphicsSwapBuffer();
}

void ResetTransform()
{
    glLoadIdentity();
}

void SetPosition(float x, float y, float z)
{
    glTranslatef(x, y, z);
}

void SetRotation(float x, float y, float z)
{
#ifdef __PSP__
    glRotatefXYZ(x * 3.14159265359 / 180.0f, y * 3.14159265359 / 180.0f, z * 3.14159265359 / 180.0f);
#elif __vita__
    glRotatef(z, 0, 0, 1);
    glRotatef(y, 0, 1, 0);
    glRotatef(x, 1, 0, 0);
#endif
}

void SetScale(float x, float y, float z)
{
    glScalef(x, y, z);
}

Camera *camera;

void InitSpriteManager()
{
    camera = new Camera();
    camera->x = 4;
    camera->y = 0;
    camera->z = -10;
    camera->yaw = 10;
    camera->pitch = 0;
}

void DrawSprite(Vector3 position, Vector3 rotation, Vector3 scale, Texture *texture)
{
    MeshData *vData = new MeshData(4, 6);
    vData->AddVertice(0.0f, 0.0f, 0xFFFFFFFF, -0.5f, -0.5f, 0.0f, 0);
    vData->AddVertice(1.0f, 0.0f, 0xFFFFFFFF, 0.5f, -0.5f, 0.0f, 1);
    vData->AddVertice(1.0f, 1.0f, 0xFFFFFFFF, 0.5f, 0.5f, 0.0f, 2);
    vData->AddVertice(0.0f, 1.0f, 0xFFFFFFFF, -0.5f, 0.5f, 0.0f, 3);
    vData->indices[0] = 0;
    vData->indices[1] = 2;
    vData->indices[2] = 1;
    vData->indices[3] = 2;
    vData->indices[4] = 0;
    vData->indices[5] = 3;
#ifdef __PSP__
    sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif
    float positionCoef = 0.1f;
    // Set settings
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);

    camera->Apply();

    // float scaleCoef = 100.0f / texture->GetPixelPerUnit() / 1000.0f;
    float scaleCoef = (100.0f / 100.0f) / 1000.0f;
    float w = 256 * scaleCoef;
    float h = 256 * scaleCoef;

    ResetTransform();
    SetPosition(-position.x, position.y, position.z);
    SetRotation(rotation.x, rotation.y + 180, rotation.z);
    // SetScale(scale.x * w, scale.y * h, 1);
    // SetScale(scale.x / w, scale.y / h, 1);
    SetScale(scale.x / (w * 1.5), scale.y / (h * 1.5), 1);

    texture->Bind();
    DrawMeshData(vData);

    delete (vData);
}