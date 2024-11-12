// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#if defined(__PS3__)

#include <engine/graphics/shader.h>
#include <rsx/rsx.h>

/**
* @brief [Internal] Shader file class
*/
class ShaderRSX : public Shader
{
public:
	~ShaderRSX();
	static void Init();

	uint32_t m_vertexProgramSize = 0;
	uint32_t m_fragmentProgramSize = 0;
	uint32_t* m_fragmentProgramCodeOnGPU = nullptr;;
	uint32_t m_fp_offset = 0;

	void* m_vertexProgramCode = nullptr;
	void* m_fragmentProgramCode = nullptr;

	rsxVertexProgram* m_vertexProgram = nullptr;
	rsxFragmentProgram* m_fragmentProgram = nullptr;

	rsxProgramConst* m_projMatrix = nullptr;
	rsxProgramConst* m_mvMatrix = nullptr;


	rsxProgramConst* m_globalAmbient = nullptr;
	rsxProgramAttrib* m_textureUnit = nullptr;

protected:
	void Load() override;
	void CreateShader(Shader::ShaderType type) override;

	/**
	* @brief Use the shader program
	*/
	bool Use() override;

	/**
	* @brief Set the shader uniform of the camera position
	*/
	void SetShaderCameraPosition() override;

	/**
	* @brief Set the shader uniform of the camera position for the canvas
	*/
	void SetShaderCameraPositionCanvas() override;

	/**
	* @brief Set the shader uniform of the camera projection
	*/
	void SetShaderProjection() override;

	/**
	* @brief Set the shader uniform of the camera projection for the canvas
	*/
	void SetShaderProjectionCanvas() override;

	/**
	* @brief Set the shader uniform of the object model
	* @param trans The transformation matrix
	*/
	void SetShaderModel(const glm::mat4& trans) override;

	/**
	* @brief Set the shader uniform of the object model
	* @param position The position of the object
	* @param eulerAngle The euler angle of the object
	* @param scale The scale of the object
	*/
	void SetShaderModel(const Vector3& position, const Vector3& eulerAngle, const Vector3& scale) override;

	void SetLightIndices(const LightsIndices& lightsIndices) override;

	/**
	* @brief Set the shader uniform for basic types
	*/
	void SetShaderAttribut(const std::string& attribut, const Vector4& value) override;
	void SetShaderAttribut(const std::string& attribut, const Vector3& value) override;
	void SetShaderAttribut(const std::string& attribut, const Vector2& value) override;
	void SetShaderAttribut(const std::string& attribut, float value) override;
	void SetShaderAttribut(const std::string& attribut, int value) override;

	/**
	* @brief Update lights in the shader
	*/
	void UpdateLights(bool disableLights) override;

	/**
	* @brief Link the shader programs
	*/
	void Link() override;

	/**
	* @brief Compile the shader
	* @param filePath The file path of the shader
	* @param type The type of the shader
	*/
	bool Compile(const std::string& filePath, ShaderType type) override;

	/**
	* @brief Set the shader uniform of a point light
	* @param light The light to set
	* @param index The index of the light
	*/
	void SetPointLightData(const Light& light, const int index) override;

	/**
	* @brief Set the shader uniform of a directional light
	* @param light The light to set
	* @param index The index of the light
	*/
	void SetDirectionalLightData(const Light& light, const int index) override;
	void SetAmbientLightData(const Vector3& color) override;

	/**
	* @brief Set the shader uniform of a spot light
	* @param light The light to set
	* @param index The index of the light
	*/
	void SetSpotLightData(const Light& light, const int index) override;

};

#endif