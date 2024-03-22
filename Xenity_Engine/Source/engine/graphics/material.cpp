#include "material.h"

#include "graphics.h"
#include "renderer/renderer.h"

#include <engine/asset_management/asset_manager.h>
#include <engine/file_system/file.h>

#include <engine/engine.h>
#include <engine/debug/debug.h>
#include <engine/debug/performance.h>

#include <engine/reflection/reflection_utils.h>

#include <iostream>
#include <json.hpp>


using json = nlohmann::json;

#pragma region Constructors / Destructor

//ProfilerBenchmark* materialUpdateBenchmark = new ProfilerBenchmark("Material update");

Material::Material()
{
	SetAttribute("color", Vector3(1, 1, 1));
	AssetManager::AddMaterial(this);
}

Material::~Material()
{
	AssetManager::RemoveMaterial(this);
}

#pragma endregion

#pragma region Attributs setters

/// <summary>
/// Add a Vector2 attribute
/// </summary>
/// <param name="attribute">Attribute name</param>
/// <param name="value">Vector2</param>
void Material::SetAttribute(const char* attribute, const Vector2& value)
{
	uniformsVector2.emplace(std::pair <const char*, Vector2>(attribute, value));
}

/// <summary>
/// Add a Vector3 attribute
/// </summary>
/// <param name="attribute">Attribute name</param>
/// <param name="value">Vector3</param>
void Material::SetAttribute(const char* attribute, const Vector3& value)
{
	uniformsVector3.emplace(std::pair <const char*, Vector3>(attribute, value));
}

/// <summary>
/// Add a Vector4 attribut
/// </summary>
/// <param name="attribute">Attribute name</param>
/// <param name="value">Vector4</param>
void Material::SetAttribute(const char* attribute, const Vector4& value)
{
	uniformsVector4.emplace(std::pair <const char*, Vector4>(attribute, value));
}

/// <summary>
/// Add a Texture attribute
/// </summary>
/// <param name="attribute">Attribute name</param>
/// <param name="value">Texture pointer</param>
//void Material::SetAttribute(const char* attribute, Texture* value)
//{
//	uniformsTextures.insert(std::pair <const char*, Texture*>(attribute, value));
//}

/// <summary>
/// Add a float attribute
/// </summary>
/// <param name="attribute">Attribute name</param>
/// <param name="value">float</param>
void Material::SetAttribute(const char* attribute, const float value)
{
	uniformsFloat.emplace(std::pair <const char*, float>(attribute, value));
}

/// <summary>
/// Add an int attribute
/// </summary>
/// <param name="attribute">Attribute name</param>
/// <param name="value">int</param>
void Material::SetAttribute(const char* attribute, const int value)
{
	uniformsInt.emplace(std::pair <const char*, int>(attribute, value));
}

std::shared_ptr<Material> Material::MakeMaterial()
{
	std::shared_ptr<Material> newFileRef = std::make_shared<Material>();
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}

#pragma endregion

/// <summary>
/// Use the material to draw something
/// </summary>
void Material::Use()
{
	const bool matChanged = Graphics::currentMaterial != shared_from_this();
	const bool cameraChanged = lastUsedCamera.lock() != Graphics::usedCamera.lock();
	const bool drawTypeChanged = Graphics::currentMode != lastUpdatedType;

	//if(true)
	if (matChanged || cameraChanged || drawTypeChanged)
	{
		Graphics::currentMaterial = std::dynamic_pointer_cast<Material>(shared_from_this());
		if (shader)
		{
			lastUsedCamera = Graphics::usedCamera;
			lastUpdatedType = Graphics::currentMode;
			shader->Use();
			Update();

			const int matCount = AssetManager::GetMaterialCount();
			for (int i = 0; i < matCount; i++)
			{
				Material* mat = AssetManager::GetMaterial(i);
				if (mat->shader == shader && mat != this)
				{
					mat->updated = false;
				}
			}
		}
		else 
		{
			Engine::GetRenderer().UseShaderProgram(0);
			Graphics::currentShader = nullptr;
		}
	}
}

/// <summary>
/// Update the material 
/// </summary>
void Material::Update()
{
	//materialUpdateBenchmark->Start();
	if (shader != nullptr)
	{
		Performance::AddMaterialUpdate();

		//Send all uniforms
		if (Graphics::currentMode == IDrawableTypes::Draw_UI)
		{
			shader->SetShaderCameraPositionCanvas();
			shader->SetShaderProjectionCanvas();
		}
		else
		{
			shader->SetShaderCameraPosition();
			shader->SetShaderProjection();
		}

		if (!updated)
		{
			if (useLighting)
				shader->UpdateLights();

			int textureIndex = 0;
			/*for (const auto& kv : uniformsTextures)
			{
				//Enable each textures units
				Engine::renderer->EnableTextureUnit(textureIndex);
				Engine::renderer->BindTexture(kv.second);
				shader->SetShaderAttribut(kv.first, textureIndex);
				textureIndex++;
			}*/
			for (const auto& kv : uniformsVector2)
			{
				shader->SetShaderAttribut(kv.first, kv.second);
			}
			for (const auto& kv : uniformsVector3)
			{
				shader->SetShaderAttribut(kv.first, kv.second);
			}
			for (auto& kv : uniformsVector4)
			{
				shader->SetShaderAttribut(kv.first, kv.second);
			}
			for (const auto& kv : uniformsInt)
			{
				shader->SetShaderAttribut(kv.first, kv.second);
			}
			for (const auto& kv : uniformsFloat)
			{
				shader->SetShaderAttribut(kv.first, kv.second);
			}

			updated = true;
		}
	}
	//materialUpdateBenchmark->Stop();
}

ReflectiveData Material::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, shader, "shader", true);
	Reflective::AddVariable(reflectedVariables, useLighting, "useLighting", true);
	Reflective::AddVariable(reflectedVariables, useTransparency, "useTransparency", true);
	return reflectedVariables;
}

ReflectiveData Material::GetMetaReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

void Material::OnReflectionUpdated()
{
	const bool loadResult = ReflectionUtils::ReflectiveDataToFile(GetReflectiveData(), file);
	if (!loadResult)
	{
		Debug::PrintError("[Material::OnReflectionUpdated] Fail to save the Material file: " + file->GetPath(), true);
	}
}

void Material::LoadFileReference()
{
	const bool loadResult = ReflectionUtils::FileToReflectiveData(file, GetReflectiveData());
	if (!loadResult)
	{
		Debug::PrintError("[Material::LoadFileReference] Fail to load the material file: " + file->GetPath(), true);
	}
}
