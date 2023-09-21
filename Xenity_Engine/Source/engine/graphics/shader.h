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
	enum ShaderType 
	{
		Vertex_Shader,
		Fragment_Shader,
		Tessellation_Control_Shader,
		Tessellation_Evaluation_Shader,
	};
	Shader() = delete;
	Shader(const std::string vertexShaderPath, const std::string fragmentShaderPath);
	Shader(const std::string vertexShaderPath, const std::string fragmentShaderPath, const std::string tessellationShaderPath, const std::string tessellationEvaluationShaderPath);
	~Shader();

	unsigned int GetProgramId();
	bool Use();
	void SetShaderCameraPosition();
	void SetShaderCameraPositionCanvas();
	void SetShaderProjection();
	void SetShaderProjectionCanvas();
	void SetShaderModel(const glm::mat4 *trans);
	void SetShaderModel(const Vector3 position, const Vector3 eulerAngle, const Vector3 scale);
	void SetShaderAttribut(const char* attribut, const Vector4& value);
	void SetShaderAttribut(const char* attribut, const Vector3& value);
	void SetShaderAttribut(const char* attribut, const Vector2& value);
	void SetShaderAttribut(const char* attribut, const float& value);
	void SetShaderAttribut(const char* attribut, const int& value);


	void UpdateLights();
	bool useTessellation = false;

private:
	void MakeShader();
	void LoadShader(const std::string filePath, ShaderType type);
	void SetPointLightData(std::shared_ptr<Light> light, const int index);
	void SetDirectionalLightData(std::shared_ptr<Light> light, const int index);
	void SetSpotLightData(std::shared_ptr<Light> light, const int index);

	unsigned int vertexShaderId = 0;
	unsigned int fragmentShaderId = 0;
	unsigned int tessellationShaderId = 0;
	unsigned int tessellationEvaluationShaderId = 0;
	unsigned int programId = 0;
	unsigned int modelLocation = 0;
	unsigned int projectionLocation = 0;
};
