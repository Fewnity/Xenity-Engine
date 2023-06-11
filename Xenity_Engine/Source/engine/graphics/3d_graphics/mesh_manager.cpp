#include "mesh_manager.h"
#include "mesh_data.h"
#include "../../../xenity.h"
#include "../renderer/renderer.h"
#include "../../file_system/mesh_loader/wavefront_loader.h"

#ifdef __PSP__
#include "../../../psp/gu2gl.h"
#endif

#ifdef __vita__
#include <vitaGL.h>
#endif

ProfilerBenchmark *meshBenchmark = nullptr;

/**
 * @brief Init Mesh Manager
 *
 */
void MeshManager::Init()
{
    meshBenchmark = new ProfilerBenchmark("Mesh");
}

MeshData *MeshManager::LoadMesh(std::string path)
{
    return WavefrontLoader::LoadFromRawData(path);
}

/**
 * @brief Draw a Mesh
 *
 * @param position
 * @param rotation
 * @param scale
 * @param texture
 * @param meshData
 * @param useDepth
 */
void MeshManager::DrawMesh(Vector3 position, Vector3 rotation, Vector3 scale, Texture *texture, MeshData *meshData, bool useDepth)
{
    if ((meshData->hasIndices && meshData->index_count == 0) || (!meshData->hasIndices && meshData->vertice_count == 0))
        return;

    // meshBenchmark->Start();
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
    // glDisable(GL_TEXTURE_2D);

    Engine::renderer->BindTexture(texture);
    MeshManager::DrawMeshData(meshData);
    // glDisable(GU_TEXTURE_2D);

    // meshBenchmark->Stop();
}

/**
 * @brief Draw Mesh data
 *
 * @param meshData
 */
void MeshManager::DrawMeshData(MeshData *meshData)
{
#ifdef __PSP__
    int params = 0;

    if (meshData->hasIndices)
    {
        params |= GL_INDEX_16BIT;
    }
    params |= GL_TEXTURE_32BITF;
    if (meshData->hasColor)
    {
        params |= GL_COLOR_8888;
    }
    else
    {
        sceGuColor(meshData->unifiedColor.GetUnsignedIntABGR());
    }
    params |= GL_VERTEX_32BITF;
    params |= GL_TRANSFORM_3D;

    if (!meshData->hasIndices)
    {
        glDrawElements(GL_TRIANGLES, params, meshData->vertice_count, 0, meshData->data);
    }
    else
    {
        glDrawElements(GL_TRIANGLES, params, meshData->index_count, meshData->indices, meshData->data);
    }

#endif

#ifdef __vita__
    glEnableClientState(GL_VERTEX_ARRAY);
    if (meshData->hasColor)
    {
        glEnableClientState(GL_COLOR_ARRAY);
    }
    else
    {
        RGBA rgba = meshData->unifiedColor.GetRGBA();
        glColor4f(rgba.r, rgba.g, rgba.b, rgba.a);
    }
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    if (meshData->hasColor)
    {
        int stride = sizeof(Vertex);
        glTexCoordPointer(2, GL_FLOAT, stride, &((Vertex *)meshData->data)[0].u);
        glColorPointer(3, GL_FLOAT, stride, &((Vertex *)meshData->data)[0].r);
        glVertexPointer(3, GL_FLOAT, stride, &((Vertex *)meshData->data)[0].x);
    }
    else
    {
        int stride = sizeof(VertexNoColor);
        glTexCoordPointer(2, GL_FLOAT, stride, &((VertexNoColor *)meshData->data)[0].u);
        glVertexPointer(3, GL_FLOAT, stride, &((VertexNoColor *)meshData->data)[0].x);
    }

    if (!meshData->hasIndices)
    {
        glDrawArrays(GL_TRIANGLES, 0, meshData->vertice_count);
    }
    else
    {
        glDrawElements(GL_TRIANGLES, meshData->index_count, GL_UNSIGNED_SHORT, meshData->indices);
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    if (meshData->hasColor)
    {
        glDisableClientState(GL_COLOR_ARRAY);
    }
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

#endif
    Performance::AddDrawCall();
}