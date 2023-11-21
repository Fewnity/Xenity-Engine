#if defined(__PSP__)
include "renderer_ge.h"

#include "../../../xenity.h"
#include "../3d_graphics/mesh_data.h"
#include "../../tools/profiler_benchmark.h"

#include <memory>
#include <glm/gtc/type_ptr.hpp>

RendererGE::RendererGE()
{
}

int RendererGE::Init()
{
	int result = 0;

	return result;
}

void RendererGE::Setup()
{
}

void RendererGE::Stop()
{
}

void RendererGE::NewFrame()
{
}

void RendererGE::EndFrame()
{
}

void RendererGE::SetViewport(int x, int y, int width, int height)
{
}

void RendererGE::SetClearColor(const Color &color)
{
}

void RendererGE::SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane)
{
}

void RendererGE::SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane, float aspect)
{
}

void RendererGE::ResetView()
{
}

void RendererGE::SetCameraPosition(const std::shared_ptr<Camera> &camera)
{
}

void RendererGE::ResetTransform()
{
}

void RendererGE::SetTransform(const Vector3 &position, const Vector3 &rotation, const Vector3 &scale, bool resetTransform)
{
}

void RendererGE::SetTransform(const glm::mat4 &mat)
{
}

void RendererGE::BindTexture(const std::shared_ptr<Texture> &texture)
{
}

void RendererGE::ApplyTextureFilters(const std::shared_ptr<Texture> &texture)
{
}

void RendererGE::DrawMeshData(const std::shared_ptr<MeshData> &meshData, const std::vector<std::shared_ptr<Texture>> &textures, RenderingSettings &settings)
{
}

void RendererGE::DrawLine(const Vector3 &a, const Vector3 &b, const Color &color, RenderingSettings &settings)
{
}

unsigned int RendererGE::CreateNewTexture()
{
	return 0;
}

void RendererGE::DeleteTexture(Texture *texture)
{
}

void RendererGE::SetTextureData(const std::shared_ptr<Texture> &texture, unsigned int textureType, const unsigned char *buffer)
{
}

void RendererGE::SetLight(int lightIndex, const Vector3 &lightPosition, float intensity, Color color, Light::LightType type, float attenuation)
{
}

void RendererGE::DisableAllLight()
{
}

void RendererGE::Setlights(const std::shared_ptr<Camera> &camera)
{
}

void RendererGE::Clear()
{
}

void RendererGE::SetFog(bool active)
{
}

void RendererGE::SetFogValues(float start, float end, const Color &color)
{
}

void RendererGE::DeleteSubMeshData(MeshData::SubMesh *subMesh)
{
}

void RendererGE::UploadMeshData(const std::shared_ptr<MeshData> &meshData)
{
}

#endif