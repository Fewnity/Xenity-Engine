#include "material.h"
#include "../../xenity.h"
#include "../graphics/renderer/renderer.h"

#include <iostream>
#include <json.hpp>
#include "../reflection/reflection_utils.h"

using json = nlohmann::json;

#pragma region Constructors / Destructor

//ProfilerBenchmark* materialUpdateBenchmark = new ProfilerBenchmark("Material update");

Material::Material()
{
	SetAttribut("color", Vector3(1, 1, 1));
	AssetManager::AddMaterial(this);
}

Material::~Material()
{
	AssetManager::RemoveMaterial(this);
}

#pragma endregion

#pragma region Attributs setters

/// <summary>
/// Add a Vector2 attribut
/// </summary>
/// <param name="attribut">Attribut name</param>
/// <param name="value">Vector2</param>
void Material::SetAttribut(const char* attribut, const Vector2 value)
{
	uniformsVector2.insert(std::pair <const char*, Vector2>(attribut, value));
}

/// <summary>
/// Add a Vector3 attribut
/// </summary>
/// <param name="attribut">Attribut name</param>
/// <param name="value">Vector3</param>
void Material::SetAttribut(const char* attribut, const Vector3 value)
{
	uniformsVector3.insert(std::pair <const char*, Vector3>(attribut, value));
}

/// <summary>
/// Add a Vector4 attribut
/// </summary>
/// <param name="attribut">Attribut name</param>
/// <param name="value">Vector4</param>
void Material::SetAttribut(const char* attribut, const Vector4 value)
{
	uniformsVector4.insert(std::pair <const char*, Vector4>(attribut, value));
}

/// <summary>
/// Add a Texture attribut
/// </summary>
/// <param name="attribut">Attribut name</param>
/// <param name="value">Texture pointer</param>
//void Material::SetAttribut(const char* attribut, Texture* value)
//{
//	uniformsTextures.insert(std::pair <const char*, Texture*>(attribut, value));
//}

/// <summary>
/// Add a float attribut
/// </summary>
/// <param name="attribut">Attribut name</param>
/// <param name="value">float</param>
void Material::SetAttribut(const char* attribut, const float value)
{
	uniformsFloat.insert(std::pair <const char*, float>(attribut, value));
}

/// <summary>
/// Add an int attribut
/// </summary>
/// <param name="attribut">Attribut name</param>
/// <param name="value">int</param>
void Material::SetAttribut(const char* attribut, const int value)
{
	uniformsInt.insert(std::pair <const char*, int>(attribut, value));
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
	bool matChanged = Graphics::currentMaterial != shared_from_this();
	bool cameraChanged = lastUsedCamera.lock() != Graphics::usedCamera.lock();
	bool drawTypeChanged = Graphics::currentMode != lastUpdatedType;

	if (matChanged || cameraChanged || drawTypeChanged)
	{
		lastUsedCamera = Graphics::usedCamera;
		lastUpdatedType = Graphics::currentMode;
		Graphics::currentMaterial = std::dynamic_pointer_cast<Material>(shared_from_this());
		shader->Use();
		Update();

		int matCount = AssetManager::GetMaterialCount();
		for (int i = 0; i < matCount; i++)
		{
			Material* mat = AssetManager::GetMaterial(i);
			if (mat->shader == shader && mat != this)
			{
				mat->updated = false;
			}
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
		if (Graphics::currentMode == Draw_UI)
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

std::unordered_map<std::string, ReflectionEntry> Material::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	Reflection::AddReflectionVariable(reflectedVariables, shader, "shader", true);
	Reflection::AddReflectionVariable(reflectedVariables, useLighting, "useLighting", true);
	return reflectedVariables;
}

std::unordered_map<std::string, ReflectionEntry> Material::GetMetaReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	return reflectedVariables;
}

void Material::OnReflectionUpdated()
{
	json myJson;
	myJson["Values"] = ReflectionUtils::MapToJson(GetReflection());
	FileSystem::fileSystem->DeleteFile(file->GetPath());
	file->Open(true);
	file->Write(myJson.dump(0));
	file->Close();
}

void Material::LoadFileReference()
{
	json myJson;
	file->Open(true);
	std::string jsonString = file->ReadAll();
	file->Close();
	if (jsonString != "")
	{
		myJson = json::parse(jsonString);
		ReflectionUtils::JsonToMap(myJson, GetReflection());
	}
}
