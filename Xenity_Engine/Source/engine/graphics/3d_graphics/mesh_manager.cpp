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

//ProfilerBenchmark* meshBenchmark = nullptr;

void MeshManager::Init()
{
	//meshBenchmark = new ProfilerBenchmark("Mesh", "Mesh");
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
	//meshBenchmark->Start();
	glm::mat4 matrix = Math::CreateModelMatrix(position, rotation, scale);
	Graphics::DrawMesh(meshData, textures, renderSettings, matrix, material, false);
	//meshBenchmark->Stop();
}

void MeshManager::DrawMesh(const std::shared_ptr<Transform>& transform, const std::vector<std::shared_ptr<Texture>>& textures, const std::shared_ptr<MeshData>& meshData, RenderingSettings& renderSettings, const std::shared_ptr<Material>& material)
{
	//meshBenchmark->Start();

	Vector3 scale = transform->GetScale();

	if (scale.x * scale.y * scale.z < 0)
		renderSettings.invertFaces = !renderSettings.invertFaces;

	Graphics::DrawMesh(meshData, textures, renderSettings, transform->transformationMatrix, material, false);
	//meshBenchmark->Stop();
}

void MeshManager::DrawMesh(const Vector3& position, const Vector3& rotation, const Vector3& scale, const std::shared_ptr<Texture>& texture, const std::shared_ptr<MeshData>& meshData, RenderingSettings& renderSettings, const std::shared_ptr<Material>& material)
{
	std::vector<std::shared_ptr<Texture>> textures;
	textures.push_back(texture);
	DrawMesh(position, rotation, scale, textures, meshData, renderSettings, material);
}
