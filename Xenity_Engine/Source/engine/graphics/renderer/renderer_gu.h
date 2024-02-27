#pragma once

/**
 * [Internal]
 */

#if defined(__PSP__)
#include <engine/api.h>

#include "renderer.h"
#include <engine/lighting/lighting.h>

class API RendererGU : public Renderer
{
public:
	RendererGU();
	int Init() override;
	void Setup() override;
	void Stop() override;
	void NewFrame() override;
	void EndFrame() override;
	void SetViewport(int x, int y, int width, int height) override;
	void SetClearColor(const Color& color) override;
	void SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane) override;
	void SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane, float aspect) override;
	void ResetView() override;
	void SetCameraPosition(const std::shared_ptr<Camera>& camera) override;
	void ResetTransform() override;
	void SetTransform(const Vector3& position, const Vector3& rotation, const Vector3& scale, bool resetTransform) override;
	void SetTransform(const glm::mat4& mat) override;
	void BindTexture(const std::shared_ptr <Texture>& texture) override;
	void DrawMeshData(const std::shared_ptr <MeshData>& meshData, const std::vector<std::shared_ptr<Texture>>& textures, RenderingSettings& settings) override;
	void DrawLine(const Vector3& a, const Vector3& bn, const Color& color, RenderingSettings& settings) override;
	unsigned int CreateNewTexture() override;
	void DeleteTexture(Texture* texture) override;
	void SetTextureData(const std::shared_ptr <Texture>& texture, unsigned int textureType, const unsigned char* buffer) override;
	void Clear() override;
	void SetFog(bool active) override;
	void SetFogValues(float start, float end, const Color& color) override;

	void DeleteSubMeshData(MeshData::SubMesh* subMesh) override;
	void UploadMeshData(const std::shared_ptr<MeshData>& meshData) override;

	void Setlights(const std::shared_ptr<Camera>& camera) override;

private:
	void ApplyTextureFilters(const std::shared_ptr<Texture>& texture);

	int maxLightCount = 8;
	void DisableAllLight();
	void SetLight(int lightIndex, const Vector3& lightPosition, float intensity, Color color, LightType type, float attenuation) override;
	int GetWrapModeEnum(WrapMode wrapMode);
	float fogStart = 0;
	float fogEnd = 10;
	Color fogColor;
	std::shared_ptr<Texture> usedTexture = nullptr;
	RenderingSettings lastSettings;
	// int GetCullFaceEnum(CullFace face);
	// float GetAnisotropicValueEnum(AnisotropicLevel level);

	// int GetDrawModeEnum(DrawMode drawMode);
};
#endif