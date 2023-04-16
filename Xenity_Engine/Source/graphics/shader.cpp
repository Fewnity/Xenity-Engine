#include "shader.h"
#include "graphics.h"
#include "../ui/window.h"
#include <iostream>
#include "../file_system/file.h"
#include "../asset_manager.h"
#include "../lighting/lighting.h"
#include "../debug/debug.h"

#pragma region Constructors / Destructor

/// <summary>
/// Instantiate shader from files
/// </summary>
/// <param name="vertexShaderPath">Vertex shader file path</param>
/// <param name="fragmentShaderPath">Fragment shader file path</param>
Shader::Shader(const std::string vertexShaderPath, const std::string fragmentShaderPath)
{
	LoadShader(vertexShaderPath, fragmentShaderPath);
	AssetManager::AddShader(this);
}

/// <summary>
/// Shader destructor
/// </summary>
Shader::~Shader()
{
	AssetManager::RemoveShader(this);
	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);
	glDeleteProgram(programId);
}

#pragma endregion

#pragma region Getters

GLuint Shader::GetProgramId()
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
		glUseProgram(programId);
		Graphics::usedShaderProgram = programId;
		return true;
	}
	return false;
}

#pragma region Data loading

/// <summary>
/// Load shader files
/// </summary>
/// <param name="vertexFileName"></param>
/// <param name="fragmentFileName"></param>
void Shader::LoadShader(const std::string vertexFileName, const std::string fragmentFileName) {

	Debug::Print("Compiling shader...");

	std::string vertexFilePath = File::shaderPath + vertexFileName;
	std::string fragmentFilePath = File::shaderPath + fragmentFileName;

	//Load vertex shader text
	std::string vertex_shader_text_temp = File::ReadText(vertexFilePath);
	const char* vertex_shader_text = vertex_shader_text_temp.c_str();
	//Create vertex shader
	vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &vertex_shader_text, NULL); // Load shader text
	glCompileShader(vertexShaderId);

	GLint vResult;
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &vResult);

	if (vResult == 0)
		Debug::Print("Vertex shader error: " + vertexFilePath);

	//Load fragment shader text
	std::string fragment_shader_text_temp = File::ReadText(fragmentFilePath);
	const char* fragment_shader_text = fragment_shader_text_temp.c_str();
	//Create fragment shader
	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, &fragment_shader_text, NULL);// Load shader text
	glCompileShader(fragmentShaderId);

	GLint fragResult;
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &fragResult);

	if (fragResult == 0)
		Debug::Print("Fragment shader error. Path: " + fragmentFilePath);

	//Link shaders to a program
	programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);
	glUseProgram(programId);
	modelLocation = glGetUniformLocation(programId, "model");

	projectionLocation = glGetUniformLocation(programId, "projection");
}

#pragma endregion

#pragma region Uniform setters

/// <summary>
/// Send to the shader the 3D camera position
/// </summary>
void Shader::SetShaderCameraPosition() {
	Use();
	//Camera position
	if (Graphics::usedCamera != nullptr && Graphics::usedCamera->gameObject != nullptr)
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

		glUniformMatrix4fv(glGetUniformLocation(programId, "camera"), 1, false, glm::value_ptr(camera));
	}
}

/// <summary>
/// Send to the shader the 2D camera position
/// </summary>
void Shader::SetShaderCameraPosition2D() {
	Use();
	//Camera position
	if (Graphics::usedCamera != nullptr && Graphics::usedCamera->gameObject != nullptr)
	{
		glm::mat4 camera = glm::mat4(1.0f);
		Camera* cam = Graphics::usedCamera;
		//camera = glm::translate(camera, glm::vec3(-cam->gameObject->transform.GetPosition().x / 10.f / cam->GetProjectionSize() * 5.0f,
			//-cam->gameObject->transform.GetPosition().y / 10.f / cam->GetProjectionSize() * 5.0f, 0));
		camera = glm::translate(camera, glm::vec3(-cam->gameObject->transform.GetPosition().x / 10.f,
			-cam->gameObject->transform.GetPosition().y / 10.f, 0));
		glUniformMatrix4fv(glGetUniformLocation(programId, "camera"), 1, false, glm::value_ptr(camera));
	}
}

void Shader::SetShaderUnscaledProjection() {
	Use();
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(Graphics::usedCamera->GetUnscaledProjection()));
}

/// <summary>
/// Send to the shader the 2D camera projection
/// </summary>
void Shader::SetShaderProjection() 
{
	Use();
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(Graphics::usedCamera->GetProjection()));
}

void Shader::SetShaderPosition(const Vector3 position) 
{
	Use();
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(position.x, position.y, position.z));
	glUniformMatrix4fv(glGetUniformLocation(programId, "offset"), 1, false, glm::value_ptr(trans));
}

/// <summary>
/// Send to the shader transform's model
/// </summary>
/// <param name="trans"></param>
void Shader::SetShaderModel(const glm::mat4* trans)
{
	Use();
	glUniformMatrix4fv(modelLocation, 1, false, glm::value_ptr(*trans));
}

/// <summary>
/// Send to the shader transform's model
/// </summary>
/// <param name="trans"></param>
void Shader::SetShaderModel(const Vector3 position, const Vector3 eulerAngle, const Vector3 scale) {
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

	glUniformMatrix4fv(glGetUniformLocation(programId, "model"), 1, false, glm::value_ptr(trans));
}

void Shader::SetShaderRotation(const Vector3 eulerAngle) {
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

	GLuint location = glGetUniformLocation(programId, "rotation");
	glm::mat4 rotation = glm::toMat4(MyQuaternion);
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(rotation));
}

void Shader::SetShaderScale(const Vector3 scale) {
	Use();
	glm::mat3 scaleMat = glm::mat3(glm::vec3(scale.x, 0.0, 0.0),
		glm::vec3(0.0, scale.y, 0.0),
		glm::vec3(0.0, 0.0, scale.z));

	glUniformMatrix3fv(glGetUniformLocation(programId, "scale"), 1, false, glm::value_ptr(scaleMat));
}


void Shader::SetShaderAttribut(const char* attribut, Vector4& value) {
	Use();
	glUniform4f(glGetUniformLocation(programId, attribut), value.x, value.y, value.z, value.w);
}

void Shader::SetShaderAttribut(const char* attribut, const Vector3 value) {
	Use();
	glUniform3f(glGetUniformLocation(programId, attribut), value.x, value.y, value.z);
}

void Shader::SetShaderAttribut(const char* attribut, const Vector2 value) {
	Use();
	glUniform2f(glGetUniformLocation(programId, attribut), value.x, value.y);
}

void Shader::SetShaderAttribut(const char* attribut, const float value) {
	Use();
	glUniform1f(glGetUniformLocation(programId, attribut), value);
}

void Shader::SetShaderAttribut(const char* attribut, const int value) {
	Use();
	glUniform1i(glGetUniformLocation(programId, attribut), value);
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