#include "sprite_manager.h"
#include "../../../xenity.h"
#include "../texture.h"
#include "../3d_graphics/mesh_data.h"
#include <cstdlib>
#include <malloc.h>
#include "../camera.h"
#include "../renderer/renderer.h"

#include "../../../../include/stb_image_resize.h"

#ifdef __PSP__
#include <pspkernel.h>
#include <psppower.h>
#include "../../../psp/gu2gl.h"
#endif

#ifdef __vita__
#include <vitaGL.h>
#endif

Camera *camera = nullptr;
GameObject *cameraGo = nullptr;
MeshData *SpriteManager::spriteMeshData = nullptr;

ProfilerBenchmark *spriteBenchmark = nullptr;

void SpriteManager::Init()
{
    // scePowerSetClockFrequency(333, 333, 167);
    spriteBenchmark = new ProfilerBenchmark("Sprite");

    Color color = Color::CreateFromRGBA(255, 255, 255, 255);

    // Create sprite mesh
    spriteMeshData = new MeshData(4, 6, false);
    spriteMeshData->AddVertex(0.0f, 0.0f, -0.5f, -0.5f, 0.0f, 0);
    spriteMeshData->AddVertex(1.0f, 0.0f, 0.5f, -0.5f, 0.0f, 1);
    spriteMeshData->AddVertex(1.0f, 1.0f, 0.5f, 0.5f, 0.0f, 2);
    spriteMeshData->AddVertex(0.0f, 1.0f, -0.5f, 0.5f, 0.0f, 3);
    spriteMeshData->indices[0] = 0;
    spriteMeshData->indices[1] = 2;
    spriteMeshData->indices[2] = 1;
    spriteMeshData->indices[3] = 2;
    spriteMeshData->indices[4] = 0;
    spriteMeshData->indices[5] = 3;

    spriteMeshData->unifiedColor = color;

#ifdef __PSP__
    sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif
    cameraGo = new GameObject("Camera");
    camera = cameraGo->AddComponent<Camera>();
    // camera->SetNearClippingPlane(0.1);
    camera->SetFarClippingPlane(100);
    camera->SetProjectionSize(5.0f);
    camera->SetProjectionType(Orthographic);
}

void SpriteManager::DrawSprite(Vector3 position, Vector3 rotation, Vector3 scale, Texture *texture)
{
    spriteBenchmark->Start();
    // texture = TextManager::fonts[0]->fontAtlas;

    float scaleCoef = (1.0f / texture->GetPixelPerUnit());

    float w = texture->GetWidth() * scaleCoef;
    float h = texture->GetHeight() * scaleCoef;

    Graphics::usedCamera->UpdateProjection();
    Engine::renderer->SetCameraPosition(Graphics::usedCamera);

    Vector3 pos = Vector3(-position.x, position.y, position.z);
    Vector3 rot = Vector3(rotation.x, rotation.y, rotation.z);
    Vector3 scl = Vector3(scale.x * w, scale.y * h, 1);
    Engine::renderer->SetTransform(pos, rot, scl, true);

    // Set settings
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);

    Engine::renderer->BindTexture(texture);
    MeshManager::DrawMeshData(spriteMeshData);

    spriteBenchmark->Stop();
}