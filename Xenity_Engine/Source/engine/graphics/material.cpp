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
#include <engine/debug/stack_debug_object.h>

using json = nlohmann::json;

#pragma region Constructors / Destructor

//ProfilerBenchmark* materialUpdateBenchmark = new ProfilerBenchmark("Material update");

Material::Material()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	AssetManager::AddMaterial(this);
	AssetManager::AddReflection(this);
}

Material::~Material()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

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
	const bool matChanged = Graphics::s_currentMaterial != this;
	const bool cameraChanged = m_lastUsedCamera != Graphics::usedCamera.get();
	const bool drawTypeChanged = Graphics::s_currentMode != m_lastUpdatedType;

	if (matChanged || cameraChanged || drawTypeChanged)
	{
		Graphics::s_currentMaterial = this;
		SCOPED_PROFILER("Material::OnMaterialChanged", scopeBenchmark);
		if (m_shader && m_shader->GetFileStatus() == FileStatus::FileStatus_Loaded)
		{
			m_lastUsedCamera = Graphics::usedCamera.get();
			m_lastUpdatedType = Graphics::s_currentMode;

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
			Graphics::s_currentShader = nullptr;
		}
	}
}

/// <summary>
/// Update the material 
/// </summary>
void Material::Update()
{
	//materialUpdateBenchmark->Start();
	if (m_shader != nullptr && m_shader->GetFileStatus() == FileStatus::FileStatus_Loaded)
	{
		Performance::AddMaterialUpdate();

		//Send all uniforms
		if (!m_updated)
		{
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
	Reflective::AddVariable(reflectedVariables, m_renderingMode, "renderingMode", true);
	ReflectiveEntry& alphaCutoffReflectiveEntry = Reflective::AddVariable(reflectedVariables, m_alphaCutoff, "alphaCutoff", m_renderingMode == MaterialRenderingModes::Cutout);
	alphaCutoffReflectiveEntry.isSlider = true;
	alphaCutoffReflectiveEntry.minSliderValue = 0.0;
	alphaCutoffReflectiveEntry.maxSliderValue = 1.0;
	Reflective::AddVariable(reflectedVariables, m_texture, "texture", true);
	Reflective::AddVariable(reflectedVariables, m_color, "color", true);
	Reflective::AddVariable(reflectedVariables, t_offset, "offset", true);
	Reflective::AddVariable(reflectedVariables, t_tiling, "tiling", true);
	Reflective::AddVariable(reflectedVariables, m_useLighting, "useLighting", true);
	return reflectedVariables;
}

ReflectiveData Material::GetMetaReflectiveData([[maybe_unused]] AssetPlatform platform)
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

void Material::OnReflectionUpdated()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	// Call set functions to ensure that the values are correct
	SetAlphaCutoff(m_alphaCutoff);

#if defined(EDITOR)
	Graphics::s_isRenderingBatchDirty = true;

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
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	if (m_fileStatus == FileStatus::FileStatus_Not_Loaded)
	{
		m_fileStatus = FileStatus::FileStatus_Loading;

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
			unsigned char* binData = ProjectManager::fileDataBase.GetBitFile().ReadBinary(m_filePosition, m_fileSize);
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
				m_fileStatus = FileStatus::FileStatus_Failed;
				return;
			}
			ReflectionUtils::JsonToReflectiveData(j, GetReflectiveData());
			m_fileStatus = FileStatus::FileStatus_Loaded;
		}
		else
		{
			XASSERT(false, "[Material::LoadFileReference] Failed to load the material file: " + m_file->GetPath());
			Debug::PrintError("[Material::LoadFileReference] Failed to load the material file: " + m_file->GetPath(), true);
			m_fileStatus = FileStatus::FileStatus_Failed;
			return;
		}
	}
}
