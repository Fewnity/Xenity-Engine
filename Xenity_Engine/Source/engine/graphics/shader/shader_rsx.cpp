// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(__PS3__)

#include "shader_rsx.h"

#include <cstring>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <rsx/rsx.h>

#include <engine/graphics/graphics.h>
#include <engine/application.h>
#include <engine/file_system/file.h>
#include <engine/debug/debug.h>
#include <engine/engine.h>
#include <engine/graphics/camera.h>
#include <engine/graphics/renderer/renderer_rsx.h>
#include <engine/game_elements/transform.h>
#include <engine/game_elements/gameobject.h>
#include <engine/lighting/lighting.h>
#include <engine/debug/performance.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/tools/endian_utils.h>

ShaderRSX::PointLightVariableIds::PointLightVariableIds(int index, rsxFragmentProgram* program)
{
	color = rsxFragmentProgramGetConst(program, s_pointlightVariableNames[index].color);
	position = rsxFragmentProgramGetConst(program, s_pointlightVariableNames[index].position);
	light_data = rsxFragmentProgramGetConst(program, s_pointlightVariableNames[index].light_data);
}

ShaderRSX::DirectionalLightsVariableIds::DirectionalLightsVariableIds(int index, rsxFragmentProgram* program)
{
	color = rsxFragmentProgramGetConst(program, s_directionallightVariableNames[index].color);
	direction = rsxFragmentProgramGetConst(program, s_directionallightVariableNames[index].direction);
}

ShaderRSX::SpotLightVariableIds::SpotLightVariableIds(int index, rsxFragmentProgram* program)
{
	color = rsxFragmentProgramGetConst(program, s_spotlightVariableNames[index].color);
	position = rsxFragmentProgramGetConst(program, s_spotlightVariableNames[index].position);
	direction = rsxFragmentProgramGetConst(program, s_spotlightVariableNames[index].direction);
	constant = rsxFragmentProgramGetConst(program, s_spotlightVariableNames[index].constant);
	linear = rsxFragmentProgramGetConst(program, s_spotlightVariableNames[index].linear);
	quadratic = rsxFragmentProgramGetConst(program, s_spotlightVariableNames[index].quadratic);
	cutOff = rsxFragmentProgramGetConst(program, s_spotlightVariableNames[index].cutOff);
	outerCutOff = rsxFragmentProgramGetConst(program, s_spotlightVariableNames[index].outerCutOff);
}

ShaderRSX::~ShaderRSX()
{

}

void ShaderRSX::Init()
{

}

void ShaderRSX::Load()
{
	size_t size = 0;
	unsigned char* fullShader = ReadShaderBinary(size);
	//Debug::Print("size: " + std::to_string(size));

	// Read vertex shader code size
	uint32_t vertexShaderCodeSize = 0;
	memcpy(&vertexShaderCodeSize, fullShader, sizeof(uint32_t));
	vertexShaderCodeSize = EndianUtils::SwapEndian(vertexShaderCodeSize);
	fullShader += sizeof(uint32_t);
	//Debug::Print("vertexShaderCodeSize: " + std::to_string(vertexShaderCodeSize));

	// Read code
	m_vertexProgram = (rsxVertexProgram*)fullShader;

	// Check magic numbers and if the file is corrupted, the vertex shader size may be greater than the full shader size
	if (((char*)m_vertexProgram)[0] != 'V' || ((char*)m_vertexProgram)[1] != 'P' || vertexShaderCodeSize >= size)
	{
		Debug::PrintError("Vertex program corrupted!");
		m_fileStatus = FileStatus::FileStatus_Failed;
		return;
	}
	fullShader += vertexShaderCodeSize;

	// Read fragment shader code size
	uint32_t fragmentShaderCodeSize = 0;
	memcpy(&fragmentShaderCodeSize, fullShader, sizeof(uint32_t));
	fragmentShaderCodeSize = EndianUtils::SwapEndian(fragmentShaderCodeSize);
	fullShader += sizeof(uint32_t);
	//Debug::Print("fragmentShaderCodeSize: " + std::to_string(fragmentShaderCodeSize));

	m_fragmentProgram = (rsxFragmentProgram*)fullShader;

	// Check magic numbers and if the file is corrupted, the fragment shader size may be greater than the full shader size
	if (((char*)m_fragmentProgram)[0] != 'F' || ((char*)m_fragmentProgram)[1] != 'P' || fragmentShaderCodeSize >= size)
	{
		Debug::PrintError("Fragment program corrupted!");
		m_fileStatus = FileStatus::FileStatus_Failed;
		return;
	}

	{
		rsxVertexProgramGetUCode(m_vertexProgram, &m_vertexProgramCode, &m_vertexProgramSize);
		//printf("vertexProgramSize: %d\n", m_vertexProgramSize);

		m_projMatrix = rsxVertexProgramGetConst(m_vertexProgram, "projection");
		m_viewMatrix = rsxVertexProgramGetConst(m_vertexProgram, "camera");
		m_modelMatrix = rsxVertexProgramGetConst(m_vertexProgram, "model");
		m_MVPMatrix = rsxVertexProgramGetConst(m_vertexProgram, "MVP");
		m_normalMatrix = rsxVertexProgramGetConst(m_vertexProgram, "normalMatrix");
	}

	{
		rsxFragmentProgramGetUCode(m_fragmentProgram, &m_fragmentProgramCode, &m_fragmentProgramSize);
		//printf("fragmentProgramSize: %d\n", m_fragmentProgramSize);

		m_fragmentProgramCodeOnGPU = (uint32_t*)rsxMemalign(64, m_fragmentProgramSize);
		memcpy(m_fragmentProgramCodeOnGPU, m_fragmentProgramCode, m_fragmentProgramSize);
		rsxAddressToOffset(m_fragmentProgramCodeOnGPU, &m_fp_offset);

		m_color = rsxFragmentProgramGetConst(m_fragmentProgram, "color");
		m_textureUnit = rsxFragmentProgramGetAttrib(m_fragmentProgram, "texture");
		m_lightingDataTextureUnit = rsxFragmentProgramGetAttrib(m_fragmentProgram, "lightingDataTexture");
		m_ambientLightLocation = rsxFragmentProgramGetConst(m_fragmentProgram, "ambientLight");
		m_usedPointLightCount = rsxFragmentProgramGetConst(m_fragmentProgram, "usedPointLightCount");
		m_tilingLocation = rsxFragmentProgramGetConst(m_fragmentProgram, "tiling");
		m_offsetLocation = rsxFragmentProgramGetConst(m_fragmentProgram, "offset");

		m_pointlightVariableIds.reserve(MAX_LIGHT_COUNT);
		m_directionallightVariableIds.reserve(MAX_LIGHT_COUNT);
		m_spotlightVariableIds.reserve(MAX_LIGHT_COUNT);
		for (int i = 0; i < MAX_LIGHT_COUNT; i++)
		{
			m_pointlightVariableIds.emplace_back(i, m_fragmentProgram);
			m_directionallightVariableIds.emplace_back(i, m_fragmentProgram);
			m_spotlightVariableIds.emplace_back(i, m_fragmentProgram);
		}
#if defined(DEBUG)
		Debug::Print("----------- FRAGMENT SHADER DEBUG -----------");
		Debug::Print("num_regs: " + std::to_string(m_fragmentProgram->num_regs));
		Debug::Print("Attributs count: " + std::to_string(m_fragmentProgram->num_attr));
		Debug::Print("Constants count: " + std::to_string(m_fragmentProgram->num_const));
#endif
		/*rsxProgramConst* consts = rsxFragmentProgramGetConsts(m_fragmentProgram);
		for (size_t i = 0; i < m_fragmentProgram->num_const; i++)
		{
			Debug::Print("Type" + std::to_string(consts[i].type));
		}*/
	}

	m_fileStatus = FileStatus::FileStatus_Loaded;
}

bool ShaderRSX::Use()
{
	rsxLoadFragmentProgramLocation(RendererRSX::context, m_fragmentProgram, m_fp_offset, GCM_LOCATION_RSX);
	if (Graphics::s_currentShader != this)
	{
		rsxLoadVertexProgram(RendererRSX::context, m_vertexProgram, m_vertexProgramCode);
		Graphics::s_currentShader = this;
		return true;
	}
	return false;
}

bool ShaderRSX::Compile(const std::string& shaderData, ShaderType type)
{
	return true;
}

#pragma endregion

#pragma region Uniform setters

/// <summary>
/// Send to the shader the 3D camera position
/// </summary>
void ShaderRSX::SetShaderCameraPosition()
{
}

/// <summary>
/// Send to the shader the 2D camera position
/// </summary>
void ShaderRSX::SetShaderCameraPositionCanvas()
{
}

/// <summary>
/// Send to the shader the 2D camera projection
/// </summary>
void ShaderRSX::SetShaderProjection()
{
}

void ShaderRSX::SetShaderProjectionCanvas()
{
}

/// <summary>
/// Send to the shader transform's model
/// </summary>
/// <param name="trans"></param>
void ShaderRSX::SetShaderModel(const glm::mat4& trans, const glm::mat3& normalMatrix, const glm::mat4& mvpMatrix)
{
	if (m_modelMatrix)
	{
		rsxSetVertexProgramParameter(RendererRSX::context, m_vertexProgram, m_modelMatrix, (float*)&trans);
	}
	if (m_MVPMatrix)
	{
		//const glm::mat4 MVP = Graphics::usedCamera->m_viewProjectionMatrix * trans;
		rsxSetVertexProgramParameter(RendererRSX::context, m_vertexProgram, m_MVPMatrix, (float*)&mvpMatrix);
	}
	if (m_normalMatrix)
	{
		rsxSetVertexProgramParameter(RendererRSX::context, m_vertexProgram, m_normalMatrix, (float*)&normalMatrix);
	}
}

/// <summary>
/// Send to the shader transform's model
/// </summary>
/// <param name="trans"></param>
void ShaderRSX::SetShaderModel(const Vector3& position, const Vector3& rotation, const Vector3& scale)
{
	//Engine::GetRenderer().SetShaderAttribut(m_programId, it->second, value);
}

void ShaderRSX::SetShaderOffsetAndTiling(const Vector2& offset, const Vector2& tiling)
{
	rsxSetFragmentProgramParameter(RendererRSX::context, m_fragmentProgram, m_tilingLocation, (float*)&tiling.x, m_fp_offset, GCM_LOCATION_RSX);
	rsxSetFragmentProgramParameter(RendererRSX::context, m_fragmentProgram, m_offsetLocation, (float*)&offset.x, m_fp_offset, GCM_LOCATION_RSX);
}

void ShaderRSX::SetLightIndices(const LightsIndices& lightsIndices)
{
	//for (int i = 0; i < MAX_LIGHT_COUNT; i++)
	//{
	//	rsxSetFragmentProgramParameter(RendererRSX::context, m_fragmentProgram, m_directionalLightIndicesLocations[], (float*)&offset.x, m_fp_offset, GCM_LOCATION_RSX);

	//	/*std::string directionalIndexStr = "directionalLightsIndices[" + std::to_string(i) + "]";
	//	SetShaderAttribut(directionalIndexStr, (float)lightsIndices.directionalLightIndices[i].x);

	//	std::string pointIndexStr = "pointLightsIndices[" + std::to_string(i) + "]";
	//	SetShaderAttribut(pointIndexStr, (float)lightsIndices.pointLightIndices[i].x);*/
	//}
	//return;
	size_t offset = 1;

	Vector4 ambientLight = Vector4(0, 0, 0, 0);
	int directionalUsed = 0;
	int pointUsed = 0;
	int spotUsed = 0;
	size_t lightCount = m_currentLights.size();
	for (size_t i = 0; i < lightCount; i++)
	{
		Light& light = *m_currentLights[i];
		if (light.m_type == LightType::Point)
		{
			SetPointLightData(light, pointUsed + offset);
			pointUsed++;
		}
		else if (light.m_type == LightType::Spot)
		{
			SetSpotLightData(light, spotUsed + offset);
			spotUsed++;
		}
	}

	const int totalLightCount = AssetManager::GetLightCount();

	//For each lights
	for (int lightI = 0; lightI < totalLightCount; lightI++)
	{
		const Light& light = *AssetManager::GetLight(lightI);
		if (light.IsEnabled() && light.GetGameObjectRaw()->IsLocalActive())
		{
			if (light.m_type == LightType::Ambient)
			{
				ambientLight += light.color.GetRGBA().ToVector4() * light.m_intensity;
			}
		}
	}

	if (m_ambientLightLocation)
	{
		SetAmbientLightData(Vector3(ambientLight.x, ambientLight.y, ambientLight.z));
	}

	for (size_t i = 0; i < lightsIndices.usedDirectionalLightCount; i++)
	{
		Light& light = *m_currentDirectionalLights[i];
		SetDirectionalLightData(light, directionalUsed + offset);
		directionalUsed++;
	}

	for (size_t i = 0; i < 2; i++)
	{
		if (i >= pointUsed)
		{
			SetPointLightData(*defaultDarkLight, static_cast<int>(i + offset));
		}
	}

	for (size_t i = 0; i < 2; i++)
	{
		if (i >= spotUsed)
		{
			SetSpotLightData(*defaultDarkLight, static_cast<int>(i + offset));
		}
	}

	for (size_t i = 0; i < 2; i++)
	{
		if (i >= directionalUsed)
		{
			SetDirectionalLightData(*defaultDarkLight, static_cast<int>(i + offset));
		}
	}
}

ShaderRSX::RsxProgramConstPair* ShaderRSX::FindOrAddAttributId(const std::string& attribut)
{
	auto it = m_uniformsIds.find(attribut);
	if (it == m_uniformsIds.end())
	{
		RsxProgramConstPair rsxProgramConstPair;
		rsxProgramConst* vPConst = rsxVertexProgramGetConst(m_vertexProgram, attribut.c_str());
		if (vPConst)
		{
			rsxProgramConstPair.programConst = vPConst;
			rsxProgramConstPair.isVertexConst = true;
		}
		else
		{
			rsxProgramConst* fPConst = rsxFragmentProgramGetConst(m_fragmentProgram, attribut.c_str());
			if (fPConst)
			{
				rsxProgramConstPair.programConst = fPConst;
				rsxProgramConstPair.isVertexConst = false;
			}
			else
			{
				return nullptr;
			}
		}
		it = m_uniformsIds.emplace(attribut, rsxProgramConstPair).first;
	}

	return &it->second;
}

void ShaderRSX::SetShaderAttribut(const std::string& attribut, const Vector4& value)
{
	RsxProgramConstPair* attributId = FindOrAddAttributId(attribut);
	if (!attributId)
	{
		return;
	}

	if (attributId->isVertexConst)
	{
		rsxSetVertexProgramParameter(RendererRSX::context, m_vertexProgram, attributId->programConst, (float*)&value.x);
	}
	else
	{
		rsxSetFragmentProgramParameter(RendererRSX::context, m_fragmentProgram, attributId->programConst, (float*)&value.x, m_fp_offset, GCM_LOCATION_RSX);
	}
}

void ShaderRSX::SetShaderAttribut(const std::string& attribut, const Vector3& value)
{
	RsxProgramConstPair* attributId = FindOrAddAttributId(attribut);
	if (!attributId)
	{
		return;
	}

	if (attributId->isVertexConst)
	{
		rsxSetVertexProgramParameter(RendererRSX::context, m_vertexProgram, attributId->programConst, (float*)&value.x);
	}
	else
	{
		rsxSetFragmentProgramParameter(RendererRSX::context, m_fragmentProgram, attributId->programConst, (float*)&value.x, m_fp_offset, GCM_LOCATION_RSX);
	}
}

void ShaderRSX::SetShaderAttribut(const std::string& attribut, const Vector2& value)
{
	RsxProgramConstPair* attributId = FindOrAddAttributId(attribut);
	if (!attributId)
	{
		return;
	}

	if (attributId->isVertexConst)
	{
		rsxSetVertexProgramParameter(RendererRSX::context, m_vertexProgram, attributId->programConst, (float*)&value.x);
	}
	else
	{
		rsxSetFragmentProgramParameter(RendererRSX::context, m_fragmentProgram, attributId->programConst, (float*)&value.x, m_fp_offset, GCM_LOCATION_RSX);
	}
}

void ShaderRSX::SetShaderAttribut(const std::string& attribut, float value)
{
	RsxProgramConstPair* attributId = FindOrAddAttributId(attribut);
	if (!attributId)
	{
		return;
	}

	if (attributId->isVertexConst)
	{
		rsxSetVertexProgramParameter(RendererRSX::context, m_vertexProgram, attributId->programConst, (float*)&value);
	}
	else
	{
		rsxSetFragmentProgramParameter(RendererRSX::context, m_fragmentProgram, attributId->programConst, (float*)&value, m_fp_offset, GCM_LOCATION_RSX);
	}
}

void ShaderRSX::SetShaderAttribut(const std::string& attribut, int value)
{
	/*RsxProgramConstPair* attributId = FindOrAddAttributId(attribut);
	if (!attributId)
	{
		return;
	}

	if (attributId->isVertexConst)
	{
		rsxSetVertexProgramParameter(RendererRSX::context, m_vertexProgram, attributId->programConst, (float*)&value);
	}
	else
	{
		rsxSetFragmentProgramParameter(RendererRSX::context, m_fragmentProgram, attributId->programConst, (float*)&value, m_fp_offset, GCM_LOCATION_RSX);
	}*/
}

void ShaderRSX::Link()
{
}

/// <summary>
/// Send to the shader the point light data
/// </summary>
/// <param name="light">Point light</param>
/// <param name="index">Shader's point light index</param>
void ShaderRSX::SetPointLightData(const Light& light, const int index)
{
	if (index >= MAX_LIGHT_COUNT)
		return;

	const PointLightVariableIds& ids = m_pointlightVariableIds[index];

	if (!ids.color || !ids.position || !ids.light_data)
		return;

	const Vector4 lightColorV4 = light.color.GetRGBA().ToVector4();
	const Vector3 lightColor = Vector3(lightColorV4.x, lightColorV4.y, lightColorV4.z) * light.GetIntensity() * 2;
	Vector3 pos = Vector3(0);
	if (light.GetTransformRaw())
	{
		pos = light.GetTransformRaw()->GetPosition();
		pos.x = -pos.x;
	}

	Vector3 lightData = Vector3(lightConstant, light.GetLinearValue(), light.GetQuadraticValue());
	rsxSetFragmentProgramParameter(RendererRSX::context, m_fragmentProgram, ids.color, (float*)&lightColor.x, m_fp_offset, GCM_LOCATION_RSX);
	rsxSetFragmentProgramParameter(RendererRSX::context, m_fragmentProgram, ids.position, (float*)&pos.x, m_fp_offset, GCM_LOCATION_RSX);
	rsxSetFragmentProgramParameter(RendererRSX::context, m_fragmentProgram, ids.light_data, (float*)&lightData.x, m_fp_offset, GCM_LOCATION_RSX);
}

/// <summary>
/// Send to the shader the directional light data
/// </summary>
/// <param name="light">Directional light</param>
/// <param name="index">Shader's directional light index</param>
void ShaderRSX::SetDirectionalLightData(const Light& light, const int index)
{
	if (index >= MAX_LIGHT_COUNT)
		return;

	const DirectionalLightsVariableIds& ids = m_directionallightVariableIds[index];
	if (!ids.color || !ids.direction)
		return;
	const Vector4 lightColorV4 = light.color.GetRGBA().ToVector4();
	const Vector3 lightColor = Vector3(lightColorV4.x, lightColorV4.y, lightColorV4.z) * light.GetIntensity() * 2;

	Vector3 dir = Vector3(0);
	if (light.GetTransformRaw())
	{
		dir = light.GetTransformRaw()->GetForward();
		dir.x = -dir.x;
	}

	rsxSetFragmentProgramParameter(RendererRSX::context, m_fragmentProgram, ids.color, (float*)&lightColor.x, m_fp_offset, GCM_LOCATION_RSX);
	rsxSetFragmentProgramParameter(RendererRSX::context, m_fragmentProgram, ids.direction, (float*)&dir.x, m_fp_offset, GCM_LOCATION_RSX);
}

void ShaderRSX::SetAmbientLightData(const Vector3& color)
{
	rsxSetFragmentProgramParameter(RendererRSX::context, m_fragmentProgram, m_ambientLightLocation, (float*)&color.x, m_fp_offset, GCM_LOCATION_RSX);
}

/// <summary>
/// Send to the shader the spot light data
/// </summary>
/// <param name="light">Spot light</param>
/// <param name="index">Shader's spot light index</param>
void ShaderRSX::SetSpotLightData(const Light& light, const int index)
{
}

int count = 0;

/// <summary>
/// Send lights data to the shader
/// </summary>
void ShaderRSX::UpdateLights()
{
	/*if (count >= 5)
	{
		return;
	}*/
	count++;
	Vector4 ambientLight = Vector4(0, 0, 0, 0);

	int offset = 1;
	const int lightCount = AssetManager::GetLightCount();

	int directionalUsed = 0;
	int pointUsed = 0;
	int spotUsed = 0;

	//if (directionalLightsids.color != INVALID_SHADER_UNIFORM)
	{
		SetDirectionalLightData(*defaultDarkLight, 0);
		//hasLightUniforms = true;
	}
	//if (pointLightIds.color != INVALID_SHADER_UNIFORM)
	{
		SetPointLightData(*defaultDarkLight, 0);
		//hasLightUniforms = true;
	}
	//if (spotLightids.color != INVALID_SHADER_UNIFORM)
	{
		SetSpotLightData(*defaultDarkLight, 0);
		//hasLightUniforms = true;
	}
	return;
	//For each lights
	for (int lightI = 0; lightI < lightCount; lightI++)
	{
		const Light& light = *AssetManager::GetLight(lightI);
		if (light.IsEnabled() && light.GetGameObjectRaw()->IsLocalActive())
		{
			if (light.m_type == LightType::Directional)
			{
				SetDirectionalLightData(light, directionalUsed + offset);
				directionalUsed++;
			}
			else if (light.m_type == LightType::Point)
			{
				SetPointLightData(light, pointUsed + offset);
				pointUsed++;
			}
			else if (light.m_type == LightType::Spot)
			{
				SetSpotLightData(light, spotUsed + offset);
				spotUsed++;
			}
			else if (light.m_type == LightType::Ambient)
			{
				ambientLight += light.color.GetRGBA().ToVector4() * light.m_intensity;
			}
		}
	}

	if (m_ambientLightLocation)
	{
		SetAmbientLightData(Vector3(ambientLight.x, ambientLight.y, ambientLight.z));
	}

	SetShaderAttribut("usedPointLightCount", (float)pointUsed);
	SetShaderAttribut("usedSpotLightCount", (float)spotUsed);
	SetShaderAttribut("usedDirectionalLightCount", (float)directionalUsed);
}

void ShaderRSX::CreateShader(Shader::ShaderType type)
{
}

#endif