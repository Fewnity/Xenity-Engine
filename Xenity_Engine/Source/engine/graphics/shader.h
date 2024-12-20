// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <string>
#include <memory>

#include <glm/fwd.hpp>

#include <engine/file_system/file_reference.h>

#include <engine/constants.h>

class Light;
class Material;
class Vector2;
class Vector3;
class Vector4;

struct ivec4
{
	int x;
	int y;
	int z;
	int w;
};

struct alignas(16) LightsIndices 
{
	int usedPointLightCount = 0;
	int usedSpotLightCount = 0;
	int usedDirectionalLightCount = 0;
	int padding0 = 0;

	ivec4 pointLightIndices[MAX_LIGHT_COUNT];
	ivec4 spotLightIndices[MAX_LIGHT_COUNT];
	ivec4 directionalLightIndices[MAX_LIGHT_COUNT];

	// Padding
	/*int padding1;
	int padding2;*/
};

/**
* @brief [Internal] Shader file class
*/
class Shader : public FileReference
{
public:
	enum class ShaderType
	{
		Vertex_Shader,
		Fragment_Shader,
		Tessellation_Control_Shader,
		Tessellation_Evaluation_Shader,
	};

	Shader();
	~Shader();

	std::string GetShaderCode(ShaderType type, Platform platform) const;

	std::shared_ptr<Shader> GetShared()
	{
		return std::dynamic_pointer_cast<Shader>(shared_from_this());
	}

protected:
	virtual void CreateShader(Shader::ShaderType type) = 0;

	std::string GetShaderCode(const std::string& fullShaderCode, ShaderType type, Platform platform) const;
	std::string ReadShader() const;

	/**
	* Data needs to be deleted by the caller
	*/
	unsigned char* ReadShaderBinary(size_t& size) const;

	friend class Renderer;
	friend class AssetManager;
	friend class ProjectManager;

	//class PointLightVariableNames 
	//{
	//public:
	//	PointLightVariableNames() = delete;
	//	explicit PointLightVariableNames(int index);
	//	~PointLightVariableNames();

	//	char* indices = nullptr;
	//	char* color = nullptr;
	//	char* position = nullptr;
	//	char* constant = nullptr;
	//	char* linear = nullptr;
	//	char* quadratic = nullptr;
	//};

	//class DirectionalLightsVariableNames
	//{
	//public:
	//	DirectionalLightsVariableNames() = delete;
	//	explicit DirectionalLightsVariableNames(int index);
	//	~DirectionalLightsVariableNames();

	//	char* indices = nullptr;
	//	char* color = nullptr;
	//	char* direction = nullptr;
	//};


	//class SpotLightVariableNames
	//{
	//public:
	//	SpotLightVariableNames() = delete;
	//	explicit SpotLightVariableNames(int index);
	//	~SpotLightVariableNames();

	//	char* indices = nullptr;
	//	char* color = nullptr;
	//	char* position = nullptr;
	//	char* direction = nullptr;
	//	char* constant = nullptr;
	//	char* linear = nullptr;
	//	char* quadratic = nullptr;
	//	char* cutOff = nullptr;
	//	char* outerCutOff = nullptr;
	//};

	//class PointLightVariableIds
	//{
	//public:
	//	PointLightVariableIds() = delete;
	//	explicit PointLightVariableIds(int index, unsigned int programId);

	//	unsigned int indices = 0;
	//	unsigned int color = 0;
	//	unsigned int position = 0;
	//	unsigned int constant = 0;
	//	unsigned int linear = 0;
	//	unsigned int quadratic = 0;
	//};

	//class DirectionalLightsVariableIds
	//{
	//public:
	//	DirectionalLightsVariableIds() = delete;
	//	explicit DirectionalLightsVariableIds(int index, unsigned int programId);

	//	unsigned int indices = 0;
	//	unsigned int color = 0;
	//	unsigned int direction = 0;
	//};


	//class SpotLightVariableIds
	//{
	//public:
	//	SpotLightVariableIds() = delete;
	//	explicit SpotLightVariableIds(int index, unsigned int programId);

	//	unsigned int indices = 0;
	//	unsigned int color = 0;
	//	unsigned int position = 0;
	//	unsigned int direction = 0;
	//	unsigned int constant = 0;
	//	unsigned int linear = 0;
	//	unsigned int quadratic = 0;
	//	unsigned int cutOff = 0;
	//	unsigned int outerCutOff = 0;
	//};

	//static std::vector<PointLightVariableNames> s_pointlightVariableNames;
	//static std::vector<DirectionalLightsVariableNames> s_directionallightVariableNames;
	//static std::vector<SpotLightVariableNames> s_spotlightVariableNames;

	//std::vector<PointLightVariableIds> m_pointlightVariableIds;
	//std::vector<DirectionalLightsVariableIds> m_directionallightVariableIds;
	//std::vector<SpotLightVariableIds> m_spotlightVariableIds;

	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData(AssetPlatform platform) override;

	void LoadFileReference() override;
	virtual void Load() = 0;

	static std::shared_ptr<Shader> MakeShader();

	friend class Material;
	friend class Graphics;
	friend class RendererOpengl;
	friend class RendererRSX;
	friend class RendererGU;
	friend class RendererGsKit;
	friend class RendererVU1;
	friend class MeshRenderer;

	static void Init();

	/**
	* @brief Use the shader program
	*/
	virtual bool Use() = 0;

	/**
	* @brief Set the shader uniform of the camera position
	*/
	virtual void SetShaderCameraPosition() = 0;

	/**
	* @brief Set the shader uniform of the camera position for the canvas
	*/
	virtual void SetShaderCameraPositionCanvas() = 0;

	/**
	* @brief Set the shader uniform of the camera projection
	*/
	virtual void SetShaderProjection() = 0;

	/**
	* @brief Set the shader uniform of the camera projection for the canvas
	*/
	virtual void SetShaderProjectionCanvas() = 0;

	/**
	* @brief Set the shader uniform of the object model
	* @param trans The transformation matrix
	*/
	virtual void SetShaderModel(const glm::mat4& trans) = 0;

	/**
	* @brief Set the shader uniform of the object model
	* @param position The position of the object
	* @param eulerAngle The euler angle of the object
	* @param scale The scale of the object
	*/
	virtual void SetShaderModel(const Vector3& position, const Vector3& eulerAngle, const Vector3& scale) = 0;

	virtual void SetLightIndices(const LightsIndices& lightsIndices) = 0;

	/**
	* @brief Set the shader uniform for basic types
	*/
	virtual void SetShaderAttribut(const std::string& attribut, const Vector4& value) = 0;
	virtual void SetShaderAttribut(const std::string& attribut, const Vector3& value) = 0;
	virtual void SetShaderAttribut(const std::string& attribut, const Vector2& value) = 0;
	virtual void SetShaderAttribut(const std::string& attribut, float value) = 0;
	virtual void SetShaderAttribut(const std::string& attribut, int value) = 0;

	/**
	* @brief Update lights in the shader
	*/
	virtual void UpdateLights() = 0;

	/**
	* @brief Link the shader programs
	*/
	virtual void Link() = 0;

	/**
	* @brief Compile the shader
	* @param filePath The file path of the shader
	* @param type The type of the shader
	*/
	virtual bool Compile(const std::string& filePath, ShaderType type) = 0;

	/**
	* @brief Set the shader uniform of a point light
	* @param light The light to set
	* @param index The index of the light
	*/
	virtual void SetPointLightData(const Light& light, const int index) = 0;

	/**
	* @brief Set the shader uniform of a directional light
	* @param light The light to set
	* @param index The index of the light
	*/
	virtual void SetDirectionalLightData(const Light& light, const int index) = 0;
	virtual void SetAmbientLightData(const Vector3& color) = 0;

	/**
	* @brief Set the shader uniform of a spot light
	* @param light The light to set
	* @param index The index of the light
	*/
	virtual void SetSpotLightData(const Light& light, const int index) = 0;

	static glm::mat4 m_canvasCameraTransformationMatrix;

	//unsigned int m_vertexShaderId = 0;
	//unsigned int m_fragmentShaderId = 0;
	//unsigned int m_tessellationShaderId = 0;
	//unsigned int m_tessellationEvaluationShaderId = 0;
	//unsigned int m_programId = 0;
	//unsigned int m_modelLocation = 0;
	//unsigned int m_projectionLocation = 0;
	//unsigned int m_cameraLocation = 0;
	//unsigned int m_ambientLightLocation = 0;

	//unsigned int m_usedPointLightCountLocation = 0;
	//unsigned int m_usedSpotLightCountLocation = 0;
	//unsigned int m_usedDirectionalLightCountLocation = 0;

	bool m_useTessellation = false;
	
	std::vector<Light*> m_currentLights;
	std::vector<Light*> m_currentDirectionalLights;
	static constexpr uint32_t INVALID_SHADER_UNIFORM = -1;
	static std::shared_ptr<Light> defaultDarkLight;
};
