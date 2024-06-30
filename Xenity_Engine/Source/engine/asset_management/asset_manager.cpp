// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "asset_manager.h"

#include <engine/engine.h>
#include <engine/debug/debug.h>

#include <engine/file_system/file_system.h>
#include <engine/file_system/file_reference.h>
#include <engine/file_system/file.h>

#include <engine/graphics/graphics.h>
#include <engine/graphics/shader.h>
#include <engine/graphics/material.h>
#include <engine/graphics/texture.h>
#include <engine/graphics/skybox.h>
#include <engine/graphics/ui/font.h>

#include <engine/scene_management/scene.h>
#include <engine/audio/audio_clip.h>
#include <engine/asset_management/project_manager.h>
#include <engine/assertions/assertions.h>

bool initialised = false;

std::vector<Material*> AssetManager::materials;
std::vector<Reflective*> AssetManager::reflections;
std::vector<std::shared_ptr<FileReference>> AssetManager::fileReferences;
std::vector<std::weak_ptr<Light>> AssetManager::lights;

int AssetManager::materialCount = 0;
int AssetManager::reflectionCount = 0;
int AssetManager::fileReferenceCount = 0;
int AssetManager::lightCount = 0;

std::shared_ptr<Shader> AssetManager::standardShader = nullptr;
std::shared_ptr<Shader> AssetManager::standardOneLightEachShader = nullptr;
std::shared_ptr<Shader> AssetManager::standardOnePointLightShader = nullptr;
std::shared_ptr<Shader> AssetManager::standardOneSpotLightShader = nullptr;
std::shared_ptr<Shader> AssetManager::standardOneDirectionalLightShader = nullptr;
std::shared_ptr<Shader> AssetManager::standardVertexLightShader = nullptr;
std::shared_ptr<Shader> AssetManager::unlitShader = nullptr;
std::shared_ptr<Shader> AssetManager::lineShader = nullptr;
std::shared_ptr<Material> AssetManager::standardMaterial = nullptr;
std::shared_ptr<Material> AssetManager::standardOneLightEachMaterial = nullptr;
std::shared_ptr<Material> AssetManager::standardOnePointLightMaterial = nullptr;
std::shared_ptr<Material> AssetManager::standardOneSpotLightMaterial = nullptr;
std::shared_ptr<Material> AssetManager::standardOneDirectionalLightMaterial = nullptr;
std::shared_ptr<Material> AssetManager::standardVertexLightMaterial = nullptr;
std::shared_ptr<Material> AssetManager::unlitMaterial = nullptr;
std::shared_ptr<Material> AssetManager::lineMaterial = nullptr;

std::shared_ptr<Texture> AssetManager::defaultTexture = nullptr;

/**
 * @brief Init
 *
 */
void AssetManager::Init()
{
	initialised = true;
	ProjectManager::GetProjectLoadedEvent().Bind(&AssetManager::OnProjectLoaded);
	ProjectManager::GetProjectUnloadedEvent().Bind(&AssetManager::OnProjectUnloaded);

	Debug::Print("-------- Asset Manager initiated --------", true);
}

template <typename T>
std::shared_ptr<T> AssetManager::LoadEngineAsset(const std::string& filePath)
{
	return std::dynamic_pointer_cast<T>(ProjectManager::GetFileReferenceByFilePath(filePath));
}

void AssetManager::OnProjectLoaded()
{
	defaultTexture = Texture::MakeTexture();
	defaultTexture->file = FileSystem::MakeFile("public_engine_assets\\textures\\default_texture.png");
	defaultTexture->LoadFileReference();

	if (!Graphics::UseOpenGLFixedFunctions)
	{
		// Load shaders
			
		standardShader = AssetManager::LoadEngineAsset<Shader>("public_engine_assets\\shaders\\standard.shader");
		standardOneLightEachShader = AssetManager::LoadEngineAsset<Shader>("public_engine_assets\\shaders\\standard_one_light_each.shader");
		standardOnePointLightShader = AssetManager::LoadEngineAsset<Shader>("public_engine_assets\\shaders\\standard_one_point_light.shader");
		standardOneSpotLightShader = AssetManager::LoadEngineAsset<Shader>("public_engine_assets\\shaders\\standard_one_spot_light.shader");
		standardOneDirectionalLightShader = AssetManager::LoadEngineAsset<Shader>("public_engine_assets\\shaders\\standard_one_directional_light.shader");
		//standardVertexLightShader = AssetManager::LoadEngineAsset<Shader>("public_engine_assets\\shaders\\standard_vertex_lighting.shader");
		unlitShader = AssetManager::LoadEngineAsset<Shader>("public_engine_assets\\shaders\\unlit.shader");
		lineShader = AssetManager::LoadEngineAsset<Shader>("public_engine_assets\\shaders\\line.shader");

		XASSERT(standardShader != nullptr, "[AssetManager::OnProjectLoaded] Standard Material is null")
		XASSERT(standardOneLightEachShader != nullptr, "[AssetManager::OnProjectLoaded] Standard Vertex Lighting Material is null")
		XASSERT(standardOnePointLightShader != nullptr, "[AssetManager::OnProjectLoaded] Standard Vertex Lighting Material is null")
		XASSERT(standardOneSpotLightShader != nullptr, "[AssetManager::OnProjectLoaded] Standard Vertex Lighting Material is null")
		XASSERT(standardOneDirectionalLightShader != nullptr, "[AssetManager::OnProjectLoaded] Standard Vertex Lighting Material is null")
		//XASSERT(standardVertexLightShader != nullptr, "[AssetManager::OnProjectLoaded] Standard Vertex Lighting Material is null")
		XASSERT(unlitShader != nullptr, "[AssetManager::OnProjectLoaded] Standard Material is null")
		XASSERT(lineShader != nullptr, "[AssetManager::OnProjectLoaded] Standard Material is null")

		standardShader->LoadFileReference();
		standardOneLightEachShader->LoadFileReference();
		standardOnePointLightShader->LoadFileReference();
		standardOneSpotLightShader->LoadFileReference();
		standardOneDirectionalLightShader->LoadFileReference();
		//standardVertexLightShader->LoadFileReference();
		unlitShader->LoadFileReference();
		lineShader->LoadFileReference();
	}

	// Load materials
	standardMaterial = AssetManager::LoadEngineAsset<Material>("public_engine_assets\\materials\\standardMaterial.mat");
	standardOneLightEachMaterial = AssetManager::LoadEngineAsset<Material>("public_engine_assets\\materials\\standardMaterialOneDirectionalLight.mat");
	standardOnePointLightMaterial = AssetManager::LoadEngineAsset<Material>("public_engine_assets\\materials\\standardMaterialOnePointLight.mat");
	standardOneSpotLightMaterial = AssetManager::LoadEngineAsset<Material>("public_engine_assets\\materials\\standardMaterialOneSpotLight.mat");
	standardOneDirectionalLightMaterial = AssetManager::LoadEngineAsset<Material>("public_engine_assets\\materials\\standardMaterialOneDirectionalLight.mat");
	//standardVertexLightMaterial = AssetManager::LoadEngineAsset<Material>("public_engine_assets\\materials\\standardVertexLightingMaterial.mat");
	unlitMaterial = AssetManager::LoadEngineAsset<Material>("public_engine_assets\\materials\\unlitMaterial.mat");
	lineMaterial = AssetManager::LoadEngineAsset<Material>("public_engine_assets\\materials\\lineMaterial.mat");
	
	XASSERT(standardMaterial != nullptr, "[AssetManager::OnProjectLoaded] Standard Material is null")
	XASSERT(standardOneLightEachMaterial != nullptr, "[AssetManager::OnProjectLoaded] Standard Vertex Lighting Material is null")
	XASSERT(standardOnePointLightMaterial != nullptr, "[AssetManager::OnProjectLoaded] Standard Vertex Lighting Material is null")
	XASSERT(standardOneSpotLightMaterial != nullptr, "[AssetManager::OnProjectLoaded] Standard Vertex Lighting Material is null")
	XASSERT(standardOneDirectionalLightMaterial != nullptr, "[AssetManager::OnProjectLoaded] Standard Vertex Lighting Material is null")
	//XASSERT(standardVertexLightMaterial != nullptr, "[AssetManager::OnProjectLoaded] Standard Vertex Lighting Material is null")
	XASSERT(unlitMaterial != nullptr, "[AssetManager::OnProjectLoaded] Standard Material is null")
	XASSERT(lineMaterial != nullptr, "[AssetManager::OnProjectLoaded] Standard Material is null")

	standardMaterial->LoadFileReference();
	standardOneLightEachMaterial->LoadFileReference();
	standardOnePointLightMaterial->LoadFileReference();
	standardOneSpotLightMaterial->LoadFileReference();
	standardOneDirectionalLightMaterial->LoadFileReference();
	//standardVertexLightMaterial->LoadFileReference();
	unlitMaterial->LoadFileReference();
	lineMaterial->LoadFileReference();
}

void AssetManager::OnProjectUnloaded()
{
	defaultTexture.reset();

	standardShader.reset();
	unlitShader.reset();
	lineShader.reset();

	standardMaterial.reset();
	unlitMaterial.reset();
	lineMaterial.reset();
}

#pragma region Add assets

void AssetManager::AddMaterial(Material* material)
{
	XASSERT(material != nullptr, "[AssetManager::AddMaterial] Material is null")

	materials.push_back(material);
	materialCount++;
}

void AssetManager::AddReflection(Reflective* reflection)
{
	XASSERT(reflection != nullptr, "[AssetManager::AddReflection] Reflection is null")

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
	XASSERT(fileReference != nullptr, "[AssetManager::AddFileReference] fileReference is null")

	fileReferences.push_back(fileReference);
	fileReferenceCount++;
}

/// <summary>
/// Add a light in the light list
/// </summary>
/// <param name="light"></param>
void AssetManager::AddLight(const std::weak_ptr<Light>& light)
{
	XASSERT(light.lock() != nullptr, "[AssetManager::AddLight] light is null")

	lights.push_back(light);
	lightCount++;
}

#pragma endregion

#pragma region Remove assets

void AssetManager::RemoveMaterial(const Material* material)
{
	XASSERT(material != nullptr, "[AssetManager::RemoveMaterial] material is null")

	if (!Engine::IsRunning(true))
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

void AssetManager::RemoveReflection(const Reflective* reflection)
{
	XASSERT(reflection != nullptr, "[AssetManager::RemoveReflection] reflection is null")

	if (!Engine::IsRunning(true))
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
	XASSERT(fileReference != nullptr, "[AssetManager::ForceDeleteFileReference] fileReference is null")

	RemoveFileReference(fileReference);
	for (int reflectionIndex = 0; reflectionIndex < reflectionCount; reflectionIndex++)
	{
		auto map = reflections[reflectionIndex]->GetReflectiveData();
		for (const ReflectiveEntry& reflectiveEntry : map)
		{
			const VariableReference& variableRef = reflectiveEntry.variable.value();
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
				const size_t vectorSize = valuePtr->get().size();
				for (size_t i = 0; i < vectorSize; i++)
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
	XASSERT(fileReference != nullptr, "[AssetManager::RemoveFileReference] fileReference is null")

	if (!Engine::IsRunning(true))
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
/// Remove a light from the light list
/// </summary>
/// <param name="light"></param>
void AssetManager::RemoveLight(const std::weak_ptr<Light>& light)
{
	XASSERT(light.lock() != nullptr, "[AssetManager::RemoveLight] light is null")

	if (!Engine::IsRunning(true))
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

Reflective* AssetManager::GetReflectiveData(const int index)
{
	return reflections[index];
}

std::shared_ptr<FileReference> AssetManager::GetFileReference(const int index)
{
	return fileReferences[index];
}

std::weak_ptr<Light> AssetManager::GetLight(const int index)
{
	return lights[index];
}

const std::vector<std::weak_ptr<Light>>& AssetManager::GetLights()
{
	return lights;
}

void AssetManager::RemoveUnusedFiles()
{
	int fileRefCount = GetFileReferenceCount();
	for (int i = 0; i < fileRefCount; i++)
	{
		std::shared_ptr<FileReference> fileRef = GetFileReference(i);
		const int refCount = fileRef.use_count();
		// If the reference count is 2 (fileRef variable and the reference in the asset manager)
#if defined(EDITOR) // Do not unload files in the editor to avoid freezes TODO: Make a cache system to reduce memory usage
		if (refCount == 1)
#else
		if (refCount == 2)
#endif
		{
			// Free the file
			RemoveFileReference(fileRef);
			fileRef.reset();
			i--;
			fileRefCount--;
		}
	}
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

int AssetManager::GetLightCount()
{
	return lightCount;
}

std::string AssetManager::GetDefaultFileData(FileType fileType)
{
	std::string data = "{\n}";
	std::shared_ptr<File> newFile = nullptr;

	switch (fileType)
	{
	case FileType::File_Scene:
		newFile = FileSystem::MakeFile("engine_assets\\empty_default\\scene.xen");
		break;
	case FileType::File_Code:
		newFile = FileSystem::MakeFile("engine_assets\\empty_default\\class.cpp");
		break;
	case FileType::File_Header:
		newFile = FileSystem::MakeFile("engine_assets\\empty_default\\class.h");
		break;
	case FileType::File_Skybox:
		newFile = FileSystem::MakeFile("engine_assets\\empty_default\\skybox.sky");
		break;
	case FileType::File_Material:
		newFile = FileSystem::MakeFile("engine_assets\\empty_default\\material.mat");
		break;
	case FileType::File_Shader:
		newFile = FileSystem::MakeFile("engine_assets\\empty_default\\standardShader.standardShader");
		break;
	}

	if (newFile && newFile->Open(FileMode::ReadOnly))
	{
		data = newFile->ReadAll();
		newFile->Close();
	}
	else
	{
		Debug::PrintError("[AssetManager::GetDefaultFileData] Default file not found", true);
	}

	return data;
}

#pragma endregion