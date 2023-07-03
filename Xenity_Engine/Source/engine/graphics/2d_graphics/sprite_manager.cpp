#include "sprite_manager.h"
#include "../../../xenity.h"
#include "../3d_graphics/mesh_data.h"
#include <cstdlib>
#include <malloc.h>
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

MeshData *SpriteManager::spriteMeshData = nullptr;

ProfilerBenchmark *spriteBenchmark = nullptr;

/**
 * @brief Init the Sprite Manager
 *
 */
void SpriteManager::Init()
{
    spriteBenchmark = new ProfilerBenchmark("Sprite");

    // Create sprite mesh
    spriteMeshData = new MeshData(4, 6, false, false);
    spriteMeshData->AddVertex(1.0f, 1.0f, -0.5f, -0.5f, 0.0f, 0);
    spriteMeshData->AddVertex(0.0f, 1.0f, 0.5f, -0.5f, 0.0f, 1);
    spriteMeshData->AddVertex(0.0f, 0.0f, 0.5f, 0.5f, 0.0f, 2);
    spriteMeshData->AddVertex(1.0f, 0.0f, -0.5f, 0.5f, 0.0f, 3);
    spriteMeshData->indices[0] = 0;
    spriteMeshData->indices[1] = 2;
    spriteMeshData->indices[2] = 1;
    spriteMeshData->indices[3] = 2;
    spriteMeshData->indices[4] = 0;
    spriteMeshData->indices[5] = 3;

#ifdef __PSP__
    sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif

    Debug::Print("-------- Sprite Manager initiated --------");
}

/**
 * @brief Draw a sprite
 *
 * @param position Sprite position (center)
 * @param rotation Sprite rotation
 * @param scale Sprite scale
 * @param texture Texture
 */
void SpriteManager::DrawSprite(Vector3 position, Vector3 rotation, Vector3 scale, Texture *texture, Color color)
{
    spriteBenchmark->Start();
    if (auto camera = Graphics::usedCamera.lock())
    {
        // texture = TextManager::fonts[0]->fontAtlas;

        spriteMeshData->unifiedColor = color;

        // Get sprite scale from texture size
        float scaleCoef = (1.0f / texture->GetPixelPerUnit());
        float w = texture->GetWidth() * scaleCoef;
        float h = texture->GetHeight() * scaleCoef;

        camera->UpdateProjection();
        Engine::renderer->SetCameraPosition(Graphics::usedCamera);

        // Move/Rotate/Scale the sprite
        Vector3 scl = Vector3(scale.x * w, scale.y * h, 1);
        Engine::renderer->SetTransform(position, rotation, scl, true);

        // Set draw settings
        RenderingSettings renderSettings = RenderingSettings();

        if (scale.x * scale.y < 0)
            renderSettings.invertFaces = true;
        else
            renderSettings.invertFaces = false;

        renderSettings.useBlend = true;
        renderSettings.useDepth = false;
        renderSettings.useTexture = true;
        renderSettings.useLighting = false;

        // Draw the sprite
        Engine::renderer->BindTexture(texture);
        Engine::renderer->DrawMeshData(spriteMeshData, renderSettings);
    }
    spriteBenchmark->Stop();
}