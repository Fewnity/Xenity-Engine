#include "mesh.h"
#include "../../../xenity.h"
#include "../../graphics/renderer/renderer.h"
#include "../../file_system/mesh_loader/wavefront_loader.h"

#include <vector>

using namespace std;

#pragma region MeshData Constructors / Destructor

/// <summary>
/// Used if a mesh is not created from a file
/// </summary>
MeshData::MeshData()
{
	filePath = "";
	AssetManager::AddMeshData(this);
}

MeshData::MeshData(const std::string path)
{
	filePath = path;
	AssetManager::AddMeshData(this);
}

MeshData::~MeshData()
{
	AssetManager::RemoveMeshData(this);
	free(vertices);
	free(indices);
}

#pragma endregion

#pragma region MeshRenderer Constructors / Destructor

/// <summary>
/// Instantiate an empty mesh
/// </summary>
MeshRenderer::MeshRenderer()
{
	componentName = "Mesh Renderer";
	/*meshData = new MeshData();
	meshData->verticesCount = 0;
	meshData->vertices = (float*)calloc(meshData->verticesCount, sizeof(float));
	meshData->indicesCount = 0;
	meshData->indices = (unsigned int*)calloc(meshData->indicesCount, sizeof(unsigned int));
	CreateBuffers(true, true);*/
}

/*MeshRenderer::MeshRenderer(MeshData* meshData)
{
	LoadFromMeshData(meshData);
}*/

/// <summary>
/// Instantiate a mesh from a vertices and indices lists
/// </summary>
/// <param name="vertices"></param>
/// <param name="indices"></param>
/// <param name="verticesCount"></param>
/// <param name="indicesCount"></param>
/*MeshRenderer::MeshRenderer(const float vertices[], const unsigned int indices[], const int verticesCount, const int indicesCount)
{
	meshData = new MeshData();
	meshData->verticesCount = verticesCount / sizeof(*vertices);
	meshData->vertices = (float*)calloc(meshData->verticesCount, sizeof(float));
	meshData->indicesCount = indicesCount / sizeof(*indices);
	meshData->indices = (unsigned int*)calloc(meshData->indicesCount, sizeof(unsigned int));
	LoadFromRawData(vertices, indices);
	CreateBuffers(true, true);
}*/

/// <summary>
/// Instantiate a mesh from a file
/// </summary>
/// <param name="meshpath"></param>
/*MeshRenderer::MeshRenderer(const std::string meshpath)
{
	LoadFromFile(meshpath);
}*/

/// <summary>
/// Destructor
/// </summary>
MeshRenderer::~MeshRenderer()
{
	delete meshData;
	Engine::renderer->DeleteVertexArray(vertexArrayBuffer);
	Engine::renderer->DeleteBuffer(vertexBuffer);
	Engine::renderer->DeleteBuffer(indiceBuffer);
}

#pragma endregion

#pragma region Data loading

/// <summary>
/// Load the mesh from a file
/// </summary>
/// <param name="meshpath"></param>
void MeshRenderer::LoadFromFile(const std::string meshpath)
{
	MeshData* foundData = AssetManager::GetMeshData(meshpath);
	if (foundData == nullptr)
	{
		meshData = new MeshData(meshpath);
		WavefrontLoader::LoadFromRawData(meshData, meshpath);
		CreateBuffers(meshData->hasUv, meshData->hasNormal);
	}
	else
	{
		LoadFromMeshData(foundData);
	}
}

void MeshRenderer::LoadFromMeshData(MeshData* meshData)
{
	this->meshData = meshData;
	CreateBuffers(meshData->hasUv, meshData->hasNormal);
}

/// <summary>
/// Fill mesh data with vertices and indices lists
/// </summary>
/// <param name="vertices"></param>
/// <param name="indices"></param>
void MeshRenderer::LoadFromRawData(const float vertices[], const unsigned int indices[]) {
	//Fill vertices
	for (int i = 0; i < meshData->verticesCount; i++)
	{
		meshData->vertices[i] = vertices[i];
	}

	//Fill indices
	for (int i = 0; i < meshData->indicesCount; i++)
	{
		meshData->indices[i] = indices[i];
	}
}

#pragma endregion



void MeshRenderer::Update()
{
}

/// <summary>
/// Create mesh's buffers
/// </summary>
/// <param name="addUv"></param>
/// <param name="addNormals"></param>
void MeshRenderer::CreateBuffers(const bool addUv, const bool addNormals)
{
	vertexArrayBuffer = Engine::renderer->GenerateVertexArray(); //Create a buffer ID for the vertex array buffer
	vertexBuffer = Engine::renderer->GenerateBuffer(); //Create a buffer ID for the vertex buffer
	indiceBuffer = Engine::renderer->GenerateBuffer(); //Create a buffer ID for the indice buffer


	Engine::renderer->BindVertexArray(vertexArrayBuffer);
	Engine::renderer->BindBuffer(Array_Buffer, vertexBuffer);
	Engine::renderer->BindBuffer(Element_Array_Buffer, indiceBuffer);

	int finalByteCount = 3;
	if (addNormals)
		finalByteCount += 3;
	if (addUv)
		finalByteCount += 2;

	int byteOffset = 0;
	Engine::renderer->SetVertexAttribPointer(0, 3, finalByteCount, byteOffset);
	//Enable vertex attrib array number 0
	Engine::renderer->SetVertexAttribArray(true, 0);
	byteOffset += 3;

	if (addUv)
	{
		Engine::renderer->SetVertexAttribPointer(1, 2, finalByteCount, byteOffset);
		//Enable texture coords attrib array number 1
		Engine::renderer->SetVertexAttribArray(true, 1);
		byteOffset += 2;
	}

	if (addNormals)
	{
		Engine::renderer->SetVertexAttribPointer(2, 3, finalByteCount, byteOffset);
		//Enable Normals attrib array number 2
		Engine::renderer->SetVertexAttribArray(true, 2);
		byteOffset += 3;
	}

	Engine::renderer->SetBufferData(Array_Buffer, Static, meshData);
	
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData->indicesCount * sizeof(*meshData->indices), meshData->indices, GL_STATIC_DRAW);//Put vertice in the array buffer
	//glDrawElements(GL_TRIANGLES, meshData->indicesCount, GL_UNSIGNED_INT, 0);

	Engine::renderer->BindVertexArray(0);
}

#pragma region Drawing

/// <summary>
/// Draw mesh
/// </summary>
void MeshRenderer::Draw()
{
	//glPatchParameteri(GL_PATCH_VERTICES, 3); // For tessellation

	//Draw the mesh only if the mesh is on an active gameobject and if the mesh data is not null
	if (GetGameObject()->GetLocalActive() && meshData != nullptr && GetIsEnabled())
	{
		if (material != nullptr)
		{
			Engine::renderer->SetDepthTest(true);
			UpdateMaterial();
			Performance::AddDrawCall();
			Engine::renderer->BindVertexArray(vertexArrayBuffer);

			Engine::renderer->BindBuffer(Array_Buffer, vertexBuffer);
			Engine::renderer->BindBuffer(Element_Array_Buffer, indiceBuffer);

			if (material->shader->useTessellation)
				Engine::renderer->DrawArray(Patches, 0, meshData->verticesCount);
			else
				Engine::renderer->DrawArray(Triangles, 0, meshData->verticesCount);

			Engine::renderer->BindVertexArray(0);
		}
	}
}

int MeshRenderer::GetDrawPriority() const
{
	return 0;
}

/// <summary>
/// Update mesh's material
/// </summary>
void MeshRenderer::UpdateMaterial()
{
	if (material != nullptr && material->shader != nullptr && Graphics::usedCamera != nullptr)
	{
		material->Use();
		bool noNeedUpdate = material->updated;
		if (!noNeedUpdate)
		{
			material->Update();
			material->shader->SetShaderCameraPosition();
			material->shader->SetShaderProjection();
			material->shader->SetShaderAttribut("cameraPos", Graphics::usedCamera->GetGameObject()->transform.GetPosition());
			material->shader->SetShaderAttribut("offsetPosition", Vector3(0, 0, 0));
			material->shader->UpdateLights();
		}
		material->shader->SetShaderModel(&GetGameObject()->transform.transformationMatrix);
	}
}

#pragma endregion

