#pragma once

#include <string>
#include "../vectors/vector3.h"
#include "../vectors/vector4.h"
#include "texture.h"

class Shader
{
public:
	Shader() = delete;
	Shader(std::string vertexShaderPath, std::string fragmentShaderPath);
	~Shader();
	unsigned int GetProgramId();
	void Use();
	void SetShaderAttribut(std::string attribut, Vector3 value);
	void SetShaderAttribut(std::string attribut, Vector4 value);
	void SetShaderCameraPosition();
	void SetShaderProjection3D();
	void SetShaderProjection2D();
	void SetShaderPosition(Vector3 position);
	void SetShaderModel(Vector3 position, Vector3 eulerAngle, Vector3 scale);
	void SetShaderRotation(Vector3 eulerAngle);
	void SetShaderScale(Vector3 scale);
	void SetShaderTexture(std::string attribut, Texture* texture);
	void SetShaderAttribut(std::string attribut, float value);

private:
	void LoadShader(std::string vertexPath, std::string fragmentPath);
	unsigned int vertexShaderId = 0;
	unsigned int fragmentShaderId = 0;
	unsigned int programId = 0;
};
