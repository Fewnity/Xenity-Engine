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

ProfilerBenchmark* meshBenchmark = nullptr;
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

std::shared_ptr <MeshData> MeshManager::LoadMesh(const std::string& path)
{
	std::shared_ptr <MeshData> mesh = MeshData::MakeMeshData();
	mesh->file = FileSystem::MakeFile(path);
	mesh->fileType = File_Mesh;
	mesh->LoadFileReference();
	return mesh;
}

void MeshManager::DrawMesh(const Vector3& position, const Vector3& rotation, const Vector3& scale, const std::vector<std::shared_ptr<Texture>>& textures, std::shared_ptr <MeshData> meshData, RenderingSettings& renderSettings, std::shared_ptr <Material> material)
{
	if (!meshData)
		return;

	std::shared_ptr<Camera> camera = Graphics::usedCamera.lock();
	if (!camera)
		return;

	if (!Engine::UseOpenGLFixedFunctions)
	{
		material->Use();
		Graphics::currentShader->SetShaderModel(position, rotation, scale);
	}

	meshBenchmark->Start();
	meshCameraBenchmark->Start();
#if defined(__PSP__)
	if (Graphics::needUpdateCamera)
	{
		Engine::renderer->SetCameraPosition(Graphics::usedCamera);
		Graphics::needUpdateCamera = false;
	}
#else
	if (Engine::UseOpenGLFixedFunctions)
		Engine::renderer->SetCameraPosition(Graphics::usedCamera);
#endif
	meshCameraBenchmark->Stop();

	meshTransformBenchmark->Start();
	if (Engine::UseOpenGLFixedFunctions)
		Engine::renderer->SetTransform(position, rotation, scale, true);

	meshTransformBenchmark->Stop();

	// Set draw settings
	if (scale.x * scale.y * scale.z < 0)
		renderSettings.invertFaces = !renderSettings.invertFaces;

	meshDrawBenchmark->Start();
	Engine::renderer->DrawMeshData(meshData, textures, renderSettings);
	meshDrawBenchmark->Stop();

	meshBenchmark->Stop();
}

void MeshManager::DrawMesh(std::shared_ptr<Transform> transform, const std::vector<std::shared_ptr<Texture>>& textures, std::shared_ptr<MeshData> meshData, RenderingSettings& renderSettings, std::shared_ptr <Material> material)
{
	if (!meshData)
		return;

	std::shared_ptr<Camera> camera = Graphics::usedCamera.lock();
	if (!camera)
		return;

	if (!Engine::UseOpenGLFixedFunctions) 
	{
		if (!material)
			return;

		material->Use();
		Graphics::currentShader->SetShaderModel(transform->transformationMatrix); //----------------------------
	}

	meshBenchmark->Start();
	meshCameraBenchmark->Start();
#if defined(__PSP__)
	if (Graphics::needUpdateCamera)
	{
		Engine::renderer->SetCameraPosition(Graphics::usedCamera);
		Graphics::needUpdateCamera = false;
	}
#else
	if (Engine::UseOpenGLFixedFunctions)
		Engine::renderer->SetCameraPosition(Graphics::usedCamera);
#endif
	meshCameraBenchmark->Stop();

	meshTransformBenchmark->Start();
	Vector3 scale = transform->GetScale();

	if (Engine::UseOpenGLFixedFunctions)
	{
		//----------------------------
#if defined(__PSP__)
		Vector3 position = transform->GetPosition();
		Vector3 rotation = transform->GetRotation();
		//Engine::renderer->SetTransform(position, rotation, scale, true);
		Engine::renderer->SetTransform(transform->transformationMatrix);
#else
		Engine::renderer->SetTransform(transform->transformationMatrix);
#endif
		//----------------------------
	}

	meshTransformBenchmark->Stop();

	// Set draw settings
	if (scale.x * scale.y * scale.z < 0)
		renderSettings.invertFaces = !renderSettings.invertFaces;

	meshDrawBenchmark->Start();
	Engine::renderer->DrawMeshData(meshData, textures, renderSettings);
	meshDrawBenchmark->Stop();

	meshBenchmark->Stop();
}

void MeshManager::DrawMesh(const Vector3& position, const Vector3& rotation, const Vector3& scale, std::shared_ptr <Texture> texture, std::shared_ptr < MeshData> meshData, RenderingSettings& renderSettings, std::shared_ptr <Material> material)
{
	std::vector< std::shared_ptr<Texture>> textures;
	textures.push_back(texture);
	DrawMesh(position, rotation, scale, textures, meshData, renderSettings, material);
}
