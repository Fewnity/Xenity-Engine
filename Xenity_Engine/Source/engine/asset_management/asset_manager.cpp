#include "asset_manager.h"

#include "../../xenity.h"
#include "../../engine/file_system/file_reference.h"

bool initialised = false;

// std::vector<Shader*> AssetManager::shaders;
std::vector<Material*> AssetManager::materials;
std::vector<Reflection*> AssetManager::reflections;
std::vector<std::shared_ptr<FileReference>> AssetManager::fileReferences;
std::vector<std::weak_ptr<IDrawable>> AssetManager::drawables;
std::vector<std::weak_ptr<Light>> AssetManager::lights;
// std::vector<MeshData*> AssetManager::meshesData;

// int AssetManager::shaderCount = 0;
int AssetManager::materialCount = 0;
int AssetManager::reflectionCount = 0;
int AssetManager::fileReferenceCount = 0;
int AssetManager::drawableCount = 0;
int AssetManager::lightCount = 0;
// int AssetManager::meshDataCount = 0;

// Material* AssetManager::default3DMaterial = nullptr;
// Material* AssetManager::default2DMaterial = nullptr;
// Material* AssetManager::defaultUIMaterial = nullptr;

std::shared_ptr <Texture> AssetManager::defaultTexture = nullptr;

// void AssetManager::ResetMaterialsUpdates()
// {
// 	for (int i = 0; i < materialCount; i++)
// 	{
// 		materials[i]->updated = false;
// 	}
// }

/**
 * @brief Init
 *
 */
void AssetManager::Init()
{
	initialised = true;
	defaultTexture = Texture::MakeTexture("engine_assets\\default_texture.png", true);
	defaultTexture->file = FileSystem::MakeFile("engine_assets\\default_texture.png");
	defaultTexture->LoadFileReference();
	// Shader* standard3D = new Shader("3D/vStandard.shader", "3D/fStandard.shader");

	// default3DMaterial = new Material("3D Standard");
	// default3DMaterial->shader = standard3D;
	// default3DMaterial->SetAttribut("material.diffuse", defaultTexture);
	// default3DMaterial->SetAttribut("material.specular", defaultTexture);
	// default3DMaterial->SetAttribut("material.shininess", 32.0f);
	// default3DMaterial->SetAttribut("ambiantLightColor", Vector3(0.7f, 0.7f, 0.7f));

	// Shader* sprite2D = new Shader("2D/vSprite.shader", "2D/fSprite.shader");

	// default2DMaterial = new Material("2D Standard 2");
	// default2DMaterial->shader = sprite2D;

	// Shader* ui = new Shader("UI/vUi.shader", "UI/fUi.shader");

	// defaultUIMaterial = new Material("UI");
	// defaultUIMaterial->shader = ui;

	Debug::Print("-------- Asset Manager initiated --------");
}

#pragma region Add assets

/// <summary>
/// Add a shader in the shader list
/// </summary>
/// <param name="shader"></param>
// void AssetManager::AddShader(Shader *shader)
// {
// 	shaders.push_back(shader);
// 	shaderCount++;
// }

 void AssetManager::AddMaterial(Material *material)
 {
 	materials.push_back(material);
 	materialCount++;
 }

void AssetManager::AddReflection(Reflection* reflection)
{
#if defined(EDITOR)
	if (initialised)
	{
		reflections.push_back(reflection);
		reflectionCount++;
	}
#endif
}

void AssetManager::AddFileReference(std::shared_ptr<FileReference> fileReference)
{
	for (int i = 0; i < fileReferenceCount; i++)
	{
		if (fileReferences[i] == fileReference)
		{
			Debug::Print("OAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
			return;
		}
	}
	fileReferences.push_back(fileReference);
	fileReferenceCount++;
}

/// <summary>
/// Add a drawable in the drawable list
/// </summary>
/// <param name="drawable"></param>
void AssetManager::AddDrawable(std::weak_ptr<IDrawable> drawable)
{
	drawables.push_back(drawable);
	drawableCount++;
}

/// <summary>
/// Add a light in the light list
/// </summary>
/// <param name="light"></param>
void AssetManager::AddLight(std::weak_ptr<Light> light)
{
	lights.push_back(light);
	lightCount++;
}

/// <summary>
/// Add a mesh data in the mesh data list
/// </summary>
/// <param name="meshData"></param>
// void AssetManager::AddMeshData(MeshData *meshData)
// {
// 	meshesData.push_back(meshData);
// 	meshDataCount++;
// }

#pragma endregion

#pragma region Remove assets

/// <summary>
/// Remove a shader from the shader list
/// </summary>
/// <param name="shader"></param>
// void AssetManager::RemoveShader(Shader *shader)
// {
// 	int shaderCount = (int)shaders.size();
// 	int shaderIndex = 0;
// 	bool found = false;
// 	for (int i = 0; i < shaderCount; i++)
// 	{
// 		if (shaders[i] == shader)
// 		{
// 			found = true;
// 			shaderIndex = i;
// 			break;
// 		}
// 	}

// 	if (found)
// 	{
// 		shaders.erase(shaders.begin() + shaderIndex);
// 		shaderCount--;
// 	}
// }

 void AssetManager::RemoveMaterial(Material *material)
 {
 	int materialIndex = 0;
 	bool found = false;
 	for (int i = 0; i < materialCount; i++)
 	{
 		if (materials[i] == material)
 		{
 			found = true;
 			materialIndex = i;
 			break;
 		}
 	}

 	if (found)
 	{
 		materials.erase(materials.begin() + materialIndex);
 		materialCount--;
 	}
 }

void AssetManager::RemoveReflection(Reflection* reflection)
{
#if defined(EDITOR)
	if (initialised)
	{
		int reflectionIndex = 0;
		bool found = false;
		for (int i = 0; i < reflectionCount; i++)
		{
			if (reflections[i] == reflection)
			{
				found = true;
				reflectionIndex = i;
				break;
			}
		}

		if (found)
		{
			reflections.erase(reflections.begin() + reflectionIndex);
			reflectionCount--;
		}
	}
#endif
}

void AssetManager::ForceDeleteFileReference(std::shared_ptr<FileReference> fileReference) 
{
	RemoveFileReference(fileReference);
	for (int i = 0; i < reflectionCount; i++)
	{
		auto map = reflections[i]->GetReflection();
		for (auto& kv : map)
		{
			Variable& variableRef = kv.second.variable.value();
			if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<MeshData>>>(&variableRef))
			{
				if (valuePtr->get() == fileReference)
				{
					valuePtr->get().reset();
				}
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<AudioClip>>>(&variableRef))
			{
				if (valuePtr->get() == fileReference)
				{
					valuePtr->get().reset();
				}
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Texture>>>(&variableRef))
			{
				if (valuePtr->get() == fileReference)
				{
					valuePtr->get().reset();
				}
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Scene>>>(&variableRef))
			{
				if (valuePtr->get() == fileReference)
				{
					valuePtr->get().reset();
				}
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<SkyBox>>>(&variableRef))
			{
				if (valuePtr->get() == fileReference)
				{
					valuePtr->get().reset();
				}
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::shared_ptr<Font>>>(&variableRef))
			{
				if (valuePtr->get() == fileReference)
				{
					valuePtr->get().reset();
				}
			}
			else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<std::shared_ptr<Texture>>>>(&variableRef))
			{
				int vectorSize = valuePtr->get().size();
				for (int i = 0; i < vectorSize; i++)
				{
					if (valuePtr->get()[i] == fileReference)
					{
						valuePtr->get()[i].reset();
					}
				}
			}
		}
	}
}

void AssetManager::RemoveAllFileReferences()
{
	fileReferences.clear();
	fileReferenceCount = 0;
}

void AssetManager::RemoveFileReference(std::shared_ptr<FileReference> fileReference)
{
	int fileReferenceIndex = 0;
	bool found = false;
	for (int i = 0; i < fileReferenceCount; i++)
	{
		if (fileReferences[i] == fileReference)
		{
			found = true;
			fileReferenceIndex = i;
			break;
		}
	}

	if (found)
	{
		fileReferences.erase(fileReferences.begin() + fileReferenceIndex);
		fileReferenceCount--;
	}
}

/// <summary>
/// Remove a drawable from the drawable list
/// </summary>
/// <param name="drawable"></param>
void AssetManager::RemoveDrawable(std::weak_ptr<IDrawable> drawable)
{
	int drawableIndex = 0;
	bool found = false;
	for (int i = 0; i < drawableCount; i++)
	{
		if (drawables[i].lock() == drawable.lock())
		{
			found = true;
			drawableIndex = i;
			break;
		}
	}

	if (found)
	{
		drawables.erase(drawables.begin() + drawableIndex);
		drawableCount--;
	}
}

/// <summary>
/// Remove a light from the light list
/// </summary>
/// <param name="light"></param>
void AssetManager::RemoveLight(std::weak_ptr<Light> light)
{
	int lightIndex = 0;
	bool found = false;
	for (int i = 0; i < lightCount; i++)
	{
		if (lights[i].lock() == light.lock())
		{
			found = true;
			lightIndex = i;
			break;
		}
	}

	if (found)
	{
		lights.erase(lights.begin() + lightIndex);
		lightCount--;
	}
}

// void AssetManager::RemoveMeshData(MeshData *meshData)
// {
// 	int meshDataCount = (int)meshesData.size();
// 	int meshDataIndex = 0;
// 	bool found = false;
// 	for (int i = 0; i < meshDataCount; i++)
// 	{
// 		if (meshesData[i] == meshData)
// 		{
// 			found = true;
// 			meshDataIndex = i;
// 			break;
// 		}
// 	}

// 	if (found)
// 	{
// 		meshesData.erase(meshesData.begin() + meshDataIndex);
// 		meshDataCount--;
// 	}
// }

#pragma endregion

#pragma region Getters

// Shader *AssetManager::GetShader(const int index)
// {
// 	return shaders[index];
// }

 Material *AssetManager::GetMaterial(const int index)
 {
 	return materials[index];
 }

// Material *AssetManager::GetMaterialByName(const std::string name)
// {
// 	for (int i = 0; i < materialCount; i++)
// 	{
// 		if (materials[i]->name == name)
// 		{
// 			return materials[i];
// 		}
// 	}
// 	return nullptr;
// }

Reflection* AssetManager::GetReflection(const int index)
{
	return reflections[index];
}

std::shared_ptr<FileReference> AssetManager::GetFileReference(const int index)
{
	return fileReferences[index];
}

std::weak_ptr<IDrawable> AssetManager::GetDrawable(const int index)
{
	return drawables[index];
}

std::weak_ptr<Light> AssetManager::GetLight(const int index)
{
	return lights[index];
}

// MeshData *AssetManager::GetMeshData(const int index)
// {
// 	return meshesData[index];
// }

// MeshData *AssetManager::GetMeshData(const std::string path)
// {
// 	for (int i = 0; i < meshDataCount; i++)
// 	{
// 		if (meshesData[i]->filePath == path)
// 		{
// 			return meshesData[i];
// 		}
// 	}
// 	return nullptr;
// }

/// <summary>
/// Get sahder count
/// </summary>
/// <returns></returns>
// int AssetManager::GetShaderCount()
// {
// 	return shaderCount;
// }

 int AssetManager::GetMaterialCount()
 {
 	return materialCount;
 }

int AssetManager::GetReflectionCount()
{
	return reflectionCount;
}

int AssetManager::GetFileReferenceCount()
{
	return fileReferenceCount;
}

int AssetManager::GetDrawableCount()
{
	return drawableCount;
}

int AssetManager::GetLightCount()
{
	return lightCount;
}

// int AssetManager::GetMeshDataCount()
// {
// 	return meshDataCount;
// }

#pragma endregion