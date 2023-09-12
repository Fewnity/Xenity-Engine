#include "mesh_manager.h"
#include "mesh_data.h"
#include "../../../xenity.h"
#include "../renderer/renderer.h"
#include "../../file_system/mesh_loader/wavefront_loader.h"

#if defined(__PSP__)
#include "../../../psp/gu2gl.h"
#elif defined(__vita__)
#include <vitaGL.h>
#endif

ProfilerBenchmark *meshBenchmark = nullptr;
ProfilerBenchmark* meshCameraBenchmark = nullptr;
ProfilerBenchmark* meshTransformBenchmark = nullptr;
ProfilerBenchmark* meshDrawBenchmark = nullptr;

void MeshManager::Init()
{
    meshBenchmark = new ProfilerBenchmark("Mesh", "Mesh");
    meshCameraBenchmark = new ProfilerBenchmark("Mesh", "Camera update");
    meshTransformBenchmark = new ProfilerBenchmark("Mesh", "Transform update");
    meshDrawBenchmark = new ProfilerBenchmark("Mesh", "Draw");
    Debug::Print("-------- Mesh Manager initiated --------");
}

std::shared_ptr <MeshData> MeshManager::LoadMesh(std::string path)
{
    std::shared_ptr <MeshData> mesh = MeshData::MakeMeshData();
    mesh->file = new File(path);
    mesh->fileType = File_Mesh;
    WavefrontLoader::LoadFromRawData(mesh);
    return mesh;
}

void MeshManager::DrawMesh(Vector3 position, Vector3 rotation, Vector3 scale, std::vector< std::shared_ptr<Texture>> textures, std::shared_ptr <MeshData> meshData, bool useDepth, bool useBlend, bool useLighting)
{
    //if (!meshData || (meshData->hasIndices && meshData->subMeshes[0]->index_count == 0) || (!meshData->hasIndices && meshData->subMeshes[0]->vertice_count == 0))
    if (!meshData)
        return;

    /*if (texture.size() || !texture->IsValid())
    {
        Debug::PrintError("[MeshManager::DrawMesh] Invalid texture");
        return;
    }*/

    auto camera = Graphics::usedCamera.lock();
    if (!camera)
        return;

    meshBenchmark->Start();
    meshCameraBenchmark->Start();
#if defined(__PSP__)
    if (Graphics::needUpdateCamera)
    {
        camera->UpdateProjection();
        Engine::renderer->SetCameraPosition(Graphics::usedCamera);
        Graphics::needUpdateCamera = false;
    }
#else
    Engine::renderer->SetCameraPosition(Graphics::usedCamera);
#endif
    meshCameraBenchmark->Stop();

    meshTransformBenchmark->Start();
    Engine::renderer->SetTransform(position, rotation, scale, true);
    meshTransformBenchmark->Stop();

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

    meshDrawBenchmark->Start();
    Engine::renderer->DrawMeshData(meshData, textures, renderSettings);
    meshDrawBenchmark->Stop();

    meshBenchmark->Stop();
}

void MeshManager::DrawMesh(Vector3 position, Vector3 rotation, Vector3 scale, std::shared_ptr < Texture> texture, std::shared_ptr < MeshData> meshData, bool useDepth, bool useBlend, bool useLighting)
{
    std::vector< std::shared_ptr<Texture>> textures;
    textures.push_back(texture);
    DrawMesh(position, rotation, scale, textures, meshData, useDepth, useBlend, useLighting);
}
