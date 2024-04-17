#pragma once
#include "texture.h"

#include <engine/vectors/vector2.h>
#include <engine/vectors/vector3.h>
#include <engine/vectors/vector4.h>

#include <engine/file_system/file_reference.h>
#include <engine/reflection/reflection.h>

#include <glm/glm.hpp>

#include <string>
#include <memory>


class Light;
class Material;

/**
 * @brief Internal
 */
class Shader : public FileReference, public Reflective
{
public:
	enum class ShaderType
	{
		Vertex_Shader,
		Fragment_Shader,
		Tessellation_Control_Shader,
		Tessellation_Evaluation_Shader,
	};

	static void Init();

	Shader();
	~Shader();

	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData() override;

	void LoadFileReference() override;

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

	/**
	* @brief Set the shader uniform for basic types
	*/
	void SetShaderAttribut(const char* attribut, const Vector4& value);
	void SetShaderAttribut(const char* attribut, const Vector3& value);
	void SetShaderAttribut(const char* attribut, const Vector2& value);
	void SetShaderAttribut(const char* attribut, float value);
	void SetShaderAttribut(const char* attribut, int value);

	/**
	* @brief Update lights in the shader
	*/
	void UpdateLights(bool disableLights);

	static std::shared_ptr<Shader> MakeShader();
private:

	/**
	* @brief Link the shader programs
	*/
	void Link();

	/**
	* @brief Compile the shader
	* @param filePath The file path of the shader
	* @param type The type of the shader
	*/
	void Compile(const std::string& filePath, ShaderType type);

	/**
	* @brief Set the shader uniform of a point light
	* @param light The light to set
	* @param index The index of the light
	*/
	void SetPointLightData(const std::shared_ptr<Light>& light, const int index);

	/**
	* @brief Set the shader uniform of a directional light
	* @param light The light to set
	* @param index The index of the light
	*/
	void SetDirectionalLightData(const std::shared_ptr<Light>& light, const int index);

	/**
	* @brief Set the shader uniform of a spot light
	* @param light The light to set
	* @param index The index of the light
	*/
	void SetSpotLightData(const std::shared_ptr<Light>& light, const int index);

	static glm::mat4 canvasCameraPosition;

	unsigned int vertexShaderId = 0;
	unsigned int fragmentShaderId = 0;
	unsigned int tessellationShaderId = 0;
	unsigned int tessellationEvaluationShaderId = 0;
	unsigned int programId = 0;
	unsigned int modelLocation = 0;
	unsigned int projectionLocation = 0;

public:
	bool useTessellation = false;
};
