#include "renderer_opengl.h"

#include "../../../xenity.h"

// #include <glad/glad.h>
// #define GLFW_INCLUDE_NONE
// #include <GLFW/glfw3.h>
// #include <glm/gtc/type_ptr.hpp>

#ifdef __PSP__
static unsigned int __attribute__((aligned(16))) list[262144];
#include <pspkernel.h>
#define GUGL_IMPLEMENTATION
#include "../../../psp/gu2gl.h"
#endif

#ifdef __vita__
#include <vitaGL.h>
#endif

RendererOpengl::RendererOpengl()
{
}

int RendererOpengl::Init()
{
	int result = 1;
#ifdef __PSP__
	guglInit(list);
#elif __vita__
	result = vglInit(0x100000);
#else
	result = glfwInit();
#endif

	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_FRONT);

	return result;
}

void RendererOpengl::Stop()
{
#ifdef __PSP__
	guglTerm();
#elif __vita__
	vglEnd();
#else
	glfwTerminate();
#endif
}

void RendererOpengl::NewFrame()
{
#ifdef __PSP__
	guglStartFrame(list, GL_FALSE);
#endif
}

void RendererOpengl::EndFrame()
{
#ifdef __PSP__
	guglSwapBuffers(GL_TRUE, GL_FALSE);
#elif __vita__
	vglSwapBuffers(GL_FALSE);
#endif
}

void RendererOpengl::SetClearColor(Color color)
{
#ifdef __PSP__
	// Convert float RGBA color to unsigned int ABGR color
	RGBA vec4Color = color.GetRGBA();
	unsigned int ABGR = ((int)(vec4Color.a * 256) << 24) + ((int)(vec4Color.b * 256) << 16) + ((int)(vec4Color.g * 256) << 8) + ((int)(vec4Color.r * 256) << 0);
	glClearColor(ABGR);
#else
	RGBA vec4Color = color.GetRGBA();
	glClearColor(vec4Color.r, vec4Color.g, vec4Color.b, vec4Color.a);
#endif
}

void RendererOpengl::SetProjection2D(float projectionSize, float nearClippingPlane, float farClippingPlane)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float halfRatio = ((float)Window::GetWidth() / (float)Window::GetHeight()) / 2.0f * 10;
	float halfOne = 0.5f * 10;
	glOrtho(-halfRatio, halfRatio, -halfOne, halfOne, 0.1f, 100.0f);

	// #ifdef __PSP__
	// 	glMatrixMode(GL_MODEL);
	// #else
	// 	glMatrixMode(GL_MODELVIEW);
	// #endif
}

void RendererOpengl::SetProjection3D(float fov, float nearClippingPlane, float farClippingPlane)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
#ifdef __PSP__
	glPerspective(fov, (float)Window::GetWidth() / (float)(float)Window::GetHeight(), 0.06f, 100);
#elif __vita__
	gluPerspective(fov, (float)Window::GetWidth() / (float)(float)Window::GetHeight(), nearClippingPlane, farClippingPlane);
#endif

	// #ifdef __PSP__
	// 	glMatrixMode(GL_MODEL);
	// #else
	// 	glMatrixMode(GL_MODELVIEW);
	// #endif
}

void RendererOpengl::SetCameraPosition(Camera *camera)
{
	Transform *trans = camera->GetTransform();
#ifdef __PSP__
	glMatrixMode(GL_VIEW);
	glLoadIdentity();

	gluRotateZ((-trans->GetRotation().z) / 180.0f * 3.14159f);
	gluRotateX(trans->GetRotation().x / 180.0f * 3.14159f);
	gluRotateY((trans->GetRotation().y + 180) / 180.0f * 3.14159f);

	glTranslatef(trans->GetPosition().x, -trans->GetPosition().y, -trans->GetPosition().z);
#else
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(-trans->GetRotation().z, 0, 0, 1);
	glRotatef(trans->GetRotation().x, 1, 0, 0);
	glRotatef(trans->GetRotation().y + 180, 0, 1, 0);
	glTranslatef(trans->GetPosition().x, -trans->GetPosition().y, -trans->GetPosition().z);
#endif
}

void RendererOpengl::ResetTransform()
{
}

void RendererOpengl::SetTransform(Vector3 position, Vector3 rotation, Vector3 scale, bool resetTransform)
{
#ifdef __PSP__
	glMatrixMode(GL_MODEL);
	if (resetTransform)
		glLoadIdentity();
#else
	glMatrixMode(GL_MODELVIEW);
#endif
	glTranslatef(position.x, position.y, position.z);

#ifdef __PSP__
	glRotatefXYZ(rotation.x * 3.14159265359 / 180.0f, rotation.y * 3.14159265359 / 180.0f, rotation.z * 3.14159265359 / 180.0f);
#elif __vita__
	glRotatef(rotation.z, 0, 0, 1);
	glRotatef(rotation.y, 0, 1, 0);
	glRotatef(rotation.x, 1, 0, 0);
#endif

	glScalef(scale.x, scale.y, scale.z);
}

void RendererOpengl::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT /*| GL_STENCIL_BUFFER_BIT*/);
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}