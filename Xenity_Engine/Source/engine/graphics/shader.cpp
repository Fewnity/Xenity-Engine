// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "shader.h"

#define _USE_MATH_DEFINES
#if defined(__PSP__) || defined(__vita__)
#undef __STRICT_ANSI__
#endif
#include <cmath>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <json.hpp>

#include <engine/lighting/lighting.h>
#include <engine/engine.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>
#include <engine/debug/debug.h>
#include <engine/tools/string_tag_finder.h>
#include <engine/file_system/file.h>
#include <engine/game_elements/transform.h>
#include "graphics.h"
#include "renderer/renderer.h"
#include "camera.h"
#include <engine/application.h>
#include <engine/accessors/acc_gameobject.h>
#include <engine/debug/stack_debug_object.h>

#include <engine/graphics/shader_opengl.h>
#include <engine/graphics/shader_rsx.h>
#include <engine/graphics/shader_null.h>

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
#include <glad/glad.h>
#elif defined(__vita__)
#include <vitaGL.h>
#endif

using json = nlohmann::json;

glm::mat4 Shader::m_canvasCameraTransformationMatrix;

std::shared_ptr<Light> Shader::defaultDarkLight = std::make_shared<Light>();

void Shader::Init()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	defaultDarkLight->SetIntensity(0);
	m_canvasCameraTransformationMatrix = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
}

Shader::Shader()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);
}

/// <summary>
/// Shader destructor
/// </summary>
Shader::~Shader()
{
}

std::string Shader::GetShaderCode(ShaderType type, Platform platform) const
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	return GetShaderCode(ReadShader(), type, platform);
}

std::string Shader::GetShaderCode(const std::string& fullShaderCode, ShaderType type, Platform platform) const
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	struct TagData
	{
		std::string tag;
		int start = -1;
		int end = -1;
	};

	std::map<Platform, TagData> platformStartIndex;
	platformStartIndex[Platform::P_Windows].tag = "{pc}";
	platformStartIndex[Platform::P_Linux].tag = "{pc}";
	platformStartIndex[Platform::P_PsVita].tag = "{psvita}";
	platformStartIndex[Platform::P_PS3].tag = "{ps3}";

	for (auto& platformTagData : platformStartIndex)
	{
		platformTagData.second.start = fullShaderCode.find(platformTagData.second.tag);
		if (platformTagData.second.start != -1)
		{
			platformTagData.second.end += platformTagData.second.start + platformTagData.second.tag.size() + 1;
		}
	}

	if (platformStartIndex[platform].start == -1)
	{
		return "";
	}

	std::map<ShaderType, TagData> shaderStart;
	shaderStart[ShaderType::Vertex_Shader].tag = "{vertex}";
	shaderStart[ShaderType::Fragment_Shader].tag = "{fragment}";
	for (auto& shaderTagData : shaderStart)
	{
		shaderTagData.second.start = fullShaderCode.find(shaderTagData.second.tag, platformStartIndex[platform].end);
		if (shaderTagData.second.start != -1)
		{
			shaderTagData.second.end += shaderTagData.second.start + shaderTagData.second.tag.size() + 1;
		}
	}

	if (shaderStart[type].start == -1)
	{
		return "";
	}

	size_t endIndex = -1;
	for (auto& platformIndex : platformStartIndex)
	{
		if (platformIndex.second.start > shaderStart[type].start && endIndex > platformIndex.second.start && platformIndex.first != platform)
		{
			endIndex = platformIndex.second.start;
		}
	}
	for (auto& typeIndex : shaderStart)
	{
		if (typeIndex.second.start > shaderStart[type].start && endIndex > typeIndex.second.start && typeIndex.first != type)
		{
			endIndex = typeIndex.second.start;
		}
	}

	std::string result = fullShaderCode.substr(shaderStart[type].end, endIndex - shaderStart[type].end);

	return result;
}

std::string Shader::ReadShader() const
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	std::string shaderText = "";

#if defined(EDITOR)
	const bool openResult = m_file->Open(FileMode::ReadOnly);
	if (openResult)
	{
		shaderText = m_file->ReadAll();
		m_file->Close();
	}
#else
	unsigned char* binData = ProjectManager::fileDataBase.GetBitFile().ReadBinary(m_filePosition, m_fileSize);
	shaderText = std::string(reinterpret_cast<const char*>(binData), m_fileSize);
	free(binData);
#endif

	return shaderText;
}

unsigned char* Shader::ReadShaderBinary(size_t& size) const
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

#if defined(EDITOR)
	const bool openResult = m_file->Open(FileMode::ReadOnly);
	if (openResult)
	{
		unsigned char* binData = m_file->ReadAllBinary(size);
		m_file->Close();

		return binData;
	}
#else
	unsigned char* binData = ProjectManager::fileDataBase.GetBitFile().ReadBinary(m_filePosition, m_fileSize);
	size = m_fileSize;
	return binData;
#endif
}

ReflectiveData Shader::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

ReflectiveData Shader::GetMetaReflectiveData(AssetPlatform platform)
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

void Shader::LoadFileReference()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);
	
	if (m_fileStatus == FileStatus::FileStatus_Not_Loaded)
	{
		Load();
	}
}

std::shared_ptr<Shader> Shader::MakeShader()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

#if defined(__PS3__)
	std::shared_ptr<Shader> newFileRef = std::make_shared<ShaderRSX>();
#elif defined(_WIN32) || defined(_WIN64) || defined(__LINUX__) || defined(__vita__)
	std::shared_ptr<Shader> newFileRef = std::make_shared<ShaderOpenGL>();
#else
	std::shared_ptr<Shader> newFileRef = std::make_shared<ShaderNull>();
#endif
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}
