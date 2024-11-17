// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(__PS3__)

#include "shader_rsx.h"

#include <cstring>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "graphics.h"
#include <engine/application.h>
#include <engine/file_system/file.h>
#include <engine/debug/debug.h>
#include <engine/engine.h>
#include <engine/graphics/camera.h>
#include <engine/graphics/renderer/renderer_rsx.h>
#include <engine/game_elements/transform.h>
#include <engine/game_elements/gameobject.h>
#include <engine/lighting/lighting.h>

#include <rsx/rsx.h>

#include <engine/asset_management/asset_manager.h>
#include <engine/tools/endian_utils.h>

ShaderRSX::~ShaderRSX()
{
	
}

void ShaderRSX::Init()
{
	
}

void ShaderRSX::Load()
{
	Debug::Print("ShaderRSX::Load");

	size_t size = 0;
	unsigned char* fullShader = ReadShaderBinary(size);
	Debug::Print("size: " + std::to_string(size));

	// Read vertex shader code size
	uint32_t vertexShaderCodeSize = 0;
	memcpy(&vertexShaderCodeSize, fullShader, sizeof(uint32_t));
	vertexShaderCodeSize = EndianUtils::SwapEndian(vertexShaderCodeSize);
	fullShader += sizeof(uint32_t);
	Debug::Print("vertexShaderCodeSize: " + std::to_string(vertexShaderCodeSize));

	// Read code
	m_vertexProgram = (rsxVertexProgram*)fullShader;

	// Check magic numbers and if the file is corrupted, the vertex shader size may be greater than the full shader size
	if(((char*)m_vertexProgram)[0] != 'V' || ((char*)m_vertexProgram)[1] != 'P' || vertexShaderCodeSize >= size)
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
	Debug::Print("fragmentShaderCodeSize: " + std::to_string(fragmentShaderCodeSize));

	m_fragmentProgram = (rsxFragmentProgram*)fullShader;

	// Check magic numbers and if the file is corrupted, the fragment shader size may be greater than the full shader size
	if(((char*)m_fragmentProgram)[0] != 'F' || ((char*)m_fragmentProgram)[1] != 'P' || fragmentShaderCodeSize >= size)
	{
		Debug::PrintError("Cannot load fragment program!");
		m_fileStatus = FileStatus::FileStatus_Failed;
		return;
	}

	{
		rsxVertexProgramGetUCode(m_vertexProgram, &m_vertexProgramCode, &m_vertexProgramSize);
		printf("vertexProgramSize: %d\n", m_vertexProgramSize);

		m_projMatrix = rsxVertexProgramGetConst(m_vertexProgram, "projection");
		m_modelMatrix = rsxVertexProgramGetConst(m_vertexProgram, "camera");
		m_viewMatrix = rsxVertexProgramGetConst(m_vertexProgram, "model");
		
		if(m_projMatrix)
		{
			Debug::Print("m_projMatrix");
		}
		else
		{
			Debug::Print("No m_projMatrix");
		}

		if(m_modelMatrix)
		{
			Debug::Print("m_modelMatrix");
		}
		else
		{
			Debug::Print("No m_modelMatrix");
		}

		if(m_viewMatrix)
		{
			Debug::Print("m_viewMatrix");
		}
		else
		{
			Debug::Print("No m_viewMatrix");
		}
	}

	{
		rsxFragmentProgramGetUCode(m_fragmentProgram, &m_fragmentProgramCode, &m_fragmentProgramSize);
		printf("fragmentProgramSize: %d\n", m_fragmentProgramSize);

		m_fragmentProgramCodeOnGPU = (uint32_t*)rsxMemalign(64, m_fragmentProgramSize);
		memcpy(m_fragmentProgramCodeOnGPU, m_fragmentProgramCode, m_fragmentProgramSize);
		rsxAddressToOffset(m_fragmentProgramCodeOnGPU, &m_fp_offset);

		m_color = rsxFragmentProgramGetConst(m_fragmentProgram, "color");
		m_textureUnit = rsxFragmentProgramGetAttrib(m_fragmentProgram, "texture");
		m_ambientLightLocation = rsxFragmentProgramGetConst(m_fragmentProgram, "ambientLight");

		if(m_color)
		{
			Debug::Print("m_color");
		}
		else
		{
			Debug::Print("No m_color");
		}

		if(m_textureUnit)
		{
			Debug::Print("m_textureUnit");
		}
		else
		{
			Debug::Print("No m_textureUnit");
		}

		if(m_ambientLightLocation)
		{
			Debug::Print("m_ambientLightLocation");
		}
		else
		{
			Debug::Print("No m_ambientLightLocation");
		}
	}

	m_fileStatus = FileStatus::FileStatus_Loaded;
}

bool ShaderRSX::Use()
{
	if (Graphics::s_currentShader != this)
	{
		rsxLoadVertexProgram(RendererRSX::context, m_vertexProgram, m_vertexProgramCode);
		rsxLoadFragmentProgramLocation(RendererRSX::context, m_fragmentProgram, m_fp_offset, GCM_LOCATION_RSX);
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
	if(m_viewMatrix)
	{
		const Transform* transform = Graphics::usedCamera->GetTransformRaw();

		const Vector3& position = transform->GetPosition();

		const Quaternion& baseQ = transform->GetRotation();
		static const Quaternion offsetQ = Quaternion::Euler(0, 180, 0);
		const Quaternion newQ = baseQ * offsetQ;

		glm::mat4 RotationMatrix = glm::toMat4(glm::quat(newQ.w, -newQ.x, newQ.y, newQ.z));

		if (position.x != 0.0f || position.y != 0.0f || position.z != 0.0f)
			RotationMatrix = glm::translate(RotationMatrix, glm::vec3(position.x, -position.y, -position.z));

		rsxSetVertexProgramParameter(RendererRSX::context, m_vertexProgram, m_viewMatrix, (float*)&RotationMatrix);
	}
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
	if(m_projMatrix)
	{
		rsxSetVertexProgramParameter(RendererRSX::context, m_vertexProgram, m_projMatrix, (float*)&Graphics::usedCamera->GetProjection());
	}
}

void ShaderRSX::SetShaderProjectionCanvas()
{
}

/// <summary>
/// Send to the shader transform's model
/// </summary>
/// <param name="trans"></param>
void ShaderRSX::SetShaderModel(const glm::mat4& trans)
{
	if(m_modelMatrix)
	{
		rsxSetVertexProgramParameter(RendererRSX::context, m_vertexProgram, m_modelMatrix, (float*)&trans);
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

void ShaderRSX::SetLightIndices(const LightsIndices& lightsIndices)
{
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
}

/// <summary>
/// Send to the shader the directional light data
/// </summary>
/// <param name="light">Directional light</param>
/// <param name="index">Shader's directional light index</param>
void ShaderRSX::SetDirectionalLightData(const Light& light, const int index)
{
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

/// <summary>
/// Send lights data to the shader
/// </summary>
void ShaderRSX::UpdateLights()
{
	Vector4 ambientLight = Vector4(0, 0, 0, 0);

	const int lightCount = AssetManager::GetLightCount();

	//For each lights
	for (int lightI = 0; lightI < lightCount; lightI++)
	{
		const Light& light = *AssetManager::GetLight(lightI);
		if (light.IsEnabled() && light.GetGameObjectRaw()->IsLocalActive())
		{
			if (light.m_type == LightType::Directional)
			{
				// SetDirectionalLightData(light, directionalUsed + offset);
				// directionalUsed++;
			}
			else if (light.m_type == LightType::Point)
			{
				// SetPointLightData(light, pointUsed + offset);
				// pointUsed++;
			}
			else if (light.m_type == LightType::Spot)
			{
				// SetSpotLightData(light, spotUsed + offset);
				// spotUsed++;
			}
			else if (light.m_type == LightType::Ambient)
			{
				ambientLight += light.color.GetRGBA().ToVector4() * light.m_intensity;
			}
		}
	}

	if(m_ambientLightLocation)
	{
		SetAmbientLightData(Vector3(ambientLight.x, ambientLight.y, ambientLight.z));
	}
}

void ShaderRSX::CreateShader(Shader::ShaderType type)
{
}

#endif