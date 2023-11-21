#if defined(_EE)
include "renderer_gskit.h"

#include "../../../xenity.h"
#include "../3d_graphics/mesh_data.h"
#include "../../tools/profiler_benchmark.h"

#include <memory>
#include <glm/gtc/type_ptr.hpp>

RendererGsKit::RendererGsKit()
{
}

int RendererGsKit::Init()
{
	int result = 0;

	return result;
}

void RendererGsKit::Setup()
{
}

void RendererGsKit::Stop()
{
}

void RendererGsKit::NewFrame()
{
}

void RendererGsKit::EndFrame()
{
}

void RendererGsKit::SetViewport(int x, int y, int width, int height)
{
}

void RendererGsKit::SetClearColor(const Color &color)
{
}

void RendererGsKit::SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane)
{
}

void RendererGsKit::SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane, float aspect)
{
}

void RendererGsKit::ResetView()
{
}

void RendererGsKit::SetCameraPosition(const std::shared_ptr<Camera> &camera)
{
}

void RendererGsKit::ResetTransform()
{
}

void RendererGsKit::SetTransform(const Vector3 &position, const Vector3 &rotation, const Vector3 &scale, bool resetTransform)
{
}

void RendererGsKit::SetTransform(const glm::mat4 &mat)
{
}

void RendererGsKit::BindTexture(const std::shared_ptr<Texture> &texture)
{
}

void RendererGsKit::ApplyTextureFilters(const std::shared_ptr<Texture> &texture)
{
}

void RendererGsKit::DrawMeshData(const std::shared_ptr<MeshData> &meshData, const std::vector<std::shared_ptr<Texture>> &textures, RenderingSettings &settings)
{
}

void RendererGsKit::DrawLine(const Vector3 &a, const Vector3 &b, const Color &color, RenderingSettings &settings)
{
}

unsigned int RendererGsKit::CreateNewTexture()
{
	return 0;
}

void RendererGsKit::DeleteTexture(Texture *texture)
{
}

void RendererGsKit::SetTextureData(const std::shared_ptr<Texture> &texture, unsigned int textureType, const unsigned char *buffer)
{
}

void RendererGsKit::SetLight(int lightIndex, const Vector3 &lightPosition, float intensity, Color color, Light::LightType type, float attenuation)
{
}

void RendererGsKit::DisableAllLight()
{
}

void RendererGsKit::Setlights(const std::shared_ptr<Camera> &camera)
{
}

void RendererGsKit::Clear()
{
}

void RendererGsKit::SetFog(bool active)
{
}

void RendererGsKit::SetFogValues(float start, float end, const Color &color)
{
}

void RendererGsKit::DeleteSubMeshData(MeshData::SubMesh *subMesh)
{
}

void RendererGsKit::UploadMeshData(const std::shared_ptr<MeshData> &meshData)
{
}

#endif