#include "graphics.h"
#include "../../xenity.h"
#include "../graphics/renderer/renderer.h"
#include "../graphics/renderer/renderer_opengl.h"
#include <algorithm>
#include "../file_system/mesh_loader/wavefront_loader.h"
#include "../graphics/3d_graphics/mesh_data.h"

#if defined(_WIN32) || defined(_WIN64)
#include <glad/glad.h>
#endif

std::weak_ptr<Camera> Graphics::usedCamera;
bool Graphics::needUpdateCamera = true;
// int Graphics::usedShaderProgram = -1;
// Material *Graphics::usedMaterial = nullptr;
int Graphics::iDrawablesCount = 0;
std::vector<std::weak_ptr<IDrawable>> Graphics::orderedIDrawable;
SkyBox* Graphics::skybox = nullptr;
unsigned int Graphics::framebuffer;
unsigned int Graphics::depthframebuffer = -1;
unsigned int Graphics::framebufferTexture = -1;
Vector2Int Graphics::framebufferSize;
bool Graphics::needFremeBufferUpdate = true;
// ProfilerBenchmark *orderBenchmark = new ProfilerBenchmark("Order Drawables");
// ProfilerBenchmark *gameobjectScanBenchmark = new ProfilerBenchmark("Scan GameObjects");

MeshData* skyPlane = nullptr;

SkyBox::SkyBox(Texture* front, Texture* back, Texture* up, Texture* down, Texture* left, Texture* right)
{
	this->front = front;
	this->back = back;
	this->up = up;
	this->down = down;
	this->left = left;
	this->right = right;
}

void Graphics::SetSkybox(SkyBox* skybox_)
{
	skybox = skybox_;
}

void Graphics::UpdaterameBuffer()
{
#if defined(_WIN32) || defined(_WIN64)
	if (needFremeBufferUpdate)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		if (framebufferTexture >= 0)
		{
			glDeleteTextures(1, &framebufferTexture);
		}
		if (depthframebuffer >= 0)
		{
			glDeleteRenderbuffers(1, &depthframebuffer);
		}

		glGenTextures(1, &framebufferTexture);
		glBindTexture(GL_TEXTURE_2D, framebufferTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, framebufferSize.x, framebufferSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenRenderbuffers(1, &depthframebuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthframebuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, framebufferSize.x, framebufferSize.y);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthframebuffer);
		glViewport(0, 0, framebufferSize.x, framebufferSize.y);
		Window::SetResolution(framebufferSize.x, framebufferSize.y);
		needFremeBufferUpdate = false;
	}
#endif
}

void Graphics::ChangeFrameBufferSize(Vector2Int resolution)
{
	if (framebufferSize != resolution)
	{
		framebufferSize = resolution;
		needFremeBufferUpdate = true;
	}
}

void Graphics::Init()
{
#if defined(EDITOR)
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	ChangeFrameBufferSize(Vector2Int(1920, 1080));

#endif
	// Texture *back = new Texture("space_back.png", "space_back", false);
	// back->SetWrapMode(Texture::ClampToEdge);
	// Texture *down = new Texture("space_down.png", "space_down", false);
	// down->SetWrapMode(Texture::ClampToEdge);
	// Texture *front = new Texture("space_front.png", "space_front", false);
	// front->SetWrapMode(Texture::ClampToEdge);
	// Texture *left = new Texture("space_left.png", "space_left", false);
	// left->SetWrapMode(Texture::ClampToEdge);
	// Texture *right = new Texture("space_right.png", "space_right", false);
	// right->SetWrapMode(Texture::ClampToEdge);
	// Texture *up = new Texture("space_up.png", "space_up", false);
	// up->SetWrapMode(Texture::ClampToEdge);

	Texture* back = new Texture("assets/sunset_back.png", "sunset_back", false);
	back->SetWrapMode(Texture::ClampToEdge);
	Texture* down = new Texture("assets/sunset_down.png", "sunset_down", false);
	down->SetWrapMode(Texture::ClampToEdge);
	Texture* front = new Texture("assets/sunset_front.png", "sunset_front", false);
	front->SetWrapMode(Texture::ClampToEdge);
	Texture* left = new Texture("assets/sunset_left.png", "sunset_left", false);
	left->SetWrapMode(Texture::ClampToEdge);
	Texture* right = new Texture("assets/sunset_right.png", "sunset_right", false);
	right->SetWrapMode(Texture::ClampToEdge);
	Texture* up = new Texture("assets/sunset_up.png", "sunset_up", false);
	up->SetWrapMode(Texture::ClampToEdge);

	SkyBox* skybox = new SkyBox(front, back, up, down, left, right);
	SetSkybox(skybox);

	skyPlane = MeshManager::LoadMesh("engine_assets/models/Plane2Triangulate.obj");

	Debug::Print("-------- Graphics initiated --------");
}

/// <summary>
/// Draw all Drawable elements
/// </summary>
void Graphics::DrawAllDrawable()
{

	auto camera = usedCamera.lock();
	if (!camera)
	{
		Debug::PrintWarning("[Graphics::DrawAllDrawable] There is no camera for rendering");
	}

	Graphics::OrderDrawables();
#if defined(EDITOR)
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	UpdaterameBuffer();
#endif
	Engine::renderer->NewFrame();
	Engine::renderer->Clear();

	if (camera)
	{
		needUpdateCamera = true;
		Vector3 camPos = camera->GetTransform()->GetPosition();

		if (skybox)
		{
			float scale = 10.01f;
			MeshManager::DrawMesh(Vector3(0, -5, 0) + camPos, Vector3(0, 180, 0), Vector3(scale), skybox->down, skyPlane, false, false, false);
			MeshManager::DrawMesh(Vector3(0, 5, 0) + camPos, Vector3(180, 180, 0), Vector3(scale), skybox->up, skyPlane, false, false, false);
			MeshManager::DrawMesh(Vector3(0, 0, 5) + camPos, Vector3(90, 0, 180), Vector3(scale), skybox->front, skyPlane, false, false, false);
			MeshManager::DrawMesh(Vector3(0, 0, -5) + camPos, Vector3(90, 0, 0), Vector3(scale), skybox->back, skyPlane, false, false, false);
			MeshManager::DrawMesh(Vector3(5, 0, 0) + camPos, Vector3(90, -90, 0), Vector3(scale), skybox->left, skyPlane, false, false, false);
			MeshManager::DrawMesh(Vector3(-5, 0, 0) + camPos, Vector3(90, 0, -90), Vector3(scale), skybox->right, skyPlane, false, false, false);
		}

		for (int i = 0; i < iDrawablesCount; i++)
		{
			orderedIDrawable[i].lock()->Draw();
		}
	}

	if (NetworkManager::needDrawMenu)
		NetworkManager::DrawNetworkSetupMenu();

#if defined(EDITOR)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	Engine::renderer->Clear();
	Engine::renderer->SetClearColor(Color::CreateFromRGB(15, 16, 16));
#endif
	Engine::renderer->EndFrame();
}

bool spriteComparator(const std::weak_ptr<IDrawable> t1, const std::weak_ptr<IDrawable> t2)
{
	const int priority1 = t1.lock()->GetDrawPriority();
	const int priority2 = t2.lock()->GetDrawPriority();

	if (priority1 <= priority2)
	{
		if (priority1 == priority2)
		{
			return t1.lock()->GetTransform()->GetPosition().z > t2.lock()->GetTransform()->GetPosition().z;
		}

		return true;
	}

	return false;
}

void Graphics::OrderDrawables()
{
	// orderBenchmark->Start();
	// gameobjectScanBenchmark->Start();
	for (int iDrawIndex = 0; iDrawIndex < iDrawablesCount; iDrawIndex++)
	{
		std::weak_ptr<IDrawable> drawableToCheck = orderedIDrawable[iDrawIndex];
		if (drawableToCheck.lock()->GetTransform()->movedLastFrame)
		{
			Engine::drawOrderListDirty = true;
			break;
		}
	}
	// gameobjectScanBenchmark->Stop();

	if (Engine::drawOrderListDirty)
	{
		std::sort(orderedIDrawable.begin(), orderedIDrawable.end(), spriteComparator);
		Engine::drawOrderListDirty = false;
	}
	// orderBenchmark->Stop();
}

void Graphics::AddDrawable(std::weak_ptr<IDrawable> drawableToPlace)
{
	orderedIDrawable.push_back(drawableToPlace);
	iDrawablesCount++;
}

void Graphics::RemoveDrawable(std::weak_ptr<IDrawable> drawableToPlace)
{
	iDrawablesCount = (int)orderedIDrawable.size(); // TODO remove?
	for (int i = 0; i < iDrawablesCount; i++)
	{
		if (orderedIDrawable[i].lock() == drawableToPlace.lock())
		{
			orderedIDrawable.erase(orderedIDrawable.begin() + i);
			iDrawablesCount--;
			break;
		}
	}
}