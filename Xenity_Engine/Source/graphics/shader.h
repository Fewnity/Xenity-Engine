#pragma once

#include <string>
#include "../vectors/vector2.h"
#include "../vectors/vector3.h"
#include "../vectors/vector4.h"
#include "texture.h"
#include <glm/glm.hpp>

class Light;

class Shader
{
public:
	Shader() = delete;
	Shader(const std::string vertexShaderPath, const std::string fragmentShaderPath);
	Shader(const std::string vertexShaderPath, const std::string fragmentShaderPath, const std::string tessellationShaderPath, const std::string tessellationEvaluationShaderPath);
	~Shader();

	unsigned int GetProgramId();
	bool Use();
	void SetShaderCameraPosition();
	void SetShaderCameraPosition2D();
	void SetShaderUnscaledProjection();
	void SetShaderProjection();
	void SetShaderPosition(const Vector3 position);
	void SetShaderModel(const glm::mat4 *trans);
	void SetShaderModel(const Vector3 position, const Vector3 eulerAngle, const Vector3 scale);
	void SetShaderRotation(const Vector3 eulerAngle);
	void SetShaderScale(const Vector3 scale);
	void SetShaderAttribut(const char* attribut, const Vector2 value);
	void SetShaderAttribut(const char* attribut, const Vector3 value);
	void SetShaderAttribut(const char* attribut, Vector4 &value);
	void SetShaderAttribut(const char* attribut, const float value);
	void SetShaderAttribut(const char* attribut, const int value);


	void UpdateLights();

private:
	void LoadShader(const std::string vertexPath, const std::string fragmentPath);
	void SetPointLightData(const Light* light, const int index);
	void SetDirectionalLightData(const Light* light, const int index);
	void SetSpotLightData(const Light* light, const int index);

	unsigned int vertexShaderId = 0;
	unsigned int fragmentShaderId = 0;
	unsigned int tessellationShaderId = 0;
	unsigned int tessellationEvaluationShaderId = 0;
	unsigned int programId = 0;
	unsigned int modelLocation = 0;
	unsigned int projectionLocation = 0;
};
