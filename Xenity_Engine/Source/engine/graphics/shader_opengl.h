// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__) || defined(__vita__)

#include <engine/graphics/shader.h>

/**
* @brief [Internal] Shader file class
*/
class ShaderOpenGL : public Shader
{
public:
	~ShaderOpenGL();
	static void Init();

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

	class PointLightVariableNames
	{
	public:
		PointLightVariableNames() = delete;
		explicit PointLightVariableNames(int index);
		~PointLightVariableNames();

		char* indices = nullptr;
		char* color = nullptr;
		char* position = nullptr;
		char* constant = nullptr;
		char* linear = nullptr;
		char* quadratic = nullptr;
	};

	class DirectionalLightsVariableNames
	{
	public:
		DirectionalLightsVariableNames() = delete;
		explicit DirectionalLightsVariableNames(int index);
		~DirectionalLightsVariableNames();

		char* indices = nullptr;
		char* color = nullptr;
		char* direction = nullptr;
	};


	class SpotLightVariableNames
	{
	public:
		SpotLightVariableNames() = delete;
		explicit SpotLightVariableNames(int index);
		~SpotLightVariableNames();

		char* indices = nullptr;
		char* color = nullptr;
		char* position = nullptr;
		char* direction = nullptr;
		char* constant = nullptr;
		char* linear = nullptr;
		char* quadratic = nullptr;
		char* cutOff = nullptr;
		char* outerCutOff = nullptr;
	};

	class PointLightVariableIds
	{
	public:
		PointLightVariableIds() = delete;
		explicit PointLightVariableIds(int index, unsigned int programId);

		unsigned int indices = 0;
		unsigned int color = 0;
		unsigned int position = 0;
		unsigned int constant = 0;
		unsigned int linear = 0;
		unsigned int quadratic = 0;
	};

	class DirectionalLightsVariableIds
	{
	public:
		DirectionalLightsVariableIds() = delete;
		explicit DirectionalLightsVariableIds(int index, unsigned int programId);

		unsigned int indices = 0;
		unsigned int color = 0;
		unsigned int direction = 0;
	};


	class SpotLightVariableIds
	{
	public:
		SpotLightVariableIds() = delete;
		explicit SpotLightVariableIds(int index, unsigned int programId);

		unsigned int indices = 0;
		unsigned int color = 0;
		unsigned int position = 0;
		unsigned int direction = 0;
		unsigned int constant = 0;
		unsigned int linear = 0;
		unsigned int quadratic = 0;
		unsigned int cutOff = 0;
		unsigned int outerCutOff = 0;
	};

	static std::vector<PointLightVariableNames> s_pointlightVariableNames;
	static std::vector<DirectionalLightsVariableNames> s_directionallightVariableNames;
	static std::vector<SpotLightVariableNames> s_spotlightVariableNames;

	std::vector<PointLightVariableIds> m_pointlightVariableIds;
	std::vector<DirectionalLightsVariableIds> m_directionallightVariableIds;
	std::vector<SpotLightVariableIds> m_spotlightVariableIds;

	unsigned int m_vertexShaderId = 0;
	unsigned int m_fragmentShaderId = 0;
	unsigned int m_tessellationShaderId = 0;
	unsigned int m_tessellationEvaluationShaderId = 0;
	unsigned int m_programId = 0;
	unsigned int m_modelLocation = 0;
	unsigned int m_projectionLocation = 0;
	unsigned int m_cameraLocation = 0;
	unsigned int m_ambientLightLocation = 0;

	unsigned int m_usedPointLightCountLocation = 0;
	unsigned int m_usedSpotLightCountLocation = 0;
	unsigned int m_usedDirectionalLightCountLocation = 0;
};

#endif