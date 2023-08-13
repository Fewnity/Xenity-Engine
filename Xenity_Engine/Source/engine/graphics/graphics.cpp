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

std::vector<std::weak_ptr<Camera>> Graphics::cameras;
std::weak_ptr<Camera> Graphics::usedCamera;
bool Graphics::needUpdateCamera = true;
// int Graphics::usedShaderProgram = -1;
// Material *Graphics::usedMaterial = nullptr;
int Graphics::iDrawablesCount = 0;
std::vector<std::weak_ptr<IDrawable>> Graphics::orderedIDrawable;
SkyBox* Graphics::skybox = nullptr;

// ProfilerBenchmark *orderBenchmark = new ProfilerBenchmark("Order Drawables");
// ProfilerBenchmark *gameobjectScanBenchmark = new ProfilerBenchmark("Scan GameObjects");

MeshData* skyPlane = nullptr;
MeshData* dirArrows = nullptr;

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

void Graphics::Init()
{
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

	Texture* back = new Texture("test_project\\assets\\sunset_back.png", "sunset_back", false);
	back->SetWrapMode(Texture::ClampToEdge);
	Texture* down = new Texture("test_project\\assets\\sunset_down.png", "sunset_down", false);
	down->SetWrapMode(Texture::ClampToEdge);
	Texture* front = new Texture("test_project\\assets\\sunset_front.png", "sunset_front", false);
	front->SetWrapMode(Texture::ClampToEdge);
	Texture* left = new Texture("test_project\\assets\\sunset_left.png", "sunset_left", false);
	left->SetWrapMode(Texture::ClampToEdge);
	Texture* right = new Texture("test_project\\assets\\sunset_right.png", "sunset_right", false);
	right->SetWrapMode(Texture::ClampToEdge);
	Texture* up = new Texture("test_project\\assets\\sunset_up.png", "sunset_up", false);
	up->SetWrapMode(Texture::ClampToEdge);

	SkyBox* skybox = new SkyBox(front, back, up, down, left, right);
	SetSkybox(skybox);

	skyPlane = MeshManager::LoadMesh("test_project\\assets\\models\\Plane2Triangulate.obj");
	dirArrows = MeshManager::LoadMesh("test_project\\assets\\dir arrows.obj");
	Debug::Print("-------- Graphics initiated --------");
}

/// <summary>
/// Draw all Drawable elements
/// </summary>
void Graphics::DrawAllDrawable()
{
	/*auto camera = usedCamera.lock();
	if (!camera)
	{
		Debug::PrintWarning("[Graphics::DrawAllDrawable] There is no camera for rendering");
	}*/

	Graphics::OrderDrawables();

	Engine::renderer->NewFrame();

	int cameraCount = cameras.size();

	for (int cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++)
	{
		usedCamera = cameras[cameraIndex];
		auto camera = usedCamera.lock();
		if (camera && camera->GetIsEnabled() && camera->GetGameObject()->GetLocalActive())
		{
			needUpdateCamera = true;
			camera->BindFrameBuffer();
			Engine::renderer->Clear();

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
			if (camera->isEditor && Engine::selectedGameObject.lock())
			{
				Vector3 selectedGOPos = Engine::selectedGameObject.lock()->GetTransform()->GetPosition();
				Vector3 selectedGoRot = Engine::selectedGameObject.lock()->GetTransform()->GetRotation();
				float dist = Vector3::Distance(selectedGOPos, camPos);
				dist /= 40;
				MeshManager::DrawMesh(selectedGOPos, selectedGoRot, Vector3(dist, dist, dist), AssetManager::defaultTexture, dirArrows, false, false, false);
			}
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