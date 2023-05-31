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
#include "../../../psp/gu2gl.h"
#endif

#ifdef __vita__
#include <vitaGL.h>
#endif

Camera *camera = nullptr;
GameObject *cameraGo = nullptr;
MeshData *spriteMeshData = nullptr;

void SpriteManager::Init()
{
    spriteMeshData = new MeshData(4, 6);
    spriteMeshData->AddVertice(0.0f, 0.0f, 0xFFFFFFFF, -0.5f, -0.5f, 0.0f, 0);
    spriteMeshData->AddVertice(1.0f, 0.0f, 0xFFFFFFFF, 0.5f, -0.5f, 0.0f, 1);
    spriteMeshData->AddVertice(1.0f, 1.0f, 0xFFFFFFFF, 0.5f, 0.5f, 0.0f, 2);
    spriteMeshData->AddVertice(0.0f, 1.0f, 0xFFFFFFFF, -0.5f, 0.5f, 0.0f, 3);
    spriteMeshData->indices[0] = 0;
    spriteMeshData->indices[1] = 2;
    spriteMeshData->indices[2] = 1;
    spriteMeshData->indices[3] = 2;
    spriteMeshData->indices[4] = 0;
    spriteMeshData->indices[5] = 3;
#ifdef __PSP__
    sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif
    cameraGo = new GameObject("Camera");
    camera = cameraGo->AddComponent<Camera>();
    // camera->SetProjectionType(Orthographic);
}

void SpriteManager::DrawSprite(Vector3 position, Vector3 rotation, Vector3 scale, Texture *texture)
{
    // texture = TextManager::fonts[0]->Characters[65].texture;
    // float scaleCoef = 100.0f / texture->GetPixelPerUnit() / 100.0f;
    float scaleCoef = (100.0f / 100.0f) / 100.0f;
    // float scaleCoef = (1.0f / 100.0f); //?

    float w = texture->width * scaleCoef;
    float h = texture->height * scaleCoef;

    Engine::renderer->SetCameraPosition(Graphics::usedCamera);

    Vector3 pos = Vector3(-position.x, position.y, position.z);
    Vector3 rot = Vector3(rotation.x, rotation.y + 180, rotation.z);
    Vector3 scl = Vector3(scale.x * w, scale.y * h, 1);
    Engine::renderer->SetTransform(pos, rot, scl, true);

    // Set settings
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);

    texture->Bind();
    MeshManager::DrawMeshData(spriteMeshData);
}