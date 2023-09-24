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

    MeshData::SubMesh* subMesh = spriteMeshData->subMeshes[0];

    subMesh->indices[0] = 0;
    subMesh->indices[1] = 2;
    subMesh->indices[2] = 1;
    subMesh->indices[3] = 2;
    subMesh->indices[4] = 0;
    subMesh->indices[5] = 3;
    spriteMeshData->SendDataToGpu();

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
void SpriteManager::DrawSprite(std::shared_ptr<Transform> transform, std::shared_ptr <Texture> texture, const Color& color, std::shared_ptr <Material> material)
{
    if (!texture || !texture->IsValid())
    {
        Debug::PrintError("[SpriteManager::DrawSprite] Invalid texture");
        return;
    }

    spriteBenchmark->Start();
    if (auto camera = Graphics::usedCamera.lock())
    {
        Vector3 scale = transform->GetScale();
        if (!Engine::UseOpenGLFixedFunctions)
            material->Use();

        spriteMeshData->unifiedColor = color;

        // Get sprite scale from texture size
        float scaleCoef = (1.0f / texture->GetPixelPerUnit());
        float w = texture->GetWidth() * scaleCoef;
        float h = texture->GetHeight() * scaleCoef;

        glm::mat4 matCopy = transform->transformationMatrix;
        matCopy = glm::scale(matCopy, glm::vec3(w, h, 1));
        if (!Engine::UseOpenGLFixedFunctions)
            Graphics::currentShader->SetShaderModel(matCopy);

#if defined(__PSP__)
        if (Graphics::needUpdateCamera)
        {
            camera->UpdateProjection();
            Engine::renderer->SetCameraPosition(Graphics::usedCamera);
            Graphics::needUpdateCamera = false;
        }
#else
        if(Engine::UseOpenGLFixedFunctions)
            Engine::renderer->SetCameraPosition(Graphics::usedCamera);
#endif

// Move/Rotate/Scale the sprite
#if defined(__PSP__)
        Engine::renderer->SetTransform(matCopy);
#else
        if (Engine::UseOpenGLFixedFunctions) 
        {
#if defined(__PSP__)
            Vector3 position = transform->GetPosition();
            Vector3 rotation = transform->GetRotation();
            Engine::renderer->SetTransform(position, rotation, scale, true);
#else
            Engine::renderer->SetTransform(matCopy);
#endif
        }
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
        std::vector<std::shared_ptr<Texture>> textures;
        textures.push_back(texture);
        Engine::renderer->DrawMeshData(spriteMeshData, textures,  renderSettings);
    }
    spriteBenchmark->Stop();
}

void SpriteManager::DrawSprite(const Vector3& position, const Vector3& rotation, const Vector3& scale, std::shared_ptr <Texture> texture, const Color& color, std::shared_ptr <Material> material)
{
    if (!texture || !texture->IsValid())
    {
        Debug::PrintError("[SpriteManager::DrawSprite] Invalid texture");
        return;
    }


    spriteBenchmark->Start();
    if (auto camera = Graphics::usedCamera.lock())
    {
        if (!Engine::UseOpenGLFixedFunctions)
            material->Use();

        spriteMeshData->unifiedColor = color;

        // Get sprite scale from texture size
        float scaleCoef = (1.0f / texture->GetPixelPerUnit());
        float w = texture->GetWidth() * scaleCoef;
        float h = texture->GetHeight() * scaleCoef;
        Vector3 scaleSprite = scale;
        scaleSprite.x *= w;
        scaleSprite.y *= h;

        if (!Engine::UseOpenGLFixedFunctions)
            Graphics::currentShader->SetShaderModel(Vector3(position.x, position.y, position.z), rotation, scaleSprite);

#if defined(__PSP__)
        if (Graphics::needUpdateCamera)
        {
            camera->UpdateProjection();
            Engine::renderer->SetCameraPosition(Graphics::usedCamera);
            Graphics::needUpdateCamera = false;
        }
#else
        if (Engine::UseOpenGLFixedFunctions)
            Engine::renderer->SetCameraPosition(Graphics::usedCamera);
#endif

        // Move/Rotate/Scale the sprite
        if (Engine::UseOpenGLFixedFunctions) 
        {
            Engine::renderer->SetTransform(position, rotation, scaleSprite, true);
        }

        // Set draw settings
        RenderingSettings renderSettings = RenderingSettings();

        if (scaleSprite.x * scaleSprite.y < 0)
            renderSettings.invertFaces = true;
        else
            renderSettings.invertFaces = false;

        renderSettings.useBlend = true;
        renderSettings.useDepth = false;
        renderSettings.useTexture = true;
        renderSettings.useLighting = false;

        // Draw the sprite
        std::vector<std::shared_ptr<Texture>> textures;
        textures.push_back(texture);
        Engine::renderer->DrawMeshData(spriteMeshData, textures, renderSettings);
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