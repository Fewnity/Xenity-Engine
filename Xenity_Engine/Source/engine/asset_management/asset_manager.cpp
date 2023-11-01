#include "asset_manager.h"

#include "../../xenity.h"
#include "../../engine/file_system/file_reference.h"

bool initialised = false;

std::vector<Material*> AssetManager::materials;
std::vector<Reflection*> AssetManager::reflections;
std::vector<std::shared_ptr<FileReference>> AssetManager::fileReferences;
std::vector<std::weak_ptr<IDrawable>> AssetManager::drawables;
std::vector<std::weak_ptr<Light>> AssetManager::lights;

int AssetManager::materialCount = 0;
int AssetManager::reflectionCount = 0;
int AssetManager::fileReferenceCount = 0;
int AssetManager::drawableCount = 0;
int AssetManager::lightCount = 0;

std::shared_ptr <Shader> AssetManager::shader = nullptr;
std::shared_ptr <Shader> AssetManager::unlitShader = nullptr;
std::shared_ptr <Shader> AssetManager::lineShader = nullptr;
std::shared_ptr<Material> AssetManager::standardMaterial = nullptr;
std::shared_ptr<Material> AssetManager::unlitMaterial = nullptr;
std::shared_ptr<Material> AssetManager::lineMaterial = nullptr;

std::shared_ptr <Texture> AssetManager::defaultTexture = nullptr;

/**
 * @brief Init
 *
 */
void AssetManager::Init()
{
	initialised = true;
	defaultTexture = Texture::MakeTexture();
	defaultTexture->file = FileSystem::MakeFile("engine_assets\\default_texture.png");
	defaultTexture->LoadFileReference();

	if (!Graphics::UseOpenGLFixedFunctions)
	{
		// Load standard shader
		shader = Shader::MakeShader();
#if defined(__vita__)
		shader->file = FileSystem::MakeFile("shaders/standard_psvita.shader");
#else
		shader->file = FileSystem::MakeFile("shaders/standard.shader");
#endif
		// Load unlit shader
		unlitShader = Shader::MakeShader();
#if defined(__vita__)
		unlitShader->file = FileSystem::MakeFile("shaders/unlit_psvita.shader");
#else
		unlitShader->file = FileSystem::MakeFile("shaders/unlit.shader");
#endif
		
		// Load line shader
		lineShader = Shader::MakeShader();
#if defined(__vita__)
		lineShader->file = FileSystem::MakeFile("shaders/line_psvita.shader");
#else
		lineShader->file = FileSystem::MakeFile("shaders/line.shader");
#endif

		// Create materials
		standardMaterial = Material::MakeMaterial();
		standardMaterial->file = FileSystem::MakeFile("shaders/standardMaterial.mat");
		standardMaterial->shader = shader;
		standardMaterial->useLighting = true;

		unlitMaterial = Material::MakeMaterial();
		unlitMaterial->file = FileSystem::MakeFile("shaders/unlitMaterial.mat");
		unlitMaterial->shader = unlitShader;
		//Engine::unlitMaterial->SetAttribut("color", Vector3(1, 1, 1));

		lineMaterial = Material::MakeMaterial();
		lineMaterial->file = FileSystem::MakeFile("shaders/lineMaterial.mat");
		lineMaterial->shader = lineShader;

		shader->LoadFileReference();
		unlitShader->LoadFileReference();
		lineShader->LoadFileReference();

		standardMaterial->LoadFileReference();
		unlitMaterial->LoadFileReference();
		lineMaterial->LoadFileReference();
	}

	Debug::Print("-------- Asset Manager initiated --------");
}

#pragma region Add assets

void AssetManager::AddMaterial(Material* material)
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

void AssetManager::AddFileReference(const std::shared_ptr<FileReference>& fileReference)
{
	fileReferences.push_back(fileReference);
	fileReferenceCount++;
}

/// <summary>
/// Add a drawable in the drawable list
/// </summary>
/// <param name="drawable"></param>
void AssetManager::AddDrawable(const std::weak_ptr<IDrawable>& drawable)
{
	drawables.push_back(drawable);
	drawableCount++;
}

/// <summary>
/// Add a light in the light list
/// </summary>
/// <param name="light"></param>
void AssetManager::AddLight(const std::weak_ptr<Light>& light)
{
	lights.push_back(light);
	lightCount++;
}

#pragma endregion

#pragma region Remove assets

void AssetManager::RemoveMaterial(const Material* material)
{
	if (!Engine::IsRunning())
		return;

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

void AssetManager::RemoveReflection(const Reflection* reflection)
{
	if (!Engine::IsRunning())
		return;

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

void AssetManager::ForceDeleteFileReference(const std::shared_ptr<FileReference>& fileReference)
{
	RemoveFileReference(fileReference);
	for (int reflectionIndex = 0; reflectionIndex < reflectionCount; reflectionIndex++)
	{
		auto map = reflections[reflectionIndex]->GetReflection();
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

void AssetManager::RemoveFileReference(const std::shared_ptr<FileReference>& fileReference)
{
	if (!Engine::IsRunning())
		return;

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
void AssetManager::RemoveDrawable(const std::weak_ptr<IDrawable>& drawable)
{
	if (!Engine::IsRunning())
		return;

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
void AssetManager::RemoveLight(const std::weak_ptr<Light>& light)
{
	if (!Engine::IsRunning())
		return;

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

#pragma endregion

#pragma region Getters

Material* AssetManager::GetMaterial(const int index)
{
	return materials[index];
}

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

std::string AssetManager::GetDefaultFileData(FileType fileType)
{
	std::string data = "{\n}";
	std::shared_ptr<File> newFile;

	switch (fileType)
	{
	case File_Scene:
		newFile = FileSystem::MakeFile("engine_assets\\empty_default\\scene.xen");
		break;
	case File_Code:
		newFile = FileSystem::MakeFile("engine_assets\\empty_default\\class.cpp");
		break;
	case File_Header:
		newFile = FileSystem::MakeFile("engine_assets\\empty_default\\class.h");
		break;
	case File_Skybox:
		newFile = FileSystem::MakeFile("engine_assets\\empty_default\\skybox.sky");
		break;
	case File_Material:
		newFile = FileSystem::MakeFile("engine_assets\\empty_default\\material.mat");
		break;
	case File_Shader:
		newFile = FileSystem::MakeFile("engine_assets\\empty_default\\shader.shader");
		break;
	}

	if (newFile && newFile->Open(false))
	{
		data = newFile->ReadAll();
		newFile->Close();
	}
	else 
	{
		Debug::PrintError("[AssetManager::GetDefaultFileData] Default file not found");
	}

	return data;
}

#pragma endregion