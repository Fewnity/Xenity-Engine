#pragma once

#include "renderer.h"
#include "../../lighting/lighting.h"

class RendererOpengl : public Renderer
{
public:
	RendererOpengl();
	int Init();
	void Setup();
	void Stop();
	void NewFrame();
	void EndFrame();
	void SetViewport(int x, int y, int width, int height);
	void SetClearColor(Color color);
	void SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane);
	void SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane);
	void ResetView();
	void SetCameraPosition(std::weak_ptr<Camera> camera);
	void ResetTransform();
	void SetTransform(Vector3 position, Vector3 rotation, Vector3 scale, bool resetTransform);
	void MoveTransform(Vector3 position);
	void BindTexture(Texture *texture);
	void ApplyTextureFilters(Texture *texture);
	void DrawMeshData(MeshData *meshData, RenderingSettings settings);
	unsigned int CreateNewTexture();
	void SetTextureData(Texture *texture, unsigned int textureType, const unsigned char *buffer);
	void Clear();

private:
	// int GetBufferTypeEnum(BufferType bufferType);
	// int GetBufferModeEnum(BufferMode bufferMode);
	int GetWrapModeEnum(Texture::WrapMode wrapMode);
	int maxLightCount = 8;
	void DisableAllLight();
	void Setlights(std::weak_ptr<Camera> camera);
	void SetLight(int lightIndex, Vector3 lightPosition, float intensity, Color color, Light::LightType type, float attenuation);
	// int GetCullFaceEnum(CullFace face);
	// float GetAnisotropicValueEnum(Texture::AnisotropicLevel level);
	// int GetShaderTypeEnum(Shader::ShaderType shaderType);
	// int GetDrawModeEnum(DrawMode drawMode);
};
