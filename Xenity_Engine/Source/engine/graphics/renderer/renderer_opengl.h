#pragma once

#include "renderer.h"

class RendererOpengl : public Renderer
{
public:
	RendererOpengl();
	int Init();
	void Stop();
	void NewFrame();
	void EndFrame();
	void SetClearColor(Color color);
	void SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane);
	void SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane);
	void Clear();
};
