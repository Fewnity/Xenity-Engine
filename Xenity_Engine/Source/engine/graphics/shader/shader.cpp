// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
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
#include <engine/graphics/graphics.h>
#include <engine/graphics/renderer/renderer.h>
#include <engine/graphics/camera.h>
#include <engine/application.h>
#include <engine/accessors/acc_gameobject.h>
#include <engine/debug/stack_debug_object.h>

#include <engine/graphics/shader/shader_opengl.h>
#include <engine/graphics/shader/shader_rsx.h>
#include <engine/graphics/shader/shader_null.h>

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
#include <glad/gl.h>
#elif defined(__vita__)
#include <vitaGL.h>
#endif

using json = nlohmann::json;

glm::mat4 Shader::m_canvasCameraTransformationMatrix;

std::shared_ptr<Light> Shader::defaultDarkLight;
std::vector<Shader::PointLightVariableNames> Shader::s_pointlightVariableNames;
std::vector<Shader::DirectionalLightsVariableNames> Shader::s_directionallightVariableNames;
std::vector<Shader::SpotLightVariableNames> Shader::s_spotlightVariableNames;

void Shader::Init()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);
	defaultDarkLight = std::make_shared<Light>();
	s_pointlightVariableNames.reserve(MAX_LIGHT_COUNT);
	s_directionallightVariableNames.reserve(MAX_LIGHT_COUNT);
	s_spotlightVariableNames.reserve(MAX_LIGHT_COUNT);
	for (int i = 0; i < MAX_LIGHT_COUNT; i++)
	{
		s_pointlightVariableNames.emplace_back(i);
		s_directionallightVariableNames.emplace_back(i);
		s_spotlightVariableNames.emplace_back(i);
	}

	defaultDarkLight->SetIntensity(0);
	m_canvasCameraTransformationMatrix = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
}

Shader::Shader()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);
	AssetManager::AddShader(this);
}

/// <summary>
/// Shader destructor
/// </summary>
Shader::~Shader()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);
	AssetManager::RemoveShader(this);
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
		size_t start = -1;
		size_t end = -1;
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

	return ReadString();
}

unsigned char* Shader::ReadShaderBinary(size_t& size) const
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	unsigned char* binData = ReadBinary(size);
	return binData;
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

void Shader::LoadFileReference(const LoadOptions& loadOptions)
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

Shader::PointLightVariableNames::PointLightVariableNames(int index)
{
	constexpr int bufferSize = 30;

	indices = new char[bufferSize];
	color = new char[bufferSize];
	position = new char[bufferSize];
	light_data = new char[bufferSize];

#if defined(_WIN32) || defined(_WIN64)
	sprintf_s(indices, bufferSize, "pointLightsIndices[%d]", index);
	sprintf_s(color, bufferSize, "pointLights[%d].color", index);
	sprintf_s(position, bufferSize, "pointLights[%d].position", index);
	sprintf_s(light_data, bufferSize, "pointLights[%d].light_data", index);
#else
	sprintf(color, "pointLights[%d].color", index);
	sprintf(position, "pointLights[%d].position", index);
	sprintf(light_data, "pointLights[%d].light_data", index);
#endif
}

Shader::PointLightVariableNames::~PointLightVariableNames()
{
	delete[] indices;
	delete[] color;
	delete[] position;
	delete[] light_data;
}

Shader::DirectionalLightsVariableNames::DirectionalLightsVariableNames(int index)
{
	constexpr int bufferSize = 35;

	indices = new char[bufferSize];
	color = new char[bufferSize];
	direction = new char[bufferSize];

#if defined(_WIN32) || defined(_WIN64)
	sprintf_s(indices, bufferSize, "directionalLightsIndices[%d]", index);
	sprintf_s(color, bufferSize, "directionalLights[%d].color", index);
	sprintf_s(direction, bufferSize, "directionalLights[%d].direction", index);
#else
	sprintf(color, "directionalLights[%d].color", index);
	sprintf(direction, "directionalLights[%d].direction", index);
#endif
}

Shader::DirectionalLightsVariableNames::~DirectionalLightsVariableNames()
{
	delete[] indices;
	delete[] color;
	delete[] direction;
}

Shader::SpotLightVariableNames::SpotLightVariableNames(int index)
{
	constexpr int bufferSize = 30;

	indices = new char[bufferSize];
	color = new char[bufferSize];
	position = new char[bufferSize];
	direction = new char[bufferSize];
	light_data = new char[bufferSize];

#if defined(_WIN32) || defined(_WIN64)
	sprintf_s(indices, bufferSize, "spotLightsIndices[%d]", index);
	sprintf_s(color, bufferSize, "spotLights[%d].color", index);
	sprintf_s(position, bufferSize, "spotLights[%d].position", index);
	sprintf_s(direction, bufferSize, "spotLights[%d].direction", index);
	sprintf_s(light_data, bufferSize, "spotLights[%d].light_data", index);
#else
	sprintf(color, "spotLights[%d].color", index);
	sprintf(position, "spotLights[%d].position", index);
	sprintf(direction, "spotLights[%d].direction", index);
	sprintf(light_data, "spotLights[%d].light_data", index);
#endif
}

Shader::SpotLightVariableNames::~SpotLightVariableNames()
{
	delete[] indices;
	delete[] color;
	delete[] position;
	delete[] direction;
	delete[] light_data;
}