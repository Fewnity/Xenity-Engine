// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__) || defined(__vita__)
#include "shader_opengl.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "graphics.h"
#include <engine/application.h>
#include <engine/file_system/file.h>
#include <engine/debug/debug.h>
#include <engine/engine.h>
#include <engine/graphics/camera.h>
#include <engine/game_elements/transform.h>
#include <engine/game_elements/gameobject.h>
#include <engine/lighting/lighting.h>

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
#include <glad/glad.h>
#elif defined(__vita__)
#include <vitaGL.h>
#endif
#include <engine/asset_management/asset_manager.h>
#include <glm/gtc/type_ptr.hpp>

std::vector<ShaderOpenGL::PointLightVariableNames> ShaderOpenGL::s_pointlightVariableNames;
std::vector<ShaderOpenGL::DirectionalLightsVariableNames> ShaderOpenGL::s_directionallightVariableNames;
std::vector<ShaderOpenGL::SpotLightVariableNames> ShaderOpenGL::s_spotlightVariableNames;
unsigned int uboLightBlock;

ShaderOpenGL::PointLightVariableIds::PointLightVariableIds(int index, unsigned int programId)
{
	indices = GetShaderUniformLocation(programId, s_pointlightVariableNames[index].indices);
	color = GetShaderUniformLocation(programId, s_pointlightVariableNames[index].color);
	position = GetShaderUniformLocation(programId, s_pointlightVariableNames[index].position);
	constant = GetShaderUniformLocation(programId, s_pointlightVariableNames[index].constant);
	linear = GetShaderUniformLocation(programId, s_pointlightVariableNames[index].linear);
	quadratic = GetShaderUniformLocation(programId, s_pointlightVariableNames[index].quadratic);

	// Initialize values
	SetShaderAttribut(color, Vector3(0, 0, 0));
	SetShaderAttribut(position, Vector3(0, 0, 0));
	SetShaderAttribut(constant, 1.0f);
	SetShaderAttribut(linear, 0.0f);
	SetShaderAttribut(quadratic, 0.0f);
}

ShaderOpenGL::DirectionalLightsVariableIds::DirectionalLightsVariableIds(int index, unsigned int programId)
{
	indices = GetShaderUniformLocation(programId, s_directionallightVariableNames[index].indices);
	color = GetShaderUniformLocation(programId, s_directionallightVariableNames[index].color);
	direction = GetShaderUniformLocation(programId, s_directionallightVariableNames[index].direction);

	// Initialize values
	SetShaderAttribut(color, Vector3(0, 0, 0));
	SetShaderAttribut(direction, Vector3(0, 0, 0));
}

ShaderOpenGL::SpotLightVariableIds::SpotLightVariableIds(int index, unsigned int programId)
{
	indices = GetShaderUniformLocation(programId, s_spotlightVariableNames[index].indices);
	color = GetShaderUniformLocation(programId, s_spotlightVariableNames[index].color);
	position = GetShaderUniformLocation(programId, s_spotlightVariableNames[index].position);
	direction = GetShaderUniformLocation(programId, s_spotlightVariableNames[index].direction);
	constant = GetShaderUniformLocation(programId, s_spotlightVariableNames[index].constant);
	linear = GetShaderUniformLocation(programId, s_spotlightVariableNames[index].linear);
	quadratic = GetShaderUniformLocation(programId, s_spotlightVariableNames[index].quadratic);
	cutOff = GetShaderUniformLocation(programId, s_spotlightVariableNames[index].cutOff);
	outerCutOff = GetShaderUniformLocation(programId, s_spotlightVariableNames[index].outerCutOff);

	// Initialize values
	SetShaderAttribut(color, Vector3(0, 0, 0));
	SetShaderAttribut(position, Vector3(0, 0, 0));
	SetShaderAttribut(direction, Vector3(0, 0, 0));
	SetShaderAttribut(constant, 1.0f);
	SetShaderAttribut(linear, 0.0f);
	SetShaderAttribut(quadratic, 0.0f);
	SetShaderAttribut(cutOff, 0.0f);
	SetShaderAttribut(outerCutOff, 0.0f);
}

ShaderOpenGL::PointLightVariableNames::PointLightVariableNames(int index)
{
	constexpr int bufferSize = 30;

	indices = new char[bufferSize];
	color = new char[bufferSize];
	position = new char[bufferSize];
	constant = new char[bufferSize];
	linear = new char[bufferSize];
	quadratic = new char[bufferSize];

#if defined(_WIN32) || defined(_WIN64)
	sprintf_s(indices, bufferSize, "pointLightsIndices[%d]", index);
	sprintf_s(color, bufferSize, "pointLights[%d].color", index);
	sprintf_s(position, bufferSize, "pointLights[%d].position", index);
	sprintf_s(constant, bufferSize, "pointLights[%d].constant", index);
	sprintf_s(linear, bufferSize, "pointLights[%d].linear", index);
	sprintf_s(quadratic, bufferSize, "pointLights[%d].quadratic", index);
#else
	sprintf(color, "pointLights[%d].color", index);
	sprintf(position, "pointLights[%d].position", index);
	sprintf(constant, "pointLights[%d].constant", index);
	sprintf(linear, "pointLights[%d].linear", index);
	sprintf(quadratic, "pointLights[%d].quadratic", index);
#endif
}

ShaderOpenGL::PointLightVariableNames::~PointLightVariableNames()
{
	delete[] color;
	delete[] position;
	delete[] constant;
	delete[] linear;
	delete[] quadratic;
}

ShaderOpenGL::DirectionalLightsVariableNames::DirectionalLightsVariableNames(int index)
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

ShaderOpenGL::DirectionalLightsVariableNames::~DirectionalLightsVariableNames()
{
	delete[] color;
	delete[] direction;
}

ShaderOpenGL::SpotLightVariableNames::SpotLightVariableNames(int index)
{
	constexpr int bufferSize = 30;

	indices = new char[bufferSize];
	color = new char[bufferSize];
	position = new char[bufferSize];
	direction = new char[bufferSize];
	constant = new char[bufferSize];
	linear = new char[bufferSize];
	quadratic = new char[bufferSize];
	cutOff = new char[bufferSize];
	outerCutOff = new char[bufferSize];

#if defined(_WIN32) || defined(_WIN64)
	sprintf_s(indices, bufferSize, "spotLightsIndices[%d]", index);
	sprintf_s(color, bufferSize, "spotLights[%d].color", index);
	sprintf_s(position, bufferSize, "spotLights[%d].position", index);
	sprintf_s(direction, bufferSize, "spotLights[%d].direction", index);
	sprintf_s(constant, bufferSize, "spotLights[%d].constant", index);
	sprintf_s(linear, bufferSize, "spotLights[%d].linear", index);
	sprintf_s(quadratic, bufferSize, "spotLights[%d].quadratic", index);
	sprintf_s(cutOff, bufferSize, "spotLights[%d].cutOff", index);
	sprintf_s(outerCutOff, bufferSize, "spotLights[%d].outerCutOff", index);
#else
	sprintf(color, "spotLights[%d].color", index);
	sprintf(position, "spotLights[%d].position", index);
	sprintf(direction, "spotLights[%d].direction", index);
	sprintf(constant, "spotLights[%d].constant", index);
	sprintf(linear, "spotLights[%d].linear", index);
	sprintf(quadratic, "spotLights[%d].quadratic", index);
	sprintf(cutOff, "spotLights[%d].cutOff", index);
	sprintf(outerCutOff, "spotLights[%d].outerCutOff", index);
#endif
}

ShaderOpenGL::SpotLightVariableNames::~SpotLightVariableNames()
{
	delete[] color;
	delete[] position;
	delete[] direction;
	delete[] constant;
	delete[] linear;
	delete[] quadratic;
	delete[] cutOff;
	delete[] outerCutOff;
}

ShaderOpenGL::~ShaderOpenGL()
{
	if (m_fileStatus == FileStatus::FileStatus_Loaded)
	{
		if constexpr (!Graphics::s_UseOpenGLFixedFunctions)
		{
			if (Engine::IsRunning(true))
			{
				glDeleteShader(m_vertexShaderId);
				glDeleteShader(m_fragmentShaderId);
				if (m_useTessellation)
				{
					glDeleteShader(m_tessellationEvaluationShaderId);
					glDeleteShader(m_fragmentShaderId);
				}
				// This cause the psvita to crash, bug in older version of vitaGL, fixed in the latest version but the latest version is running very slow
				//glDeleteProgram(m_programId);
				//Engine::GetRenderer().DeleteShaderProgram(m_programId);
			}
		}
		m_fileStatus = FileStatus::FileStatus_Not_Loaded;
	}
}

void ShaderOpenGL::Init()
{
	s_pointlightVariableNames.reserve(MAX_LIGHT_COUNT);
	s_directionallightVariableNames.reserve(MAX_LIGHT_COUNT);
	s_spotlightVariableNames.reserve(MAX_LIGHT_COUNT);
	for (int i = 0; i < MAX_LIGHT_COUNT; i++)
	{
		s_pointlightVariableNames.emplace_back(i);
		s_directionallightVariableNames.emplace_back(i);
		s_spotlightVariableNames.emplace_back(i);
	}

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__) || defined(__vita__)
	glGenBuffers(1, &uboLightBlock);
	glBindBuffer(GL_UNIFORM_BUFFER, uboLightBlock);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightsIndices), NULL, GL_DYNAMIC_DRAW);
#endif
}

void ShaderOpenGL::Load()
{
	if constexpr (Graphics::s_UseOpenGLFixedFunctions)
	{
		m_fileStatus = FileStatus::FileStatus_Loaded;
		return;
	}

	const std::string vertexShaderCode = GetShaderCode(ShaderType::Vertex_Shader, Application::GetPlatform());
	const std::string fragmentShaderCode = GetShaderCode(ShaderType::Fragment_Shader, Application::GetPlatform());

	if (!vertexShaderCode.empty() && !fragmentShaderCode.empty())
	{
		const bool vertexRet = Compile(vertexShaderCode, ShaderType::Vertex_Shader);
		const bool fragRet = Compile(fragmentShaderCode, ShaderType::Fragment_Shader);

		if (vertexRet && fragRet)
		{
			Link();
			m_fileStatus = FileStatus::FileStatus_Loaded;
		}
		else
		{
			Debug::PrintError("[Shader::LoadFileReference] Cannot link the shader, the compilation has failed: " + m_file->GetPath(), true);
			m_fileStatus = FileStatus::FileStatus_Failed;
		}
	}
	else
	{
		Debug::PrintError("[Shader::LoadFileReference] The shader structure is wrong: " + m_file->GetPath(), true);
		m_fileStatus = FileStatus::FileStatus_Failed;
	}
}

bool ShaderOpenGL::Use()
{
	if (Graphics::s_currentShader != this)
	{
		glUseProgram(m_programId);
		Graphics::s_currentShader = this;
		return true;
	}
	return false;
}

int ShaderOpenGL::GetShaderTypeEnum(ShaderType shaderType)
{
	int compileType = GL_VERTEX_SHADER;
	switch (shaderType)
	{
	case Shader::ShaderType::Vertex_Shader:
		compileType = GL_VERTEX_SHADER;
		break;
	case Shader::ShaderType::Fragment_Shader:
		compileType = GL_FRAGMENT_SHADER;
		break;
#if !defined(__vita__)
	case Shader::ShaderType::Tessellation_Control_Shader:
		compileType = GL_TESS_CONTROL_SHADER;
		break;
	case Shader::ShaderType::Tessellation_Evaluation_Shader:
		compileType = GL_TESS_EVALUATION_SHADER;
		break;
#endif
	}
	return compileType;
}

bool ShaderOpenGL::Compile(const std::string& shaderData, ShaderType type)
{
	XASSERT(!shaderData.empty(), "[Shader::Compile] shaderData is empty");

	const char* shaderDataConst = shaderData.c_str();

	unsigned int* id = nullptr;

	switch (type)
	{
	case ShaderType::Vertex_Shader:
		id = &m_vertexShaderId;
		break;
	case ShaderType::Fragment_Shader:
		id = &m_fragmentShaderId;
		break;
	case ShaderType::Tessellation_Control_Shader:
		id = &m_tessellationShaderId;
		break;
	case ShaderType::Tessellation_Evaluation_Shader:
		id = &m_tessellationEvaluationShaderId;
		break;
	}

	XASSERT(id != nullptr, "[Shader::Compile] Shader type not found, id is nullptr");

	//Compile
	*id = glCreateShader(GetShaderTypeEnum(type));
	glShaderSource(*id, 1, &shaderDataConst, NULL);

	glCompileShader(*id);

	GLint vResult;
	glGetShaderiv(*id, GL_COMPILE_STATUS, &vResult);

	//On error
	if (vResult == 0)
	{
		int maxLength = 256;
		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(*id, maxLength, &maxLength, &errorLog[0]);

		std::string shaderError = "[Shader::Compile] Compilation error: ";
		switch (type)
		{
		case ShaderType::Vertex_Shader:
			shaderError += "Vertex";
			break;
		case ShaderType::Fragment_Shader:
			shaderError += "Fragment";
			break;
		case ShaderType::Tessellation_Control_Shader:
			shaderError += "Tessellation control";
			break;
		case ShaderType::Tessellation_Evaluation_Shader:
			shaderError += "Tessellation evaluation";
			break;
		}

		shaderError += " shader: ";
		for (int i = 0; i < errorLog.size(); i++)
		{
			shaderError += errorLog[i];
		}

		shaderError += ". File path: " + shaderData;
		Debug::PrintError(shaderError);
		return false;
	}

	return true;
}

#pragma endregion

#pragma region Uniform setters

/// <summary>
/// Send to the shader the 3D camera position
/// </summary>
void ShaderOpenGL::SetShaderCameraPosition()
{
	//Camera position
	if (Graphics::usedCamera != nullptr)
	{
		const Transform* transform = Graphics::usedCamera->GetTransformRaw();

		const Vector3& position = transform->GetPosition();

		const Quaternion& baseQ = transform->GetRotation();
		static const Quaternion offsetQ = Quaternion::Euler(0, 180, 0);
		const Quaternion newQ = baseQ * offsetQ;

		glm::mat4 RotationMatrix = glm::toMat4(glm::quat(newQ.w, -newQ.x, newQ.y, newQ.z));

		if (position.x != 0.0f || position.y != 0.0f || position.z != 0.0f)
			RotationMatrix = glm::translate(RotationMatrix, glm::vec3(position.x, -position.y, -position.z));

		glUniformMatrix4fv(m_cameraLocation, 1, false, glm::value_ptr(RotationMatrix)); // Y position and rotation inverted
	}
}

/// <summary>
/// Send to the shader the 2D camera position
/// </summary>
void ShaderOpenGL::SetShaderCameraPositionCanvas()
{
	glUniformMatrix4fv(m_cameraLocation, 1, false, glm::value_ptr(m_canvasCameraTransformationMatrix));
}

/// <summary>
/// Send to the shader the 2D camera projection
/// </summary>
void ShaderOpenGL::SetShaderProjection()
{
	glUniformMatrix4fv(m_projectionLocation, 1, false, glm::value_ptr(Graphics::usedCamera->GetProjection()));
}

void ShaderOpenGL::SetShaderProjectionCanvas()
{
	glUniformMatrix4fv(m_projectionLocation, 1, false, glm::value_ptr(Graphics::usedCamera->GetCanvasProjection()));
}

/// <summary>
/// Send to the shader transform's model
/// </summary>
/// <param name="trans"></param>
void ShaderOpenGL::SetShaderModel(const glm::mat4& trans)
{
	SetShaderAttribut(m_modelLocation, trans);
}

/// <summary>
/// Send to the shader transform's model
/// </summary>
/// <param name="trans"></param>
void ShaderOpenGL::SetShaderModel(const Vector3& position, const Vector3& rotation, const Vector3& scale)
{
	glm::mat4 transformationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-position.x, position.y, position.z));

	if (rotation.y != 0)
		transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.y * -1), glm::vec3(0.0f, 1.0f, 0.0f));
	if (rotation.x != 0)
		transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	if (rotation.z != 0)
		transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.z * -1), glm::vec3(0.0f, 0.0f, 1.0f));

	//if (scale.x != 1 || scale.y != 1|| scale.z != 1)
	transformationMatrix = glm::scale(transformationMatrix, glm::vec3(scale.x, scale.y, scale.z));

	SetShaderAttribut(m_modelLocation, transformationMatrix);
}

void ShaderOpenGL::SetLightIndices(const LightsIndices& lightsIndices)
{
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__) || defined(__vita__)
	glBindBuffer(GL_UNIFORM_BUFFER, uboLightBlock);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(lightsIndices), &lightsIndices);
#endif
}

unsigned int ShaderOpenGL::GetShaderUniformLocation(const char* name)
{
	return glGetUniformLocation(m_programId, name);
}

unsigned int ShaderOpenGL::GetShaderUniformLocation(unsigned int programId, const char* name)
{
	return glGetUniformLocation(programId, name);
}

unsigned int ShaderOpenGL::FindOrAddAttributId(const std::string& attribut)
{
	auto it = m_uniformsIds.find(attribut);
	if (it == m_uniformsIds.end())
	{
		const unsigned int id = GetShaderUniformLocation(attribut.c_str());
		it = m_uniformsIds.emplace(attribut, id).first;
	}
	return it->second;
}

void ShaderOpenGL::SetShaderAttribut(const std::string& attribut, const Vector4& value)
{
	unsigned int attributId = FindOrAddAttributId(attribut);
	glUniform4f(attributId, value.x, value.y, value.z, value.w);
}

void ShaderOpenGL::SetShaderAttribut(const std::string& attribut, const Vector3& value)
{
	unsigned int attributId = FindOrAddAttributId(attribut);
	glUniform3f(attributId, value.x, value.y, value.z);
}

void ShaderOpenGL::SetShaderAttribut(const std::string& attribut, const Vector2& value)
{
	unsigned int attributId = FindOrAddAttributId(attribut);
	glUniform2f(attributId, value.x, value.y);
}

void ShaderOpenGL::SetShaderAttribut(const std::string& attribut, float value)
{
	unsigned int attributId = FindOrAddAttributId(attribut);
	glUniform1f(attributId, value);
}

void ShaderOpenGL::SetShaderAttribut(const std::string& attribut, int value)
{
	unsigned int attributId = FindOrAddAttributId(attribut);
	glUniform1i(attributId, value);
}

void ShaderOpenGL::SetShaderAttribut(unsigned int attributId, const Vector4& value)
{
	glUniform4f(attributId, value.x, value.y, value.z, value.w);
}

void ShaderOpenGL::SetShaderAttribut(unsigned int attributId, const Vector3& value)
{
	glUniform3f(attributId, value.x, value.y, value.z);
}

void ShaderOpenGL::SetShaderAttribut(unsigned int attributId, const Vector2& value)
{
	glUniform2f(attributId, value.x, value.y);
}

void ShaderOpenGL::SetShaderAttribut(unsigned int attributId, const float value)
{
	glUniform1f(attributId, value);
}

void ShaderOpenGL::SetShaderAttribut(unsigned int attributId, const int value)
{
	glUniform1i(attributId, value);
}

void ShaderOpenGL::SetShaderAttribut(unsigned int attributId, const glm::mat4& trans)
{
	glUniformMatrix4fv(attributId, 1, false, glm::value_ptr(trans));
}

void ShaderOpenGL::SetShaderAttribut(unsigned int attributId, const glm::mat3& trans)
{
	glUniformMatrix3fv(attributId, 1, false, glm::value_ptr(trans));
}

void ShaderOpenGL::Link()
{
	m_programId = glCreateProgram();
	glAttachShader(m_programId, m_vertexShaderId);

	if (m_useTessellation)
	{
		glAttachShader(m_programId, m_tessellationShaderId);
		glAttachShader(m_programId, m_tessellationEvaluationShaderId);
	}
	glAttachShader(m_programId, m_fragmentShaderId);

#if defined(__vita__)
	glBindAttribLocation(m_programId, 0, "position");
	glBindAttribLocation(m_programId, 1, "uv");
	glBindAttribLocation(m_programId, 2, "normal");
#endif
	glLinkProgram(m_programId);
	Engine::GetRenderer().UseShaderProgram(m_programId);

	m_modelLocation = GetShaderUniformLocation("model");
	m_projectionLocation = GetShaderUniformLocation("projection");
	m_cameraLocation = GetShaderUniformLocation("camera");
	m_ambientLightLocation = GetShaderUniformLocation("ambientLight");

	m_usedPointLightCountLocation = GetShaderUniformLocation("usedPointLightCount");
	m_usedSpotLightCountLocation = GetShaderUniformLocation("usedSpotLightCount");
	m_usedDirectionalLightCountLocation = GetShaderUniformLocation("usedDirectionalLightCount");

	m_pointlightVariableIds.reserve(MAX_LIGHT_COUNT);
	m_directionallightVariableIds.reserve(MAX_LIGHT_COUNT);
	m_spotlightVariableIds.reserve(MAX_LIGHT_COUNT);
	for (int i = 0; i < MAX_LIGHT_COUNT; i++)
	{
		m_pointlightVariableIds.emplace_back(i, m_programId);
		m_directionallightVariableIds.emplace_back(i, m_programId);
		m_spotlightVariableIds.emplace_back(i, m_programId);
	}

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__) || defined(__vita__)
	GLuint blockIndex = -1;
#if defined(__vita__)
	blockIndex = glGetUniformBlockIndex(m_programId, "lightIndices");
#else // #if !defined(__vita__)
	blockIndex = glGetUniformBlockIndex(m_programId, "LightIndices");
#endif

	if (blockIndex == -1)
	{
		Debug::PrintWarning("The shader does not have a LightIndices uniform buffer: " + m_file->GetFileName());
	}
	else
	{
		GLuint bindingPoint = 0;
		glUniformBlockBinding(m_programId, blockIndex, bindingPoint);
		glBindBuffer(GL_UNIFORM_BUFFER, uboLightBlock);
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, uboLightBlock);
	}
#endif // #if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__) || defined(__vita__)
}

/// <summary>
/// Send to the shader the point light data
/// </summary>
/// <param name="light">Point light</param>
/// <param name="index">Shader's point light index</param>
void ShaderOpenGL::SetPointLightData(const Light& light, const int index)
{
	if (index >= MAX_LIGHT_COUNT)
		return;

	const PointLightVariableIds& ids = m_pointlightVariableIds[index];

	// Check uniforms
	XASSERT(ids.color != INVALID_SHADER_UNIFORM, "[Shader::SetPointLightData] The shader does not have a point light color uniform");
	XASSERT(ids.position != INVALID_SHADER_UNIFORM, "[Shader::SetPointLightData] The shader does not have a point light position uniform");
	XASSERT(ids.constant != INVALID_SHADER_UNIFORM, "[Shader::SetPointLightData] The shader does not have a point light constant uniform");
	XASSERT(ids.linear != INVALID_SHADER_UNIFORM, "[Shader::SetPointLightData] The shader does not have a point light linear uniform");
	XASSERT(ids.quadratic != INVALID_SHADER_UNIFORM, "[Shader::SetPointLightData] The shader does not have a point light quadratic uniform");

	const Vector4 lightColorV4 = light.color.GetRGBA().ToVector4();
	const Vector3 lightColor = Vector3(lightColorV4.x, lightColorV4.y, lightColorV4.z);
	Vector3 pos = Vector3(0);
	if (light.GetTransformRaw())
	{
		pos = light.GetTransformRaw()->GetPosition();
		pos.x = -pos.x;
	}

	SetShaderAttribut(ids.color, lightColor * light.GetIntensity());
	SetShaderAttribut(ids.position, pos);
	SetShaderAttribut(ids.constant, lightConstant);
	SetShaderAttribut(ids.linear, light.GetLinearValue());
	SetShaderAttribut(ids.quadratic, light.GetQuadraticValue());
}

/// <summary>
/// Send to the shader the directional light data
/// </summary>
/// <param name="light">Directional light</param>
/// <param name="index">Shader's directional light index</param>
void ShaderOpenGL::SetDirectionalLightData(const Light& light, const int index)
{
	if (index >= MAX_LIGHT_COUNT)
		return;

	const DirectionalLightsVariableIds& ids = m_directionallightVariableIds[index];

	// Check uniforms
	XASSERT(ids.color != INVALID_SHADER_UNIFORM, "[Shader::SetDirectionalLightData] The shader does not have a directional light color uniform");
	XASSERT(ids.direction != INVALID_SHADER_UNIFORM, "[Shader::SetDirectionalLightData] The shader does not have a directional light direction uniform");

	const Vector4 lightColorV4 = light.color.GetRGBA().ToVector4();
	const Vector3 lightColor = Vector3(lightColorV4.x, lightColorV4.y, lightColorV4.z);

	Vector3 dir = Vector3(0);
	if (light.GetTransformRaw())
	{
		dir = light.GetTransformRaw()->GetForward();
		dir.x = -dir.x;
	}

	SetShaderAttribut(ids.color, lightColor * light.GetIntensity());
	SetShaderAttribut(ids.direction, dir);
}

void ShaderOpenGL::SetAmbientLightData(const Vector3& color)
{
	SetShaderAttribut(m_ambientLightLocation, color);
}

/// <summary>
/// Send to the shader the spot light data
/// </summary>
/// <param name="light">Spot light</param>
/// <param name="index">Shader's spot light index</param>
void ShaderOpenGL::SetSpotLightData(const Light& light, const int index)
{
	if (index >= MAX_LIGHT_COUNT)
		return;

	const SpotLightVariableIds& ids = m_spotlightVariableIds[index];

	// Check uniforms
	XASSERT(ids.color != INVALID_SHADER_UNIFORM, "[Shader::SetSpotLightData] The shader does not have a spot light color uniform");
	XASSERT(ids.position != INVALID_SHADER_UNIFORM, "[Shader::SetSpotLightData] The shader does not have a spot light position uniform");
	XASSERT(ids.direction != INVALID_SHADER_UNIFORM, "[Shader::SetSpotLightData] The shader does not have a spot light direction uniform");
	XASSERT(ids.constant != INVALID_SHADER_UNIFORM, "[Shader::SetSpotLightData] The shader does not have a spot light constant uniform");
	XASSERT(ids.linear != INVALID_SHADER_UNIFORM, "[Shader::SetSpotLightData] The shader does not have a spot light linear uniform");
	XASSERT(ids.quadratic != INVALID_SHADER_UNIFORM, "[Shader::SetSpotLightData] The shader does not have a spot light quadratic uniform");
	XASSERT(ids.cutOff != INVALID_SHADER_UNIFORM, "[Shader::SetSpotLightData] The shader does not have a spot light cutOff uniform");
	XASSERT(ids.outerCutOff != INVALID_SHADER_UNIFORM, "[Shader::SetSpotLightData] The shader does not have a spot light outerCutOff uniform");

	const Vector4 lightColorV4 = light.color.GetRGBA().ToVector4();
	const Vector3 lightColor = Vector3(lightColorV4.x, lightColorV4.y, lightColorV4.z);

	Vector3 pos = Vector3(0);
	Vector3 dir = Vector3(0);

	if (light.GetTransformRaw())
	{
		pos = light.GetTransformRaw()->GetPosition();
		pos.x = -pos.x;

		dir = light.GetTransformRaw()->GetForward();
		dir.x = -dir.x;
	}

	SetShaderAttribut(ids.color, lightColor * light.GetIntensity());
	SetShaderAttribut(ids.position, pos);
	SetShaderAttribut(ids.direction, dir);
	SetShaderAttribut(ids.constant, lightConstant);
	SetShaderAttribut(ids.linear, light.GetLinearValue());
	SetShaderAttribut(ids.quadratic, light.GetQuadraticValue());
	SetShaderAttribut(ids.cutOff, glm::cos(glm::radians(light.GetSpotAngle() * (1 - light.GetSpotSmoothness()))));
	SetShaderAttribut(ids.outerCutOff, glm::cos(glm::radians(light.GetSpotAngle())));
}

/// <summary>
/// Send lights data to the shader
/// </summary>
void ShaderOpenGL::UpdateLights()
{
	Vector4 ambientLight = Vector4(0, 0, 0, 0);

	const PointLightVariableIds& pointLightIds = m_pointlightVariableIds[0];
	const DirectionalLightsVariableIds& directionalLightsids = m_directionallightVariableIds[0];
	const SpotLightVariableIds& spotLightids = m_spotlightVariableIds[0];

	int directionalUsed = 0;
	int pointUsed = 0;
	int spotUsed = 0;
	bool hasLightUniforms = false;

	// Set the first light of each type to a dark light in the shader
	// Update values only if the shader has lighting uniforms
	if (directionalLightsids.color != INVALID_SHADER_UNIFORM)
	{
		SetDirectionalLightData(*defaultDarkLight, 0);
		hasLightUniforms = true;
	}
	if (pointLightIds.color != INVALID_SHADER_UNIFORM)
	{
		SetPointLightData(*defaultDarkLight, 0);
		hasLightUniforms = true;
	}
	if (spotLightids.color != INVALID_SHADER_UNIFORM)
	{
		SetSpotLightData(*defaultDarkLight, 0);
		hasLightUniforms = true;
	}

	if (hasLightUniforms)
	{
		int offset = 1;
		const int lightCount = AssetManager::GetLightCount();

		//For each lights
		for (int lightI = 0; lightI < lightCount; lightI++)
		{
			const Light& light = *AssetManager::GetLight(lightI);
			if (light.IsEnabled() && light.GetGameObjectRaw()->IsLocalActive())
			{
				if (light.m_type == LightType::Directional && directionalLightsids.color != INVALID_SHADER_UNIFORM)
				{
					SetDirectionalLightData(light, directionalUsed + offset);
					directionalUsed++;
				}
				else if (light.m_type == LightType::Point && pointLightIds.color != INVALID_SHADER_UNIFORM)
				{
					SetPointLightData(light, pointUsed + offset);
					pointUsed++;
				}
				else if (light.m_type == LightType::Spot && spotLightids.color != INVALID_SHADER_UNIFORM)
				{
					SetSpotLightData(light, spotUsed + offset);
					spotUsed++;
				}
				else if (light.m_type == LightType::Ambient && m_ambientLightLocation != INVALID_SHADER_UNIFORM)
				{
					ambientLight += light.color.GetRGBA().ToVector4() * light.m_intensity;
				}
			}
		}
	}

	if (m_ambientLightLocation != -1)
	{
		SetAmbientLightData(Vector3(ambientLight.x, ambientLight.y, ambientLight.z));
	}
}

void ShaderOpenGL::CreateShader(Shader::ShaderType type)
{
}

#endif