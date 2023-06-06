#include "mesh_manager.h"
#include "mesh_data.h"
#include "../../../xenity.h"
#include "../renderer/renderer.h"

#ifdef __PSP__
#include "../../../psp/gu2gl.h"
#endif

#ifdef __vita__
#include <vitaGL.h>
#endif

ProfilerBenchmark *meshBenchmark = nullptr;

void MeshManager::Init()
{
    meshBenchmark = new ProfilerBenchmark("Mesh");
}

void MeshManager::DrawMesh(Vector3 position, Vector3 rotation, Vector3 scale, Texture *texture, MeshData *meshData, bool useDepth)
{
    meshBenchmark->Start();
    Graphics::usedCamera->UpdateProjection();
    Engine::renderer->SetCameraPosition(Graphics::usedCamera);

    Vector3 pos = Vector3(-position.x, position.y, position.z);
    Vector3 rot = Vector3(rotation.x, rotation.y, rotation.z);
    Vector3 scl = Vector3(scale.x, scale.y, scale.z);
    Engine::renderer->SetTransform(pos, rot, scl, true);

    // Set settings
    if (useDepth)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    Engine::renderer->BindTexture(texture);
    MeshManager::DrawMeshData(meshData);

    meshBenchmark->Stop();
}

void MeshManager::DrawMeshData(MeshData *meshData)
{
#ifdef __PSP__
    // glDrawElements(GL_TRIANGLES, GL_INDEX_16BIT | GL_TEXTURE_32BITF | GL_COLOR_8888 | GL_VERTEX_32BITF | GL_TRANSFORM_3D, 6, meshData->indices, meshData->data);
    glDrawElements(GL_TRIANGLES, GL_INDEX_BITS | GL_TEXTURE_32BITF | GL_COLOR_8888 | GL_VERTEX_32BITF | GL_TRANSFORM_3D, meshData->index_count, meshData->indices, meshData->data);
#endif

#ifdef __vita__
    glEnableClientState(GL_VERTEX_ARRAY);
    // glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    int stride = sizeof(Vertex);
    glTexCoordPointer(2, GL_FLOAT, stride, &meshData->data[0].u);
    // glColorPointer(1, GL_UNSIGNED_INT, stride, &vData->data[0].color);
    glVertexPointer(3, GL_FLOAT, stride, &meshData->data[0].x);

    glDrawElements(GL_TRIANGLES, meshData->index_count, GL_UNSIGNED_INT, meshData->indices);
    glDisableClientState(GL_VERTEX_ARRAY);
    // glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
}