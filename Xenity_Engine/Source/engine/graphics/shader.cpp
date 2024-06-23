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

#pragma region Constructors / Destructor

void Shader::Init()
{
	canvasCameraTransformationMatrix = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));
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
		if (!Graphics::UseOpenGLFixedFunctions && Engine::IsRunning(true))
		{
			Engine::GetRenderer().DeleteShader(vertexShaderId);
			Engine::GetRenderer().DeleteShader(fragmentShaderId);

			if (useTessellation)
			{
				Engine::GetRenderer().DeleteShader(tessellationEvaluationShaderId);
				Engine::GetRenderer().DeleteShader(fragmentShaderId);
			}
			Engine::GetRenderer().DeleteShaderProgram(programId);
		}
	}
}

ReflectiveData Shader::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

ReflectiveData Shader::GetMetaReflectiveData()
{
	ReflectiveData reflectedVariables;
	return reflectedVariables;
}

void Shader::LoadFileReference()
{
	if (!isLoaded)
	{
		//Debug::Print("LoadFileReference()" + std::to_string(file->GetUniqueId())+ " " + std::to_string(fileId), true);
		isLoaded = true;
		if (Graphics::UseOpenGLFixedFunctions)
			return;

		const bool isOpen = file->Open(FileMode::ReadOnly);
		if (isOpen)
		{
			const std::string shaderText = file->ReadAll();
			file->Close();
			const size_t textSize = shaderText.size();

			if (textSize != 0)
			{
				int fragmentPos = -1;
				int fragmentStartPos = -1;

				int vertexPos = -1;
				int vertexStartPos = -1;
				bool foundPlatform = false;
				Platform currentPlatform = Platform::P_Windows;
#if defined(__vita__)
				currentPlatform = Platform::P_PsVita;
#endif
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
						if (currentPlatform == Platform::P_Windows)
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
	XASSERT(!shaderData.empty(), "[Shader::Compile] shaderData is empty")

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

	XASSERT(id != nullptr, "[Shader::Compile] Shader type not found, id is nullptr")

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
	Use();
	//Camera position
	if (Graphics::usedCamera != nullptr)
	{
		std::shared_ptr<Transform> transform = Graphics::usedCamera->GetTransform();
		Vector3 lookDirection = transform->GetForward();
		const Vector3& camPos = transform->GetPosition();
		lookDirection = lookDirection + camPos;

		float xAngle = transform->GetRotation().x;
		while (xAngle < -90)
		{
			xAngle += 360;
		}
		while (xAngle > 360 - 90)
		{
			xAngle -= 360;
		}

		const float angle = glm::radians(-transform->GetRotation().z);
		const glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));

		glm::mat4 camera;
		if (xAngle > 90 || xAngle < -90)
			camera = glm::lookAt(glm::vec3(-camPos.x, camPos.y, camPos.z), glm::vec3(-lookDirection.x, lookDirection.y, lookDirection.z), glm::vec3(0, -1, 0));
		else
			camera = glm::lookAt(glm::vec3(-camPos.x, camPos.y, camPos.z), glm::vec3(-lookDirection.x, lookDirection.y, lookDirection.z), glm::vec3(0, 1, 0));
		camera = rotationMatrix * camera;
		Engine::GetRenderer().SetShaderAttribut(programId, "camera", camera);
	}
}

/// <summary>
/// Send to the shader the 2D camera position
/// </summary>
void Shader::SetShaderCameraPositionCanvas()
{
	Use();
	Engine::GetRenderer().SetShaderAttribut(programId, "camera", canvasCameraTransformationMatrix);
}

/// <summary>
/// Send to the shader the 2D camera projection
/// </summary>
void Shader::SetShaderProjection()
{
	Use();
	Engine::GetRenderer().SetShaderAttribut(programId, "projection", Graphics::usedCamera->GetProjection());
}

void Shader::SetShaderProjectionCanvas()
{
	Use();
	Engine::GetRenderer().SetShaderAttribut(programId, "projection", Graphics::usedCamera->GetCanvasProjection());
}

/// <summary>
/// Send to the shader transform's model
/// </summary>
/// <param name="trans"></param>
void Shader::SetShaderModel(const glm::mat4& trans)
{
	Use();
	Engine::GetRenderer().SetShaderAttribut(programId, "model", trans);
}

/// <summary>
/// Send to the shader transform's model
/// </summary>
/// <param name="trans"></param>
void Shader::SetShaderModel(const Vector3& position, const Vector3& rotation, const Vector3& scale)
{
	Use();

	glm::mat4 transformationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-position.x, position.y, position.z));

	if (rotation.y != 0)
		transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.y * -1), glm::vec3(0.0f, 1.0f, 0.0f));
	if (rotation.x != 0)
		transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	if (rotation.z != 0)
		transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.z * -1), glm::vec3(0.0f, 0.0f, 1.0f));
	//if (scale.x != 1 || scale.y != 1|| scale.z != 1)
	transformationMatrix = glm::scale(transformationMatrix, glm::vec3(scale.x, scale.y, scale.z));

	Engine::GetRenderer().SetShaderAttribut(programId, "model", transformationMatrix);
}

void Shader::SetShaderAttribut(const char* attribut, const Vector4& value)
{
	Use();
	Engine::GetRenderer().SetShaderAttribut(programId, attribut, value);
}

void Shader::SetShaderAttribut(const char* attribut, const Vector3& value)
{
	Use();
	Engine::GetRenderer().SetShaderAttribut(programId, attribut, value);
}

void Shader::SetShaderAttribut(const char* attribut, const Vector2& value)
{
	Use();
	Engine::GetRenderer().SetShaderAttribut(programId, attribut, value);
}

void Shader::SetShaderAttribut(const char* attribut, float value)
{
	Use();
	Engine::GetRenderer().SetShaderAttribut(programId, attribut, value);
}

void Shader::SetShaderAttribut(const char* attribut, int value)
{
	Use();
	Engine::GetRenderer().SetShaderAttribut(programId, attribut, value);
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
}

/// <summary>
/// Send to the shader the point light data
/// </summary>
/// <param name="light">Point light</param>
/// <param name="index">Shader's point light index</param>
void Shader::SetPointLightData(const std::shared_ptr<Light>& light, const int index)
{
	XASSERT(light != nullptr, "[Shader::SetPointLightData] light is nullptr")

	const std::string baseString = "pointLights[" + std::to_string(index) + "].";

	const Vector4 lightColorV4 = light->color.GetRGBA().ToVector4();
	const Vector3 lightColor = Vector3(lightColorV4.x, lightColorV4.y, lightColorV4.z);
	Vector3 pos = light->GetTransform()->GetPosition();
	pos.x = -pos.x;
	SetShaderAttribut((baseString + "color").c_str(), lightColor * light->GetIntensity());
	SetShaderAttribut((baseString + "position").c_str(), pos);
	SetShaderAttribut((baseString + "constant").c_str(), lightConstant);
	SetShaderAttribut((baseString + "linear").c_str(), light->GetLinearValue());
	SetShaderAttribut((baseString + "quadratic").c_str(), light->GetQuadraticValue());
}

/// <summary>
/// Send to the shader the directional light data
/// </summary>
/// <param name="light">Directional light</param>
/// <param name="index">Shader's directional light index</param>
void Shader::SetDirectionalLightData(const std::shared_ptr<Light>& light, const int index)
{
	XASSERT(light != nullptr, "[Shader::SetDirectionalLightData] light is nullptr")

	static const std::string baseString = "directionalLights[" + std::to_string(index) + "].";

	const Vector4 lightColorV4 = light->color.GetRGBA().ToVector4();
	const Vector3 lightColor = Vector3(lightColorV4.x, lightColorV4.y, lightColorV4.z);

	SetShaderAttribut((baseString + "color").c_str(), light->GetIntensity() * lightColor);
	SetShaderAttribut((baseString + "direction").c_str(), light->GetTransform()->GetForward());
}

/// <summary>
/// Send to the shader the spot light data
/// </summary>
/// <param name="light">Spot light</param>
/// <param name="index">Shader's spot light index</param>
void Shader::SetSpotLightData(const std::shared_ptr<Light>& light, const int index)
{
	XASSERT(light != nullptr, "[Shader::SetSpotLightData] light is nullptr")

	const std::string baseString = "spotLights[" + std::to_string(index) + "].";
	const Vector4 lightColorV4 = light->color.GetRGBA().ToVector4();
	const Vector3 lightColor = Vector3(lightColorV4.x, lightColorV4.y, lightColorV4.z);

	Vector3 pos = light->GetTransform()->GetPosition();
	pos.x = -pos.x;

	SetShaderAttribut((baseString + "color").c_str(), light->GetIntensity() * lightColor);
	SetShaderAttribut((baseString + "position").c_str(), pos);
	Vector3 dir = light->GetTransform()->GetForward();
	dir.x = -dir.x;
	SetShaderAttribut((baseString + "direction").c_str(), dir);
	SetShaderAttribut((baseString + "constant").c_str(), lightConstant);
	SetShaderAttribut((baseString + "linear").c_str(), light->GetLinearValue());
	SetShaderAttribut((baseString + "quadratic").c_str(), light->GetQuadraticValue());
	SetShaderAttribut((baseString + "cutOff").c_str(), glm::cos(glm::radians(light->GetSpotAngle() * light->GetSpotSmoothness())));
	SetShaderAttribut((baseString + "outerCutOff").c_str(), glm::cos(glm::radians(light->GetSpotAngle())));
}

/// <summary>
/// Send lights data to the shader
/// </summary>
void Shader::UpdateLights(bool disableLights)
{
	int directionalUsed = 0;
	int pointUsed = 0;
	int spotUsed = 0;
	if (disableLights)
	{
		const int lightCount = AssetManager::GetLightCount();

		//For each lights
		for (int lightI = 0; lightI < lightCount; lightI++)
		{
			std::shared_ptr<Light> light = AssetManager::GetLight(lightI).lock();
			if (light->GetIsEnabled() && light->GetGameObject()->GetLocalActive())
			{
				if (light->type == LightType::Directional)
				{
					SetDirectionalLightData(light, directionalUsed);
					directionalUsed++;
				}
				else if (light->type == LightType::Point)
				{
					SetPointLightData(light, pointUsed);
					pointUsed++;
				}
				else if (light->type == LightType::Spot)
				{
					SetSpotLightData(light, spotUsed);
					spotUsed++;
				}
			}
		}
	}

	SetShaderAttribut("usedPointLightCount", pointUsed);
	SetShaderAttribut("usedSpotLightCount", spotUsed);
	SetShaderAttribut("usedDirectionalLightCount", directionalUsed);
}

std::shared_ptr<Shader> Shader::MakeShader()
{
	std::shared_ptr<Shader> newFileRef = std::make_shared<Shader>();
	AssetManager::AddFileReference(newFileRef);
	return newFileRef;
}

#pragma endregion