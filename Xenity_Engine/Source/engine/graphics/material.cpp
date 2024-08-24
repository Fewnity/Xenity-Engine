// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "material.h"

#include <json.hpp>


#include <engine/asset_management/asset_manager.h>
#include <engine/file_system/file.h>
#include <engine/engine.h>
#include <engine/debug/debug.h>
#include <engine/debug/performance.h>
#include <engine/reflection/reflection_utils.h>
#include <engine/tools/scope_benchmark.h>
#include "graphics.h"
#include "renderer/renderer.h"

using json = nlohmann::json;

#pragma region Constructors / Destructor

//ProfilerBenchmark* materialUpdateBenchmark = new ProfilerBenchmark("Material update");

Material::Material()
{
	SetAttribute("color", Vector3(1, 1, 1));
	AssetManager::AddMaterial(this);
	AssetManager::AddReflection(this);
}

Material::~Material()
{
	AssetManager::RemoveMaterial(this);
	AssetManager::RemoveReflection(this);
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
	XASSERT(strlen(attribute) != 0, "[Material::SetAttribute] attribute name is empty");
	m_uniformsVector2.emplace(std::pair <const char*, Vector2>(attribute, value));
}

/// <summary>
/// Add a Vector3 attribute
/// </summary>
/// <param name="attribute">Attribute name</param>
/// <param name="value">Vector3</param>
void Material::SetAttribute(const char* attribute, const Vector3& value)
{
	XASSERT(strlen(attribute) != 0, "[Material::SetAttribute] attribute name is empty");
	m_uniformsVector3.emplace(std::pair <const char*, Vector3>(attribute, value));
}

/// <summary>
/// Add a Vector4 attribut
/// </summary>
/// <param name="attribute">Attribute name</param>
/// <param name="value">Vector4</param>
void Material::SetAttribute(const char* attribute, const Vector4& value)
{
	XASSERT(strlen(attribute) != 0, "[Material::SetAttribute] attribute name is empty");
	m_uniformsVector4.emplace(std::pair <const char*, Vector4>(attribute, value));
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
	XASSERT(strlen(attribute) != 0, "[Material::SetAttribute] attribute name is empty");
	m_uniformsFloat.emplace(std::pair <const char*, float>(attribute, value));
}

/// <summary>
/// Add an int attribute
/// </summary>
/// <param name="attribute">Attribute name</param>
/// <param name="value">int</param>
void Material::SetAttribute(const char* attribute, const int value)
{
	XASSERT(strlen(attribute) != 0, "[Material::SetAttribute] attribute name is empty");
	m_uniformsInt.emplace(std::pair <const char*, int>(attribute, value));
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
	const bool cameraChanged = m_lastUsedCamera.lock() != Graphics::usedCamera;
	const bool drawTypeChanged = Graphics::currentMode != m_lastUpdatedType;

	if (matChanged || cameraChanged || drawTypeChanged)
	{
		Graphics::currentMaterial = std::dynamic_pointer_cast<Material>(shared_from_this());
		SCOPED_PROFILER("Material::OnMaterialChanged", scopeBenchmark);
		if (m_shader)
		{
			m_lastUsedCamera = Graphics::usedCamera;
			m_lastUpdatedType = Graphics::currentMode;

			m_shader->Use();
			Update();

			const int matCount = AssetManager::GetMaterialCount();
			for (int i = 0; i < matCount; i++)
			{
				Material* mat = AssetManager::GetMaterial(i);
				if (mat->m_shader == m_shader && mat != this)
				{
					mat->m_updated = false;
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
	if (m_shader != nullptr && m_shader->m_isLoaded)
	{
		Performance::AddMaterialUpdate();

		//Send all uniforms

		{
			//ScopeBenchmark scopeBenchmark = ScopeBenchmark("Material::UpdateProj");
			if (Graphics::currentMode == IDrawableTypes::Draw_UI)
			{
				m_shader->SetShaderCameraPositionCanvas();
				m_shader->SetShaderProjectionCanvas();
			}
			else
			{
				m_shader->SetShaderCameraPosition();
				m_shader->SetShaderProjection();
			}
		}

		if (!m_updated)
		{
			m_shader->UpdateLights(m_useLighting);

			m_shader->SetShaderAttribut("tiling", t_tiling);
			m_shader->SetShaderAttribut("offset", t_offset);

			//int textureIndex = 0;
			/*for (const auto& kv : uniformsTextures)
			{
				//Enable each textures units
				Engine::renderer->EnableTextureUnit(textureIndex);
				Engine::renderer->BindTexture(kv.second);
				shader->SetShaderAttribut(kv.first, textureIndex);
				textureIndex++;
			}*/
			for (const auto& kv : m_uniformsVector2)
			{
				m_shader->SetShaderAttribut(kv.first, kv.second);
			}
			for (const auto& kv : m_uniformsVector3)
			{
				m_shader->SetShaderAttribut(kv.first, kv.second);
			}
			for (auto& kv : m_uniformsVector4)
			{
				m_shader->SetShaderAttribut(kv.first, kv.second);
			}
			for (const auto& kv : m_uniformsInt)
			{
				m_shader->SetShaderAttribut(kv.first, kv.second);
			}
			for (const auto& kv : m_uniformsFloat)
			{
				m_shader->SetShaderAttribut(kv.first, kv.second);
			}

			m_updated = true;
		}
	}
	//materialUpdateBenchmark->Stop();
}

ReflectiveData Material::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, m_shader, "shader", true);
	Reflective::AddVariable(reflectedVariables, m_texture, "texture", true);
	Reflective::AddVariable(reflectedVariables, m_color, "color", true);
	Reflective::AddVariable(reflectedVariables, t_offset, "offset", true);
	Reflective::AddVariable(reflectedVariables, t_tiling, "tiling", true);
	Reflective::AddVariable(reflectedVariables, m_useLighting, "useLighting", true);
	Reflective::AddVariable(reflectedVariables, m_useTransparency, "useTransparency", true);
	return reflectedVariables;
}

ReflectiveData Material::GetMetaReflectiveData(AssetPlatform platform)
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

void Material::OnReflectionUpdated()
{
#if defined(EDITOR)
	json jsonData;
	jsonData["Values"] = ReflectionUtils::ReflectiveDataToJson(GetReflectiveData());
	jsonData["Version"] = s_version;

	const bool saveResult = ReflectionUtils::JsonToFile(jsonData, m_file);
	if (!saveResult)
	{
		XASSERT(false, "[Material::OnReflectionUpdated] Failed to save the material file: " + m_file->GetPath());
		Debug::PrintError("[Material::OnReflectionUpdated] Fail to save the Material file: " + m_file->GetPath(), true);
	}
#endif
}

void Material::LoadFileReference()
{
	if (!m_isLoaded)
	{
		m_isLoaded = true;

		bool loadResult = true;
#if defined(EDITOR)
		loadResult = m_file->Open(FileMode::ReadOnly);
#endif
		if (loadResult)
		{
			std::string jsonString;
#if defined(EDITOR)
			jsonString = m_file->ReadAll();
			m_file->Close();
#else
			unsigned char* binData = ProjectManager::fileDataBase.bitFile.ReadBinary(m_filePosition, m_fileSize);
			jsonString = std::string(reinterpret_cast<const char*>(binData), m_fileSize);
			free(binData);
#endif

			json j;
			try
			{
				j = json::parse(jsonString);
			}
			catch (const std::exception&)
			{
				Debug::PrintError("[ProjectManager::LoadFileReference] Failed to load the material file", true);
				return;
			}
			ReflectionUtils::JsonToReflectiveData(j, GetReflectiveData());

		}
		else
		{
			XASSERT(false, "[Material::LoadFileReference] Failed to load the material file: " + m_file->GetPath());
			Debug::PrintError("[Material::LoadFileReference] Failed to load the material file: " + m_file->GetPath(), true);
		}
	}
}
