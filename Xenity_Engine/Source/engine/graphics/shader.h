// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <string>
#include <memory>
#include <glm/glm.hpp>

#include <engine/file_system/file_reference.h>
#include <engine/reflection/reflection.h>

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
	int padding0;

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
class Shader : public FileReference, public Reflective
{
public:
	Shader();
	~Shader();

protected:
	friend class Renderer;
	friend class AssetManager;
	friend class ProjectManager;

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

	enum class ShaderType
	{
		Vertex_Shader,
		Fragment_Shader,
		Tessellation_Control_Shader,
		Tessellation_Evaluation_Shader,
	};


	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData(AssetPlatform platform) override;

	void LoadFileReference() override;

	static std::shared_ptr<Shader> MakeShader();

	friend class Material;
	friend class Graphics;
	friend class RendererOpengl;
	friend class RendererGU;
	friend class RendererGsKit;
	friend class RendererVU1;
	friend class MeshRenderer;

	static void Init();

	/**
	* @brief Get the shader program id
	*/
	unsigned int GetProgramId() const;

	/**
	* @brief Use the shader program
	*/
	bool Use();

	/**
	* @brief Set the shader uniform of the camera position
	*/
	void SetShaderCameraPosition();

	/**
	* @brief Set the shader uniform of the camera position for the canvas
	*/
	void SetShaderCameraPositionCanvas();

	/**
	* @brief Set the shader uniform of the camera projection
	*/
	void SetShaderProjection();

	/**
	* @brief Set the shader uniform of the camera projection for the canvas
	*/
	void SetShaderProjectionCanvas();

	/**
	* @brief Set the shader uniform of the object model
	* @param trans The transformation matrix
	*/
	void SetShaderModel(const glm::mat4& trans);

	/**
	* @brief Set the shader uniform of the object model
	* @param position The position of the object
	* @param eulerAngle The euler angle of the object
	* @param scale The scale of the object
	*/
	void SetShaderModel(const Vector3& position, const Vector3& eulerAngle, const Vector3& scale);

	void SetLightIndices(const LightsIndices& lightsIndices);

	/**
	* @brief Set the shader uniform for basic types
	*/
	void SetShaderAttribut(const std::string& attribut, const Vector4& value);
	void SetShaderAttribut(const std::string& attribut, const Vector3& value);
	void SetShaderAttribut(const std::string& attribut, const Vector2& value);
	void SetShaderAttribut(const std::string& attribut, float value);
	void SetShaderAttribut(const std::string& attribut, int value);

	/**
	* @brief Update lights in the shader
	*/
	void UpdateLights(bool disableLights);

	/**
	* @brief Link the shader programs
	*/
	void Link();

	/**
	* @brief Compile the shader
	* @param filePath The file path of the shader
	* @param type The type of the shader
	*/
	bool Compile(const std::string& filePath, ShaderType type);

	/**
	* @brief Set the shader uniform of a point light
	* @param light The light to set
	* @param index The index of the light
	*/
	void SetPointLightData(const Light& light, const int index);

	/**
	* @brief Set the shader uniform of a directional light
	* @param light The light to set
	* @param index The index of the light
	*/
	void SetDirectionalLightData(const Light& light, const int index);
	void SetAmbientLightData(const Vector3& color);

	/**
	* @brief Set the shader uniform of a spot light
	* @param light The light to set
	* @param index The index of the light
	*/
	void SetSpotLightData(const Light& light, const int index);

	static glm::mat4 m_canvasCameraTransformationMatrix;

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

	bool m_useTessellation = false;
	std::unordered_map<std::string, unsigned int> m_uniformsIds;
	std::vector<Light*> currentLights;
	std::vector<Light*> currentDirectionalLights;
};
