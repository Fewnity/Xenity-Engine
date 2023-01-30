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
	unsigned int GetProgramId();
	void Use();
	void SetShaderAttribut(std::string attribut, Vector3 value);
	void SetShaderAttribut(std::string attribut, Vector4 value);
	void SetShaderCameraPosition();
	void SetShaderProjection();
	void SetShaderProjection2D();
	void SetShaderPosition(Vector3 position);
	void SetShaderRotation(Vector3 eulerAngle);
	void SetShaderScale(Vector3 scale);
	void SetShaderTexture(std::string attribut, Texture texture);
	void SetShaderAttribut(std::string attribut, float value);

private:
	void LoadShader(std::string vertexPath, std::string fragmentPath);
	unsigned int vertexShaderId;
	unsigned int fragmentShaderId;
	unsigned int programId;
};
