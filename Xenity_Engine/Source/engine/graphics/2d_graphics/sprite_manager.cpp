#include "sprite_manager.h"
#include "../../../xenity.h"
#include "../3d_graphics/mesh_data.h"
#include <cstdlib>
#include <malloc.h>
#include "../renderer/renderer.h"

#include "../../../../include/stb_image_resize.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifdef __PSP__
#include <pspkernel.h>
#include <psppower.h>
#include "../../../psp/gu2gl.h"
#endif

#ifdef __vita__
#include <vitaGL.h>
#endif

std::shared_ptr <MeshData> SpriteManager::spriteMeshData = nullptr;

ProfilerBenchmark *spriteBenchmark = nullptr;

/**
 * @brief Init the Sprite Manager
 *
 */
void SpriteManager::Init()
{
    spriteBenchmark = new ProfilerBenchmark("Sprite", "Sprite");

    // Create sprite mesh
    spriteMeshData = MeshData::MakeMeshData(4, 6, false, false, true);
    spriteMeshData->AddVertex(1.0f, 1.0f, -0.5f, -0.5f, 0.0f, 0,0);
    spriteMeshData->AddVertex(0.0f, 1.0f, 0.5f, -0.5f, 0.0f, 1,0);
    spriteMeshData->AddVertex(0.0f, 0.0f, 0.5f, 0.5f, 0.0f, 2,0);
    spriteMeshData->AddVertex(1.0f, 0.0f, -0.5f, 0.5f, 0.0f, 3,0);
    spriteMeshData->subMeshes[0]->indices[0] = 0;
    spriteMeshData->subMeshes[0]->indices[1] = 2;
    spriteMeshData->subMeshes[0]->indices[2] = 1;
    spriteMeshData->subMeshes[0]->indices[3] = 2;
    spriteMeshData->subMeshes[0]->indices[4] = 0;
    spriteMeshData->subMeshes[0]->indices[5] = 3;

#if defined(__PSP__)
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
void SpriteManager::DrawSprite(Vector3 position, Vector3 rotation, Vector3 scale, std::shared_ptr < Texture> texture, Color color, std::weak_ptr<Transform> transform)
{
    if (!texture || !texture->IsValid())
    {
        Debug::PrintError("[SpriteManager::DrawSprite] Invalid texture");
        return;
    }

    spriteBenchmark->Start();
    if (auto camera = Graphics::usedCamera.lock())
    {
        // texture = TextManager::fonts[0]->fontAtlas;

        spriteMeshData->unifiedColor = color;

        // Get sprite scale from texture size
        float scaleCoef = (1.0f / texture->GetPixelPerUnit());
        float w = texture->GetWidth() * scaleCoef;
        float h = texture->GetHeight() * scaleCoef;

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

// Move/Rotate/Scale the sprite
#if defined(__PSP__)
        glm::mat4 mat = transform.lock()->transformationMatrix;
        mat = glm::scale(mat, glm::vec3(w, h, 1));
        Engine::renderer->SetTransform(mat);
#else
        Vector3 scl = Vector3(scale.x * w, scale.y * h, 1);
        Engine::renderer->SetTransform(position, rotation, scl, true);
#endif

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
        //Engine::renderer->BindTexture(texture);
        std::vector<std::shared_ptr<Texture>> textures;
        textures.push_back(texture);
        Engine::renderer->DrawMeshData(spriteMeshData, textures,  renderSettings);
    }
    spriteBenchmark->Stop();
}

void SpriteManager::Render2DLine(std::shared_ptr <MeshData> meshData)
{
    if (!AssetManager::defaultTexture || !AssetManager::defaultTexture->IsValid())
    {
        Debug::PrintError("[SpriteManager::Render2DLine] Invalid texture");
        return;
}

    spriteBenchmark->Start();
    if (auto camera = Graphics::usedCamera.lock())
    {
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

        Vector3 zero = Vector3(0);
        Vector3 one = Vector3(1);

        Engine::renderer->SetTransform(zero, zero, one, true);

        // Set draw settings
        RenderingSettings renderSettings = RenderingSettings();

        renderSettings.invertFaces = true;
        renderSettings.useBlend = true;
        renderSettings.useDepth = false;
        renderSettings.useTexture = true;
        renderSettings.useLighting = false;

        std::vector<std::shared_ptr<Texture>> textures;
        textures.push_back(AssetManager::defaultTexture);
        Engine::renderer->DrawMeshData(meshData, textures, renderSettings);
    }
    spriteBenchmark->Stop();
}