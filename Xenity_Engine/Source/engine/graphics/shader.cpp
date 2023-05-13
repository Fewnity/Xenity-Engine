#include "shader.h"
#include "../../xenity.h"
#include "../graphics/renderer/renderer.h"

#include <iostream>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#pragma region Constructors / Destructor

/// <summary>
/// Instantiate shader from files
/// </summary>
/// <param name="vertexShaderPath">Vertex shader file path</param>
/// <param name="fragmentShaderPath">Fragment shader file path</param>
Shader::Shader(const std::string vertexShaderPath, const std::string fragmentShaderPath)
{
	LoadShader(vertexShaderPath, Vertex_Shader);
	LoadShader(fragmentShaderPath, Fragment_Shader);
	MakeShader();
}

Shader::Shader(const std::string vertexShaderPath, const std::string fragmentShaderPath, const std::string tessellationShaderPath, const std::string tessellationEvaluationShaderPath)
{
	useTessellation = true;
	LoadShader(vertexShaderPath, Vertex_Shader);
	LoadShader(tessellationShaderPath, Tessellation_Control_Shader);
	LoadShader(tessellationEvaluationShaderPath, Tessellation_Evaluation_Shader);
	LoadShader(fragmentShaderPath, Fragment_Shader);
	MakeShader();
}

/// <summary>
/// Shader destructor
/// </summary>
Shader::~Shader()
{
	AssetManager::RemoveShader(this);
	Engine::renderer->DeleteShader(vertexShaderId);
	Engine::renderer->DeleteShader(fragmentShaderId);

	if (useTessellation) 
	{
		Engine::renderer->DeleteShader(tessellationEvaluationShaderId);
		Engine::renderer->DeleteShader(fragmentShaderId);
	}
	Engine::renderer->DeleteShaderProgram(programId);
}

#pragma endregion

#pragma region Getters

unsigned int Shader::GetProgramId()
{
	return this->programId;
}

#pragma endregion

/// <summary>
/// Use shader
/// </summary>
bool Shader::Use()
{
	if (Graphics::usedShaderProgram != programId)
	{
		Engine::renderer->UseShaderProgram(programId);
		Graphics::usedShaderProgram = programId;
		return true;
	}
	return false;
}

#pragma region Data loading

void Shader::LoadShader(const std::string filePath, ShaderType type)
{
	std::string path = File::shaderPath + filePath;
	//Get file data
	std::string shaderData = File::ReadText(path);
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

			shaderError += ". File path: " + path;
			Debug::Print(shaderError);
		}
	}
	else 
	{
		std::string shaderError = "Compilation error: Shader type not found. File path: " + path;
		Debug::Print(shaderError);
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
	if (Graphics::usedCamera != nullptr)
	{
		Vector3 lookDirection = Graphics::usedCamera->gameObject->transform.GetForward();

		lookDirection = lookDirection + Graphics::usedCamera->gameObject->transform.GetPosition();

		float xAngle = Graphics::usedCamera->gameObject->transform.GetRotation().x;
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
			camera = glm::lookAt(glm::vec3(-Graphics::usedCamera->gameObject->transform.GetPosition().x, Graphics::usedCamera->gameObject->transform.GetPosition().y, Graphics::usedCamera->gameObject->transform.GetPosition().z), glm::vec3(-lookDirection.x, lookDirection.y, lookDirection.z), glm::vec3(0, -1, 0));
		else
			camera = glm::lookAt(glm::vec3(-Graphics::usedCamera->gameObject->transform.GetPosition().x, Graphics::usedCamera->gameObject->transform.GetPosition().y, Graphics::usedCamera->gameObject->transform.GetPosition().z), glm::vec3(-lookDirection.x, lookDirection.y, lookDirection.z), glm::vec3(0, 1, 0));
		
		Engine::renderer->SetShaderAttribut(programId, "camera", camera);
	}
}

/// <summary>
/// Send to the shader the 2D camera position
/// </summary>
void Shader::SetShaderCameraPosition2D() 
{
	Use();
	//Camera position
	if (Graphics::usedCamera != nullptr)
	{
		glm::mat4 camera = glm::mat4(1.0f);
		Camera* cam = Graphics::usedCamera;
		//camera = glm::translate(camera, glm::vec3(-cam->gameObject->transform.GetPosition().x / 10.f / cam->GetProjectionSize() * 5.0f,
			//-cam->gameObject->transform.GetPosition().y / 10.f / cam->GetProjectionSize() * 5.0f, 0));
		camera = glm::translate(camera, glm::vec3(-cam->gameObject->transform.GetPosition().x / 10.f,
			-cam->gameObject->transform.GetPosition().y / 10.f, 0));
		
		Engine::renderer->SetShaderAttribut(programId, "camera", camera);
	}
}

void Shader::SetShaderUnscaledProjection() 
{
	Use();
	Engine::renderer->SetShaderAttribut(programId, "projection", Graphics::usedCamera->GetUnscaledProjection());
}

/// <summary>
/// Send to the shader the 2D camera projection
/// </summary>
void Shader::SetShaderProjection()
{
	Use();
	Engine::renderer->SetShaderAttribut(programId, "projection", Graphics::usedCamera->GetProjection());
}

void Shader::SetShaderPosition(const Vector3 position)
{
	Use();
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(position.x, position.y, position.z));
	Engine::renderer->SetShaderAttribut(programId, "offset", trans);
}

/// <summary>
/// Send to the shader transform's model
/// </summary>
/// <param name="trans"></param>
void Shader::SetShaderModel(const glm::mat4* trans)
{
	Use();
	Engine::renderer->SetShaderAttribut(programId, "model", *trans);
}

/// <summary>
/// Send to the shader transform's model
/// </summary>
/// <param name="trans"></param>
void Shader::SetShaderModel(const Vector3 position, const Vector3 eulerAngle, const Vector3 scale) 
{
	Use();
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(position.x, position.y, position.z));

	/*glm::quat MyQuaternion;
	glm::vec3 EulerAngles((eulerAngle.x * M_PI) / 180.0, (eulerAngle.y * M_PI) / 180.0, (eulerAngle.z * M_PI) / 180.0);
	MyQuaternion = glm::quat(EulerAngles);
	glm::mat4 rotation = glm::toMat4(MyQuaternion);
	trans = trans * rotation;*/

	//DO NOT CHANGE THE ORDER Y-X-Z
	trans = glm::rotate(trans, glm::radians(eulerAngle.y), glm::vec3(0.0, 1.0, 0.0));
	trans = glm::rotate(trans, glm::radians(eulerAngle.x), glm::vec3(1.0, 0.0, 0.0));
	trans = glm::rotate(trans, glm::radians(eulerAngle.z), glm::vec3(0.0, 0.0, 1.0));
	trans = glm::scale(trans, glm::vec3(scale.x, scale.y, scale.z));
	Engine::renderer->SetShaderAttribut(programId, "model", trans);
}

void Shader::SetShaderRotation(const Vector3 eulerAngle) 
{
	Use();
	//Rotation
	glm::quat MyQuaternion;
	// Spécification directe des quatre composantes
	// Vous ne faites presque jamais cela directement
	// MyQuaternion = glm::quat(w, x, y, z);
	// Conversion à partir des angles d'Euler (en radians) vers un quaternion
	// radians = (degrees * pi) / 180 ;
	// degrees = radians * (180.0 / 3.141592653589793238463)
	//glm::vec3 EulerAngles(eulerAngle.x, eulerAngle.y, eulerAngle.z);
	glm::vec3 EulerAngles((eulerAngle.x * M_PI) / 180.0, (eulerAngle.y * M_PI) / 180.0, (eulerAngle.z * M_PI) / 180.0);
	MyQuaternion = glm::quat(EulerAngles);
	// Conversion à partir d'un axe/angle
	// Dans GLM l'angle doit être en degrés. Ici, donc, convertissez-le.
	// MyQuaternion = gtx::quaternion::angleAxis(glm::degrees(RotationAngle), RotationAxis);

	glm::mat4 rotation = glm::toMat4(MyQuaternion);
	Engine::renderer->SetShaderAttribut(programId, "rotation", rotation);
}

void Shader::SetShaderScale(const Vector3 scale) 
{
	Use();
	glm::mat3 scaleMat = glm::mat3(glm::vec3(scale.x, 0.0, 0.0),
		glm::vec3(0.0, scale.y, 0.0),
		glm::vec3(0.0, 0.0, scale.z));

	Engine::renderer->SetShaderAttribut(programId, "scale", scaleMat);
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

void Shader::SetShaderAttribut(const char* attribut, const float& value) 
{
	Use();
	Engine::renderer->SetShaderAttribut(programId, attribut, value);
}

void Shader::SetShaderAttribut(const char* attribut, const int& value)
{
	Use();
	Engine::renderer->SetShaderAttribut(programId, attribut, value);
}


void Shader::MakeShader()
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
	AssetManager::AddShader(this);
}

/// <summary>
/// Send to the shader the point light data
/// </summary>
/// <param name="light">Point light</param>
/// <param name="index">Shader's point light index</param>
void Shader::SetPointLightData(const Light* light, const int index)
{
	std::string baseString = "pointLights[" + std::to_string(index) + "].";
	SetShaderAttribut((baseString + "color").c_str(), light->color * light->intensity);
	SetShaderAttribut((baseString + "position").c_str(), light->gameObject->transform.GetPosition());
	SetShaderAttribut((baseString + "constant").c_str(), lightConstant);
	SetShaderAttribut((baseString + "linear").c_str(), light->linear);
	SetShaderAttribut((baseString + "quadratic").c_str(), light->quadratic);
}

/// <summary>
/// Send to the shader the directional light data
/// </summary>
/// <param name="light">Directional light</param>
/// <param name="index">Shader's directional light index</param>
void Shader::SetDirectionalLightData(const Light* light, const int index)
{
	std::string baseString = "directionalLights[" + std::to_string(index) + "].";
	SetShaderAttribut((baseString + "color").c_str(), light->color);
	SetShaderAttribut((baseString + "direction").c_str(), light->gameObject->transform.GetForward());
}

/// <summary>
/// Send to the shader the spot light data
/// </summary>
/// <param name="light">Spot light</param>
/// <param name="index">Shader's spot light index</param>
void Shader::SetSpotLightData(const Light* light, const int index)
{
	std::string baseString = "spotLights[" + std::to_string(index) + "].";
	SetShaderAttribut((baseString + "color").c_str(), light->intensity * light->color);
	SetShaderAttribut((baseString + "position").c_str(), light->gameObject->transform.GetPosition());
	SetShaderAttribut((baseString + "direction").c_str(), light->gameObject->transform.GetForward());
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
		Light* light = AssetManager::GetLight(lightI);
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

	SetShaderAttribut("usedPointLightCount", pointUsed);
	SetShaderAttribut("usedSpotLightCount", spotUsed);
	SetShaderAttribut("usedDirectionalLightCount", directionalUsed);
}

#pragma endregion