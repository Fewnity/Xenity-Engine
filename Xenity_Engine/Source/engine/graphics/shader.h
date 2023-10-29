#pragma once

#include <string>
#include "../vectors/vector2.h"
#include "../vectors/vector3.h"
#include "../vectors/vector4.h"
#include "texture.h"
#include <glm/glm.hpp>
#include "../file_system/file_reference.h"
#include "../reflection/reflection.h"
#include <memory>

class Light;

class Shader : public FileReference, public Reflection
{
public:
	enum ShaderType
	{
		Vertex_Shader,
		Fragment_Shader,
		Tessellation_Control_Shader,
		Tessellation_Evaluation_Shader,
	};

	static void Init();

	Shader();
	~Shader();

	std::unordered_map<std::string, ReflectionEntry> GetReflection() override;
	std::unordered_map<std::string, ReflectionEntry> GetMetaReflection() override;

	void LoadFileReference() override;

	unsigned int GetProgramId() const;
	bool Use();
	void SetShaderCameraPosition();
	void SetShaderCameraPositionCanvas();
	void SetShaderProjection();
	void SetShaderProjectionCanvas();
	void SetShaderModel(const glm::mat4& trans);
	void SetShaderModel(const Vector3& position, const Vector3& eulerAngle, const Vector3& scale);
	void SetShaderAttribut(const char* attribut, const Vector4& value);
	void SetShaderAttribut(const char* attribut, const Vector3& value);
	void SetShaderAttribut(const char* attribut, const Vector2& value);
	void SetShaderAttribut(const char* attribut, float value);
	void SetShaderAttribut(const char* attribut, int value);

	void UpdateLights();
	bool useTessellation = false;

	static std::shared_ptr<Shader> MakeShader();

private:
	void Link();
	void Compile(const std::string& filePath, ShaderType type);
	void SetPointLightData(std::shared_ptr<Light> light, const int index);
	void SetDirectionalLightData(std::shared_ptr<Light> light, const int index);
	void SetSpotLightData(std::shared_ptr<Light> light, const int index);

	static glm::mat4 canvasCameraPosition;

	unsigned int vertexShaderId = 0;
	unsigned int fragmentShaderId = 0;
	unsigned int tessellationShaderId = 0;
	unsigned int tessellationEvaluationShaderId = 0;
	unsigned int programId = 0;
	unsigned int modelLocation = 0;
	unsigned int projectionLocation = 0;
};
