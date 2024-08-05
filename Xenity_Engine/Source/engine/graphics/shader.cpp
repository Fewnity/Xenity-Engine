// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#define _USE_MATH_DEFINES
#if defined(__PSP__) || defined(__vita__)
#undef __STRICT_ANSI__
#endif

#include "shader.h"

#include "graphics.h"
#include "renderer/renderer.h"
#include "camera.h"

#include <engine/lighting/lighting.h>

#include <engine/engine.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameobject.h>

#include <engine/debug/debug.h>
#include <engine/tools/string_tag_finder.h>

#include <engine/file_system/file.h>

#include <engine/game_elements/transform.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>
#include <cmath>

#include <json.hpp>


using json = nlohmann::json;

glm::mat4 Shader::canvasCameraTransformationMatrix;
std::vector<Shader::PointLightVariableNames*> Shader::pointlightVariableNames;
std::vector<Shader::DirectionalLightsVariableNames*> Shader::directionallightVariableNames;
std::vector<Shader::SpotLightVariableNames*> Shader::spotlightVariableNames;

Shader::PointLightVariableIds::PointLightVariableIds(int index, unsigned int programId)
{
	color = Engine::GetRenderer().GetShaderUniformLocation(programId, pointlightVariableNames[index]->color);
	position = Engine::GetRenderer().GetShaderUniformLocation(programId, pointlightVariableNames[index]->position);
	constant = Engine::GetRenderer().GetShaderUniformLocation(programId, pointlightVariableNames[index]->constant);
	linear = Engine::GetRenderer().GetShaderUniformLocation(programId, pointlightVariableNames[index]->linear);
	quadratic = Engine::GetRenderer().GetShaderUniformLocation(programId, pointlightVariableNames[index]->quadratic);

	// Initialize values
	Engine::GetRenderer().SetShaderAttribut(programId, color, Vector3(0, 0, 0));
	Engine::GetRenderer().SetShaderAttribut(programId, position, Vector3(0, 0, 0));
	Engine::GetRenderer().SetShaderAttribut(programId, constant, 1.0f);
	Engine::GetRenderer().SetShaderAttribut(programId, linear, 0.0f);
	Engine::GetRenderer().SetShaderAttribut(programId, quadratic, 0.0f);
}

Shader::DirectionalLightsVariableIds::DirectionalLightsVariableIds(int index, unsigned int programId)
{
	color = Engine::GetRenderer().GetShaderUniformLocation(programId, directionallightVariableNames[index]->color);
	direction = Engine::GetRenderer().GetShaderUniformLocation(programId, directionallightVariableNames[index]->direction);

	// Initialize values
	Engine::GetRenderer().SetShaderAttribut(programId, color, Vector3(0, 0, 0));
	Engine::GetRenderer().SetShaderAttribut(programId, direction, Vector3(0, 0, 0));
}

Shader::SpotLightVariableIds::SpotLightVariableIds(int index, unsigned int programId)
{
	color = Engine::GetRenderer().GetShaderUniformLocation(programId, spotlightVariableNames[index]->color);
	position = Engine::GetRenderer().GetShaderUniformLocation(programId, spotlightVariableNames[index]->position);
	direction = Engine::GetRenderer().GetShaderUniformLocation(programId, spotlightVariableNames[index]->direction);
	constant = Engine::GetRenderer().GetShaderUniformLocation(programId, spotlightVariableNames[index]->constant);
	linear = Engine::GetRenderer().GetShaderUniformLocation(programId, spotlightVariableNames[index]->linear);
	quadratic = Engine::GetRenderer().GetShaderUniformLocation(programId, spotlightVariableNames[index]->quadratic);
	cutOff = Engine::GetRenderer().GetShaderUniformLocation(programId, spotlightVariableNames[index]->cutOff);
	outerCutOff = Engine::GetRenderer().GetShaderUniformLocation(programId, spotlightVariableNames[index]->outerCutOff);

	// Initialize values
	Engine::GetRenderer().SetShaderAttribut(programId, color, Vector3(0, 0, 0));
	Engine::GetRenderer().SetShaderAttribut(programId, position, Vector3(0, 0, 0));
	Engine::GetRenderer().SetShaderAttribut(programId, direction, Vector3(0, 0, 0));
	Engine::GetRenderer().SetShaderAttribut(programId, constant, 1.0f);
	Engine::GetRenderer().SetShaderAttribut(programId, linear, 0.0f);
	Engine::GetRenderer().SetShaderAttribut(programId, quadratic, 0.0f);
	Engine::GetRenderer().SetShaderAttribut(programId, cutOff, 0.0f);
	Engine::GetRenderer().SetShaderAttribut(programId, outerCutOff, 0.0f);
}

Shader::PointLightVariableNames::PointLightVariableNames(int index)
{
	constexpr int bufferSize = 30;

	color = new char[bufferSize];
	position = new char[bufferSize];
	constant = new char[bufferSize];
	linear = new char[bufferSize];
	quadratic = new char[bufferSize];

#if defined(_WIN32) || defined(_WIN64)
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

Shader::PointLightVariableNames::~PointLightVariableNames()
{
	delete[] color;
	delete[] position;
	delete[] constant;
	delete[] linear;
	delete[] quadratic;
}

Shader::DirectionalLightsVariableNames::DirectionalLightsVariableNames(int index)
{
	constexpr int bufferSize = 35;

	color = new char[bufferSize];
	direction = new char[bufferSize];

#if defined(_WIN32) || defined(_WIN64)
	sprintf_s(color, bufferSize, "directionalLights[%d].color", index);
	sprintf_s(direction, bufferSize, "directionalLights[%d].direction", index);
#else
	sprintf(color, "directionalLights[%d].color", index);
	sprintf(direction, "directionalLights[%d].direction", index);
#endif
}

Shader::DirectionalLightsVariableNames::~DirectionalLightsVariableNames()
{
	delete[] color;
	delete[] direction;
}

Shader::SpotLightVariableNames::SpotLightVariableNames(int index)
{
	constexpr int bufferSize = 30;

	color = new char[bufferSize];
	position = new char[bufferSize];
	direction = new char[bufferSize];
	constant = new char[bufferSize];
	linear = new char[bufferSize];
	quadratic = new char[bufferSize];
	cutOff = new char[bufferSize];
	outerCutOff = new char[bufferSize];

#if defined(_WIN32) || defined(_WIN64)
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

Shader::SpotLightVariableNames::~SpotLightVariableNames()
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

#pragma region Constructors / Destructor

void Shader::Init()
{
	canvasCameraTransformationMatrix = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));

	for (int i = 0; i < 10; i++)
	{
		pointlightVariableNames.push_back(new PointLightVariableNames(i));
		directionallightVariableNames.push_back(new DirectionalLightsVariableNames(i));
		spotlightVariableNames.push_back(new SpotLightVariableNames(i));
	}
}

Shader::Shader()
{
}

/// <summary>
/// Shader destructor
/// </summary>
Shader::~Shader()
{
	if (isLoaded)
	{
		if constexpr (!Graphics::UseOpenGLFixedFunctions)
		{
			if (Engine::IsRunning(true))
			{
				Engine::GetRenderer().DeleteShader(vertexShaderId);
				Engine::GetRenderer().DeleteShader(fragmentShaderId);
				if (useTessellation)
				{
					Engine::GetRenderer().DeleteShader(tessellationEvaluationShaderId);
					Engine::GetRenderer().DeleteShader(fragmentShaderId);
				}
				// This cause the psvita to crash, bug in older version of vitaGL, fixed in the latest version but the latest version is running very slow
				//Engine::GetRenderer().DeleteShaderProgram(programId);
			}
		}
	}
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
	if (!isLoaded)
	{
		isLoaded = true;
		if constexpr (Graphics::UseOpenGLFixedFunctions)
			return;

		bool openResult = true;
#if defined(EDITOR)
		openResult = file->Open(FileMode::ReadOnly);
#endif
		if (openResult)
		{
			std::string shaderText;

#if defined(EDITOR)
			shaderText = file->ReadAll();
			file->Close();
#else
			unsigned char* binData = ProjectManager::fileDataBase.bitFile.ReadBinary(filePosition, fileSize);
			shaderText = std::string(reinterpret_cast<const char*>(binData), fileSize);
			free(binData);
#endif

			const size_t textSize = shaderText.size();

			if (textSize != 0)
			{
				int fragmentPos = -1;
				int fragmentStartPos = -1;

				int vertexPos = -1;
				int vertexStartPos = -1;
				bool foundPlatform = false;
				Platform currentPlatform = Application::GetPlatform();

				int unused = 0;
				int end = 0;
				bool foundEnd = false;

				for (size_t i = 0; i < textSize - 1; i++)
				{
					if (StringTagFinder::FindTag(shaderText, i, textSize, "{pc}", unused, end))
					{
						if (foundPlatform)
						{
							foundEnd = true;
							break;
						}
						if (currentPlatform == Platform::P_Windows || currentPlatform == Platform::P_Linux)
						{
							foundPlatform = true;
						}
					}
					else if (StringTagFinder::FindTag(shaderText, i, textSize, "{psvita}", unused, end))
					{
						if (foundPlatform)
						{
							foundEnd = true;
							break;
						}
						if (currentPlatform == Platform::P_PsVita)
						{
							foundPlatform = true;
						}
					}
					else if (foundPlatform && StringTagFinder::FindTag(shaderText, i, textSize, "{fragment}", fragmentPos, fragmentStartPos))
					{
					}
					else if (foundPlatform && StringTagFinder::FindTag(shaderText, i, textSize, "{vertex}", vertexPos, vertexStartPos))
					{
					}
				}

				if (vertexPos != -1 && fragmentPos != -1)
				{
					std::string fragShaderData;
					if (foundEnd)
						fragShaderData = shaderText.substr(fragmentStartPos, end - fragmentStartPos);
					else
						fragShaderData = shaderText.substr(fragmentStartPos);

					std::string vertexShaderData = shaderText.substr(vertexStartPos, fragmentPos - vertexStartPos);

					bool vertexRet = Compile(vertexShaderData, ShaderType::Vertex_Shader);
					bool fragRet = Compile(fragShaderData, ShaderType::Fragment_Shader);

					if (vertexRet && fragRet)
					{
						Link();
					}
					else
					{
						Debug::PrintError("[Shader::LoadFileReference] Cannot link the shader, the compilation has failed: " + file->GetPath(), true);
						isLoaded = false;
					}

					//useTessellation = true;
					//LoadShader(tessellationEvaluationShaderPath, Tessellation_Evaluation_Shader);
					//LoadShader(fragmentShaderPath, Fragment_Shader);
				}
				else
				{
					Debug::PrintError("[Shader::LoadFileReference] The shader structure is wrong: " + file->GetPath(), true);
					isLoaded = false;
				}
			}
			else
			{
				Debug::PrintError("[Shader::LoadFileReference] The shader file is empty: " + file->GetPath(), true);
				isLoaded = false;
			}
		}
		else
		{
			Debug::PrintError("[Shader::LoadFileReference] Fail to load the shader file: " + file->GetPath(), true);
			isLoaded = false;
		}
	}
}

#pragma endregion

#pragma region Getters

unsigned int Shader::GetProgramId() const
{
	return this->programId;
}

#pragma endregion

/// <summary>
/// Use shader
/// </summary>
bool Shader::Use()
{
	if (Graphics::currentShader != shared_from_this())
	{
		Engine::GetRenderer().UseShaderProgram(programId);
		Graphics::currentShader = std::dynamic_pointer_cast<Shader>(shared_from_this());
		return true;
	}
	return false;
}

#pragma region Data loading

bool Shader::Compile(const std::string& shaderData, ShaderType type)
{
	XASSERT(!shaderData.empty(), "[Shader::Compile] shaderData is empty");

	const char* shaderDataConst = shaderData.c_str();

	unsigned int* id = nullptr;

	switch (type)
	{
	case ShaderType::Vertex_Shader:
		id = &vertexShaderId;
		break;
	case ShaderType::Fragment_Shader:
		id = &fragmentShaderId;
		break;
	case ShaderType::Tessellation_Control_Shader:
		id = &tessellationShaderId;
		break;
	case ShaderType::Tessellation_Evaluation_Shader:
		id = &tessellationEvaluationShaderId;
		break;
	}

	XASSERT(id != nullptr, "[Shader::Compile] Shader type not found, id is nullptr");

	//Compile
	*id = Engine::GetRenderer().CreateShader(type);
	Engine::GetRenderer().SetShaderData(*id, shaderDataConst);
	Engine::GetRenderer().CompileShader(*id);

	int vResult = Engine::GetRenderer().GetShaderCompilationResult(*id);

	//On error
	if (vResult == 0)
	{
		std::vector<char> errorLog = Engine::GetRenderer().GetCompilationError(*id);

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
void Shader::SetShaderCameraPosition()
{
	//Camera position
	if (Graphics::usedCamera != nullptr)
	{
		std::shared_ptr<Transform> transform = Graphics::usedCamera->GetTransform();

		const Vector3& position = transform->GetPosition();

		const Quaternion& baseQ = transform->GetRotation();
		Quaternion offsetQ = Quaternion::Euler(0, 180, 0);
		Quaternion newQ = baseQ * offsetQ;

		glm::mat4 RotationMatrix = glm::toMat4(glm::quat(newQ.w, -newQ.x, newQ.y, newQ.z));

		if (position.x != 0.0f || position.y != 0.0f || position.z != 0.0f)
			RotationMatrix = glm::translate(RotationMatrix, glm::vec3(position.x, -position.y, -position.z));

		Engine::GetRenderer().SetShaderAttribut(programId, cameraLocation, RotationMatrix); // Y position and rotation inverted
	}
}

/// <summary>
/// Send to the shader the 2D camera position
/// </summary>
void Shader::SetShaderCameraPositionCanvas()
{
	Engine::GetRenderer().SetShaderAttribut(programId, cameraLocation, canvasCameraTransformationMatrix);
}

/// <summary>
/// Send to the shader the 2D camera projection
/// </summary>
void Shader::SetShaderProjection()
{
	Engine::GetRenderer().SetShaderAttribut(programId, projectionLocation, Graphics::usedCamera->GetProjection());
}

void Shader::SetShaderProjectionCanvas()
{
	Engine::GetRenderer().SetShaderAttribut(programId, projectionLocation, Graphics::usedCamera->GetCanvasProjection());
}

/// <summary>
/// Send to the shader transform's model
/// </summary>
/// <param name="trans"></param>
void Shader::SetShaderModel(const glm::mat4& trans)
{
	//Use();
	Engine::GetRenderer().SetShaderAttribut(programId, modelLocation, trans);
}

/// <summary>
/// Send to the shader transform's model
/// </summary>
/// <param name="trans"></param>
void Shader::SetShaderModel(const Vector3& position, const Vector3& rotation, const Vector3& scale)
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

	Engine::GetRenderer().SetShaderAttribut(programId, modelLocation, transformationMatrix);
}

void Shader::SetShaderAttribut(const std::string& attribut, const Vector4& value)
{
	auto it = uniformsIds.find(attribut);
	if (it == uniformsIds.end())
	{
		unsigned int id = Engine::GetRenderer().GetShaderUniformLocation(programId, attribut.c_str());
		it = uniformsIds.emplace(attribut, id).first;
	}
	Engine::GetRenderer().SetShaderAttribut(programId, it->second, value);
}

void Shader::SetShaderAttribut(const std::string& attribut, const Vector3& value)
{
	auto it = uniformsIds.find(attribut);
	if (it == uniformsIds.end())
	{
		unsigned int id = Engine::GetRenderer().GetShaderUniformLocation(programId, attribut.c_str());
		it = uniformsIds.emplace(attribut, id).first;
	}
	Engine::GetRenderer().SetShaderAttribut(programId, it->second, value);
}

void Shader::SetShaderAttribut(const std::string& attribut, const Vector2& value)
{
	auto it = uniformsIds.find(attribut);
	if (it == uniformsIds.end())
	{
		unsigned int id = Engine::GetRenderer().GetShaderUniformLocation(programId, attribut.c_str());
		it = uniformsIds.emplace(attribut, id).first;
	}
	Engine::GetRenderer().SetShaderAttribut(programId, it->second, value);
}

void Shader::SetShaderAttribut(const std::string& attribut, float value)
{
	auto it = uniformsIds.find(attribut);
	if (it == uniformsIds.end())
	{
		unsigned int id = Engine::GetRenderer().GetShaderUniformLocation(programId, attribut.c_str());
		it = uniformsIds.emplace(attribut, id).first;
	}
	Engine::GetRenderer().SetShaderAttribut(programId, it->second, value);
}

void Shader::SetShaderAttribut(const std::string& attribut, int value)
{
	auto it = uniformsIds.find(attribut);
	if (it == uniformsIds.end())
	{
		unsigned int id = Engine::GetRenderer().GetShaderUniformLocation(programId, attribut.c_str());
		it = uniformsIds.emplace(attribut, id).first;
	}
	Engine::GetRenderer().SetShaderAttribut(programId, it->second, value);
}


void Shader::Link()
{
	programId = Engine::GetRenderer().CreateShaderProgram();
	Engine::GetRenderer().AttachShader(programId, vertexShaderId);

	if (useTessellation)
	{
		Engine::GetRenderer().AttachShader(programId, tessellationShaderId);
		Engine::GetRenderer().AttachShader(programId, tessellationEvaluationShaderId);
	}
	Engine::GetRenderer().AttachShader(programId, fragmentShaderId);

	Engine::GetRenderer().LinkShaderProgram(programId);
	Engine::GetRenderer().UseShaderProgram(programId);

	modelLocation = Engine::GetRenderer().GetShaderUniformLocation(programId, "model");
	projectionLocation = Engine::GetRenderer().GetShaderUniformLocation(programId, "projection");
	cameraLocation = Engine::GetRenderer().GetShaderUniformLocation(programId, "camera");
	ambientLightLocation = Engine::GetRenderer().GetShaderUniformLocation(programId, "ambientLight");

	usedPointLightCountLocation = Engine::GetRenderer().GetShaderUniformLocation(programId, "usedPointLightCount");
	usedSpotLightCountLocation = Engine::GetRenderer().GetShaderUniformLocation(programId, "usedSpotLightCount");
	usedDirectionalLightCountLocation = Engine::GetRenderer().GetShaderUniformLocation(programId, "usedDirectionalLightCount");

	for (int i = 0; i < 10; i++)
	{
		pointlightVariableIds.push_back(new PointLightVariableIds(i, programId));
		directionallightVariableIds.push_back(new DirectionalLightsVariableIds(i, programId));
		spotlightVariableIds.push_back(new SpotLightVariableIds(i, programId));
	}
}

/// <summary>
/// Send to the shader the point light data
/// </summary>
/// <param name="light">Point light</param>
/// <param name="index">Shader's point light index</param>
void Shader::SetPointLightData(const Light& light, const int index)
{
	const Vector4 lightColorV4 = light.color.GetRGBA().ToVector4();
	const Vector3 lightColor = Vector3(lightColorV4.x, lightColorV4.y, lightColorV4.z);
	Vector3 pos = light.GetTransform()->GetPosition();
	pos.x = -pos.x;

	const PointLightVariableIds* ids = pointlightVariableIds[index];
	Engine::GetRenderer().SetShaderAttribut(programId, ids->color, lightColor * light.GetIntensity());
	Engine::GetRenderer().SetShaderAttribut(programId, ids->position, pos);
	Engine::GetRenderer().SetShaderAttribut(programId, ids->constant, lightConstant);
	Engine::GetRenderer().SetShaderAttribut(programId, ids->linear, light.GetLinearValue());
	Engine::GetRenderer().SetShaderAttribut(programId, ids->quadratic, light.GetQuadraticValue());
}

/// <summary>
/// Send to the shader the directional light data
/// </summary>
/// <param name="light">Directional light</param>
/// <param name="index">Shader's directional light index</param>
void Shader::SetDirectionalLightData(const Light& light, const int index)
{
	const Vector4 lightColorV4 = light.color.GetRGBA().ToVector4();
	const Vector3 lightColor = Vector3(lightColorV4.x, lightColorV4.y, lightColorV4.z);

	Vector3 dir = light.GetTransform()->GetForward();
	dir.x = -dir.x;

	const DirectionalLightsVariableIds* ids = directionallightVariableIds[index];
	Engine::GetRenderer().SetShaderAttribut(programId, ids->color, lightColor * light.GetIntensity());
	Engine::GetRenderer().SetShaderAttribut(programId, ids->direction, dir);
}

void Shader::SetAmbientLightData(const Vector3& color)
{
	Engine::GetRenderer().SetShaderAttribut(programId, ambientLightLocation, color);
}

/// <summary>
/// Send to the shader the spot light data
/// </summary>
/// <param name="light">Spot light</param>
/// <param name="index">Shader's spot light index</param>
void Shader::SetSpotLightData(const Light& light, const int index)
{
	const Vector4 lightColorV4 = light.color.GetRGBA().ToVector4();
	const Vector3 lightColor = Vector3(lightColorV4.x, lightColorV4.y, lightColorV4.z);

	Vector3 pos = light.GetTransform()->GetPosition();
	pos.x = -pos.x;

	Vector3 dir = light.GetTransform()->GetForward();
	dir.x = -dir.x;

	const SpotLightVariableIds* ids = spotlightVariableIds[index];
	Engine::GetRenderer().SetShaderAttribut(programId, ids->color, lightColor * light.GetIntensity());
	Engine::GetRenderer().SetShaderAttribut(programId, ids->position, pos);
	Engine::GetRenderer().SetShaderAttribut(programId, ids->direction, dir);
	Engine::GetRenderer().SetShaderAttribut(programId, ids->constant, lightConstant);
	Engine::GetRenderer().SetShaderAttribut(programId, ids->linear, light.GetLinearValue());
	Engine::GetRenderer().SetShaderAttribut(programId, ids->quadratic, light.GetQuadraticValue());
	Engine::GetRenderer().SetShaderAttribut(programId, ids->cutOff, glm::cos(glm::radians(light.GetSpotAngle() * (1 - light.GetSpotSmoothness()))));
	Engine::GetRenderer().SetShaderAttribut(programId, ids->outerCutOff, glm::cos(glm::radians(light.GetSpotAngle())));
}

/// <summary>
/// Send lights data to the shader
/// </summary>
void Shader::UpdateLights(bool disableLights)
{
	Engine::GetRenderer().SetShaderAttribut(programId, pointlightVariableIds[0]->color, Vector3(0, 0, 0));
	Engine::GetRenderer().SetShaderAttribut(programId, spotlightVariableIds[0]->color, Vector3(0, 0, 0));
	Engine::GetRenderer().SetShaderAttribut(programId, directionallightVariableIds[0]->color, Vector3(0, 0, 0));

	int directionalUsed = 0;
	int pointUsed = 0;
	int spotUsed = 0;
	Vector4 ambientLight = Vector4(0, 0, 0, 0);

	if (disableLights)
	{
		const int lightCount = AssetManager::GetLightCount();

		//For each lights
		for (int lightI = 0; lightI < lightCount; lightI++)
		{
			const Light& light = *AssetManager::GetLight(lightI).lock();
			if (light.IsEnabled() && light.GetGameObject()->IsLocalActive())
			{
				if (light.type == LightType::Directional)
				{
					SetDirectionalLightData(light, directionalUsed);
					directionalUsed++;
				}
				else if (light.type == LightType::Point)
				{
					SetPointLightData(light, pointUsed);
					pointUsed++;
				}
				else if (light.type == LightType::Spot)
				{
					SetSpotLightData(light, spotUsed);
					spotUsed++;
				}
				else if (light.type == LightType::Ambient)
				{
					ambientLight += light.color.GetRGBA().ToVector4() * light.intensity;
				}
			}
		}
	}

	SetAmbientLightData(Vector3(ambientLight.x, ambientLight.y, ambientLight.z));
	Engine::GetRenderer().SetShaderAttribut(programId, usedPointLightCountLocation, pointUsed);
	Engine::GetRenderer().SetShaderAttribut(programId, usedSpotLightCountLocation, spotUsed);
	Engine::GetRenderer().SetShaderAttribut(programId, usedDirectionalLightCountLocation, directionalUsed);
}

std::shared_ptr<Shader> Shader::MakeShader()
{
	std::shared_ptr<Shader> newFileRef = std::make_shared<Shader>();
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}

#pragma endregion