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
    Debug::Print("-------- Mesh Manager initiated --------");
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
void MeshManager::DrawMesh(Vector3 position, Vector3 rotation, Vector3 scale, Texture *texture, MeshData *meshData, bool useDepth, bool useBlend, bool useLighting)
{
    if ((meshData->hasIndices && meshData->index_count == 0) || (!meshData->hasIndices && meshData->vertice_count == 0))
        return;

    // meshBenchmark->Start();
    Graphics::usedCamera->UpdateProjection();
    Engine::renderer->SetCameraPosition(Graphics::usedCamera);

    Engine::renderer->SetTransform(position, rotation, scale, true);

    // Set draw settings
    RenderingSettings renderSettings = RenderingSettings();
    if (scale.x * scale.y * scale.z < 0)
        renderSettings.invertFaces = true;
    else
        renderSettings.invertFaces = false;

    renderSettings.useBlend = useBlend;
    renderSettings.useDepth = useDepth;
    renderSettings.useTexture = true;
    renderSettings.useLighting = useLighting;

    Engine::renderer->BindTexture(texture);
    Engine::renderer->DrawMeshData(meshData, renderSettings);

    // meshBenchmark->Stop();
}