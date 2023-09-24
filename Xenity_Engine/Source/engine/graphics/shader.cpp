#define _USE_MATH_DEFINES
#if defined(__PSP__) || defined(__vita__)
#undef __STRICT_ANSI__
#endif
#include <cmath>

#include "shader.h"
#include "../../xenity.h"
#include "../graphics/renderer/renderer.h"

#include <iostream>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <json.hpp>

using json = nlohmann::json;

#pragma region Constructors / Destructor

Shader::Shader()
{
}

/// <summary>
/// Shader destructor
/// </summary>
Shader::~Shader()
{
	Engine::renderer->DeleteShader(vertexShaderId);
	Engine::renderer->DeleteShader(fragmentShaderId);

	if (useTessellation)
	{
		Engine::renderer->DeleteShader(tessellationEvaluationShaderId);
		Engine::renderer->DeleteShader(fragmentShaderId);
	}
	Engine::renderer->DeleteShaderProgram(programId);
}

std::unordered_map<std::string, ReflectionEntry> Shader::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	return reflectedVariables;
}

std::unordered_map<std::string, ReflectionEntry> Shader::GetMetaReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	return reflectedVariables;
}

void Shader::LoadFileReference()
{
	file->Open(false);
	std::string shaderText = file->ReadAll();
	file->Close();
	int textSize = shaderText.size();

	int fragmentPos = -1;
	int fragmentStartPos = -1;

	int vertexPos = -1;
	int vertexStartPos = -1;

	if (textSize != 0)
	{
		for (int i = 0; i < textSize - 1; i++)
		{
			if (shaderText[i] == '{' && shaderText[i + 1] == 'f')
			{
				fragmentPos = i;
				for (int j = i + 1; j < textSize; j++)
				{
					if (shaderText[j] == '}')
					{
						fragmentStartPos = j + 2;
						break;
					}
				}
			}
			else if (shaderText[i] == '{' && shaderText[i + 1] == 'v')
			{
				for (int j = i + 1; j < textSize; j++)
				{
					vertexPos = i;
					if (shaderText[j] == '}')
					{
						vertexStartPos = j + 2;
						break;
					}
				}
			}
		}

		if (vertexPos != -1 && fragmentPos != -1)
		{
			std::string fragShaderData = shaderText.substr(fragmentStartPos);
			std::string vertexShaderData = shaderText.substr(vertexStartPos, fragmentPos - vertexStartPos);

			LoadShader(vertexShaderData, ShaderType::Vertex_Shader);
			LoadShader(fragShaderData, ShaderType::Fragment_Shader);

			//useTessellation = true;
			//LoadShader(tessellationEvaluationShaderPath, Tessellation_Evaluation_Shader);
			//LoadShader(fragmentShaderPath, Fragment_Shader);

			BuildShader();
		}
	}
}

#pragma endregion

#pragma region Getters

unsigned int Shader::GetProgramId()
{
	return this->programId;
}

#pragma endregion

unsigned int usedShaderProgram = 0;

/// <summary>
/// Use shader
/// </summary>
bool Shader::Use()
{
	if (Graphics::currentShader != shared_from_this())
	{
		Engine::renderer->UseShaderProgram(programId);
		Graphics::currentShader = std::dynamic_pointer_cast<Shader>(shared_from_this());
		return true;
	}
	return false;
}

#pragma region Data loading

void Shader::LoadShader(const std::string& shaderData, ShaderType type)
{
	const char* shaderDataConst = shaderData.c_str();

	unsigned int* id = nullptr;

	switch (type)
	{
	case Shader::Vertex_Shader:
		id = &vertexShaderId;
		break;
	case Shader::Fragment_Shader:
		id = &fragmentShaderId;
		break;
	case Shader::Tessellation_Control_Shader:
		id = &tessellationShaderId;
		break;
	case Shader::Tessellation_Evaluation_Shader:
		id = &tessellationEvaluationShaderId;
		break;
	}

	if (id != nullptr)
	{
		//Compile
		*id = Engine::renderer->CreateShader(type);
		Engine::renderer->SetShaderData(*id, shaderDataConst);
		Engine::renderer->CompileShader(*id);

		int vResult = Engine::renderer->GetShaderCompilationResult(*id);

		//On error
		if (vResult == 0)
		{
			std::vector<char> errorLog = Engine::renderer->GetCompilationError(*id);

			std::string shaderError = "Compilation error: ";
			switch (type)
			{
			case Shader::Vertex_Shader:
				shaderError += "Vertex";
				break;
			case Shader::Fragment_Shader:
				shaderError += "Fragment";
				break;
			case Shader::Tessellation_Control_Shader:
				shaderError += "Tessellation control";
				break;
			case Shader::Tessellation_Evaluation_Shader:
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
		}
	}
	else
	{
		std::string shaderError = "Compilation error: Shader type not found. File path: " + shaderData;
		Debug::PrintError(shaderError);
	}
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
	if (Graphics::usedCamera.lock() != nullptr)
	{
		std::shared_ptr<Transform> transform = Graphics::usedCamera.lock()->GetTransform();
		Vector3 lookDirection = transform->GetForward();
		Vector3 camPos = transform->GetPosition();
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

		glm::mat4 camera;
		if (xAngle > 90 || xAngle < -90)
			camera = glm::lookAt(glm::vec3(-camPos.x, camPos.y, camPos.z), glm::vec3(-lookDirection.x, lookDirection.y, lookDirection.z), glm::vec3(0, -1, 0));
		else
			camera = glm::lookAt(glm::vec3(-camPos.x, camPos.y, camPos.z), glm::vec3(-lookDirection.x, lookDirection.y, lookDirection.z), glm::vec3(0, 1, 0));

		Engine::renderer->SetShaderAttribut(programId, "camera", camera);
	}
}

/// <summary>
/// Send to the shader the 2D camera position
/// </summary>
void Shader::SetShaderCameraPositionCanvas()
{
	Use();

	glm::mat4 camera;
	camera = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));

	Engine::renderer->SetShaderAttribut(programId, "camera", camera);
}

/// <summary>
/// Send to the shader the 2D camera projection
/// </summary>
void Shader::SetShaderProjection()
{
	Use();
	Engine::renderer->SetShaderAttribut(programId, "projection", Graphics::usedCamera.lock()->GetProjection());
}

void Shader::SetShaderProjectionCanvas()
{
	Use();
	float size = 5;
	float halfAspect = Graphics::usedCamera.lock()->GetAspectRatio() / 2.0f * 10 * size / 5.0f;
	float halfOne = 0.5f * 10 * size / 5.0f;
	glm::mat4 projection = glm::orthoZO(-halfAspect, halfAspect, -halfOne, halfOne, 0.03f, 100.0f);
	Engine::renderer->SetShaderAttribut(programId, "projection", projection);
}

/// <summary>
/// Send to the shader transform's model
/// </summary>
/// <param name="trans"></param>
void Shader::SetShaderModel(const glm::mat4& trans)
{
	Use();
	Engine::renderer->SetShaderAttribut(programId, "model", trans);
}

/// <summary>
/// Send to the shader transform's model
/// </summary>
/// <param name="trans"></param>
void Shader::SetShaderModel(const Vector3& position, const Vector3& rotation, const Vector3& scale)
{
	Use();
	glm::mat4 transformationMatrix = glm::mat4(1.0f);
	transformationMatrix = glm::translate(transformationMatrix, glm::vec3(-position.x, position.y, position.z));

	transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.y * -1), glm::vec3(0.0f, 1.0f, 0.0f));
	transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.z * -1), glm::vec3(0.0f, 0.0f, 1.0f));
	transformationMatrix = glm::scale(transformationMatrix, glm::vec3(scale.x, scale.y, scale.z));
	Engine::renderer->SetShaderAttribut(programId, "model", transformationMatrix);
}

void Shader::SetShaderAttribut(const char* attribut, const Vector4& value)
{
	Use();
	Engine::renderer->SetShaderAttribut(programId, attribut, value);
}

void Shader::SetShaderAttribut(const char* attribut, const Vector3& value)
{
	Use();
	Engine::renderer->SetShaderAttribut(programId, attribut, value);
}

void Shader::SetShaderAttribut(const char* attribut, const Vector2& value)
{
	Use();
	Engine::renderer->SetShaderAttribut(programId, attribut, value);
}

void Shader::SetShaderAttribut(const char* attribut, float value)
{
	Use();
	Engine::renderer->SetShaderAttribut(programId, attribut, value);
}

void Shader::SetShaderAttribut(const char* attribut, int value)
{
	Use();
	Engine::renderer->SetShaderAttribut(programId, attribut, value);
}


void Shader::BuildShader()
{
	programId = Engine::renderer->CreateShaderProgram();
	Engine::renderer->AttachShader(programId, vertexShaderId);

	if (useTessellation)
	{
		Engine::renderer->AttachShader(programId, tessellationShaderId);
		Engine::renderer->AttachShader(programId, tessellationEvaluationShaderId);
	}
	Engine::renderer->AttachShader(programId, fragmentShaderId);

	Engine::renderer->LinkShaderProgram(programId);
	Engine::renderer->UseShaderProgram(programId);

	modelLocation = Engine::renderer->GetShaderUniformLocation(programId, "model");
	projectionLocation = Engine::renderer->GetShaderUniformLocation(programId, "projection");
}

/// <summary>
/// Send to the shader the point light data
/// </summary>
/// <param name="light">Point light</param>
/// <param name="index">Shader's point light index</param>
void Shader::SetPointLightData(std::shared_ptr<Light> light, const int index)
{
	std::string baseString = "pointLights[" + std::to_string(index) + "].";

	Vector4 lightColorV4 = light->color.GetRGBA().ToVector4();
	Vector3 lightColor = Vector3(lightColorV4.x, lightColorV4.y, lightColorV4.z);
	Vector3 pos = light->GetTransform()->GetPosition();
	pos.x = -pos.x;
	SetShaderAttribut((baseString + "color").c_str(), lightColor * light->intensity);
	SetShaderAttribut((baseString + "position").c_str(), pos);
	SetShaderAttribut((baseString + "constant").c_str(), lightConstant);
	SetShaderAttribut((baseString + "linear").c_str(), light->linear);
	SetShaderAttribut((baseString + "quadratic").c_str(), light->quadratic);
}

/// <summary>
/// Send to the shader the directional light data
/// </summary>
/// <param name="light">Directional light</param>
/// <param name="index">Shader's directional light index</param>
void Shader::SetDirectionalLightData(std::shared_ptr<Light> light, const int index)
{
	std::string baseString = "directionalLights[" + std::to_string(index) + "].";

	Vector4 lightColorV4 = light->color.GetRGBA().ToVector4();
	Vector3 lightColor = Vector3(lightColorV4.x, lightColorV4.y, lightColorV4.z);

	SetShaderAttribut((baseString + "color").c_str(), light->intensity * lightColor);
	SetShaderAttribut((baseString + "direction").c_str(), light->GetTransform()->GetForward());
}

/// <summary>
/// Send to the shader the spot light data
/// </summary>
/// <param name="light">Spot light</param>
/// <param name="index">Shader's spot light index</param>
void Shader::SetSpotLightData(std::shared_ptr<Light> light, const int index)
{
	std::string baseString = "spotLights[" + std::to_string(index) + "].";
	Vector4 lightColorV4 = light->color.GetRGBA().ToVector4();
	Vector3 lightColor = Vector3(lightColorV4.x, lightColorV4.y, lightColorV4.z);

	Vector3 pos = light->GetTransform()->GetPosition();
	pos.x = -pos.x;

	SetShaderAttribut((baseString + "color").c_str(), light->intensity * lightColor);
	SetShaderAttribut((baseString + "position").c_str(), pos);
	SetShaderAttribut((baseString + "direction").c_str(), light->GetTransform()->GetForward());
	SetShaderAttribut((baseString + "constant").c_str(), lightConstant);
	SetShaderAttribut((baseString + "linear").c_str(), light->linear);
	SetShaderAttribut((baseString + "quadratic").c_str(), light->quadratic);
	SetShaderAttribut((baseString + "cutOff").c_str(), glm::cos(glm::radians(light->GetSpotAngle() * light->GetSpotSmoothness())));
	SetShaderAttribut((baseString + "outerCutOff").c_str(), glm::cos(glm::radians(light->GetSpotAngle())));
}

/// <summary>
/// Send lights data to the shader
/// </summary>
void Shader::UpdateLights()
{
	int lightCount = AssetManager::GetLightCount();
	int directionalUsed = 0;
	int pointUsed = 0;
	int spotUsed = 0;

	//For each lights
	for (int lightI = 0; lightI < lightCount; lightI++)
	{
		std::shared_ptr<Light> light = AssetManager::GetLight(lightI).lock();
		if (light->type == Light::Directional)
		{
			SetDirectionalLightData(light, directionalUsed);
			directionalUsed++;
		}
		else if (light->type == Light::Point)
		{
			SetPointLightData(light, pointUsed);
			pointUsed++;
		}
		else if (light->type == Light::Spot)
		{
			SetSpotLightData(light, spotUsed);
			spotUsed++;
		}
	}
	//SetShaderAttribut("usedPointLightCount", 0);
	//SetShaderAttribut("usedSpotLightCount", 0);
	//SetShaderAttribut("usedDirectionalLightCount", 0);
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