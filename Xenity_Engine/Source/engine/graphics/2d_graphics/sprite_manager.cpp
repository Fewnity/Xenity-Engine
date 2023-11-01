#include "sprite_manager.h"
#include "../../../xenity.h"
#include "../3d_graphics/mesh_data.h"
#include <cstdlib>
#include <malloc.h>
#include "../renderer/renderer.h"

#include "../../../../include/stb_image_resize.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

#if defined(__PSP__)
#include <pspkernel.h>
#endif

std::shared_ptr <MeshData> SpriteManager::spriteMeshData = nullptr;

ProfilerBenchmark* spriteBenchmark = nullptr;

/**
 * @brief Init the Sprite Manager
 *
 */
void SpriteManager::Init()
{
	spriteBenchmark = new ProfilerBenchmark("Sprite", "Sprite");

	// Create sprite mesh
	spriteMeshData = MeshData::MakeMeshData(4, 6, false, false, true);
	spriteMeshData->AddVertex(1.0f, 1.0f, -0.5f, -0.5f, 0.0f, 0, 0);
	spriteMeshData->AddVertex(0.0f, 1.0f, 0.5f, -0.5f, 0.0f, 1, 0);
	spriteMeshData->AddVertex(0.0f, 0.0f, 0.5f, 0.5f, 0.0f, 2, 0);
	spriteMeshData->AddVertex(1.0f, 0.0f, -0.5f, 0.5f, 0.0f, 3, 0);

	MeshData::SubMesh* subMesh = spriteMeshData->subMeshes[0];

	subMesh->indices[0] = 0;
	subMesh->indices[1] = 2;
	subMesh->indices[2] = 1;
	subMesh->indices[3] = 2;
	subMesh->indices[4] = 0;
	subMesh->indices[5] = 3;
	spriteMeshData->OnLoadFileReferenceFinished();

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
void SpriteManager::DrawSprite(const std::shared_ptr<Transform>& transform, const std::shared_ptr<Texture>& texture, const Color& color, const std::shared_ptr<Material>& material)
{
	spriteBenchmark->Start();

	spriteMeshData->unifiedColor = color;

	Vector3 scale = transform->GetScale();
	RenderingSettings renderSettings = RenderingSettings();

	if (scale.x * scale.y < 0)
		renderSettings.invertFaces = true;
	else
		renderSettings.invertFaces = false;

	renderSettings.useBlend = true;
	renderSettings.useDepth = false;
	renderSettings.useTexture = true;
	renderSettings.useLighting = false;

	float scaleCoef = (1.0f / texture->GetPixelPerUnit());
	float w = texture->GetWidth() * scaleCoef;
	float h = texture->GetHeight() * scaleCoef;

	glm::mat4 matCopy = transform->transformationMatrix;
	matCopy = glm::scale(matCopy, glm::vec3(w, h, 1));

	std::vector<std::shared_ptr<Texture>> textures;
	textures.push_back(texture);
	Graphics::DrawMesh(spriteMeshData, textures, renderSettings, matCopy, material, false);
	spriteBenchmark->Stop();
}

void SpriteManager::DrawSprite(const Vector3& position, const Vector3& rotation, const Vector3& scale, const std::shared_ptr<Texture>& texture, const Color& color, const std::shared_ptr<Material>& material)
{
	spriteBenchmark->Start();
	glm::mat4 matrix = Math::CreateModelMatrix(position, rotation, scale);

	spriteMeshData->unifiedColor = color;

	//Vector3 scale = transform->GetScale();
	RenderingSettings renderSettings = RenderingSettings();

	if (scale.x * scale.y < 0)
		renderSettings.invertFaces = true;
	else
		renderSettings.invertFaces = false;

	renderSettings.useBlend = true;
	renderSettings.useDepth = false;
	renderSettings.useTexture = true;
	renderSettings.useLighting = false;

	float scaleCoef = (1.0f / texture->GetPixelPerUnit());
	float w = texture->GetWidth() * scaleCoef;
	float h = texture->GetHeight() * scaleCoef;

	//glm::mat4 matCopy = transform->transformationMatrix;
	matrix = glm::scale(matrix, glm::vec3(w, h, 1));

	std::vector<std::shared_ptr<Texture>> textures;
	textures.push_back(texture);
	Graphics::DrawMesh(spriteMeshData, textures, renderSettings, matrix, material, false);
	spriteBenchmark->Stop();
}

void SpriteManager::Render2DLine(const std::shared_ptr<MeshData>& meshData)
{
	if (!AssetManager::defaultTexture || !AssetManager::defaultTexture->IsValid())
	{
		Debug::PrintError("[SpriteManager::Render2DLine] Invalid texture");
		return;
	}

	spriteBenchmark->Start();
	if (Graphics::usedCamera.lock())
	{
#if defined(__PSP__)
		if (Graphics::needUpdateCamera)
		{
			Graphics::usedCamera.lock()->UpdateProjection();
			Engine::GetRenderer().SetCameraPosition(Graphics::usedCamera.lock());
			Graphics::needUpdateCamera = false;
		}
#else
		Engine::GetRenderer().SetCameraPosition(Graphics::usedCamera.lock());
#endif

		Vector3 zero = Vector3(0);
		Vector3 one = Vector3(1);

		Engine::GetRenderer().SetTransform(zero, zero, one, true);

		// Set draw settings
		RenderingSettings renderSettings = RenderingSettings();

		renderSettings.invertFaces = true;
		renderSettings.useBlend = true;
		renderSettings.useDepth = false;
		renderSettings.useTexture = true;
		renderSettings.useLighting = false;

		std::vector<std::shared_ptr<Texture>> textures;
		textures.push_back(AssetManager::defaultTexture);
		Engine::GetRenderer().DrawMeshData(meshData, textures, renderSettings);
	}
	spriteBenchmark->Stop();
}