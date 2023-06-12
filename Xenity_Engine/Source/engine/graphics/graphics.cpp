#include "graphics.h"
#include "../../xenity.h"
#include "../graphics/renderer/renderer.h"
#include <algorithm>
#include "../file_system/mesh_loader/wavefront_loader.h"
#include "../graphics/3d_graphics/mesh_data.h"

Camera *Graphics::usedCamera = nullptr;
int Graphics::usedShaderProgram = -1;
Material *Graphics::usedMaterial = nullptr;
int Graphics::iDrawablesCount = 0;
std::vector<IDrawable *> Graphics::orderedIDrawable;
SkyBox *Graphics::skybox = nullptr;

// ProfilerBenchmark *orderBenchmark = new ProfilerBenchmark("Order Drawables");
// ProfilerBenchmark *gameobjectScanBenchmark = new ProfilerBenchmark("Scan GameObjects");

GameObject *downPlane = nullptr;
GameObject *UpPlane = nullptr;
GameObject *frontPlane = nullptr;
GameObject *backPlane = nullptr;
GameObject *leftPlane = nullptr;
GameObject *rightPlane = nullptr;
GameObject *cameraGO2 = nullptr;

MeshData *skyPlane = nullptr;

SkyBox::SkyBox(Texture *front, Texture *back, Texture *up, Texture *down, Texture *left, Texture *right)
{
	this->front = front;
	this->back = back;
	this->up = up;
	this->down = down;
	this->left = left;
	this->right = right;
}

void Graphics::SetSkybox(SkyBox *skybox_)
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

	Texture *back = new Texture("sunset_back.png", "sunset_back", false);
	back->SetWrapMode(Texture::ClampToEdge);
	Texture *down = new Texture("sunset_down.png", "sunset_down", false);
	down->SetWrapMode(Texture::ClampToEdge);
	Texture *front = new Texture("sunset_front.png", "sunset_front", false);
	front->SetWrapMode(Texture::ClampToEdge);
	Texture *left = new Texture("sunset_left.png", "sunset_left", false);
	left->SetWrapMode(Texture::ClampToEdge);
	Texture *right = new Texture("sunset_right.png", "sunset_right", false);
	right->SetWrapMode(Texture::ClampToEdge);
	Texture *up = new Texture("sunset_up.png", "sunset_up", false);
	up->SetWrapMode(Texture::ClampToEdge);

	SkyBox *skybox = new SkyBox(front, back, up, down, left, right);
	SetSkybox(skybox);

	skyPlane = WavefrontLoader::LoadFromRawData("Plane2Triangulate.obj");
}

/// <summary>
/// Draw all Drawable elements
/// </summary>
void Graphics::DrawAllDrawable()
{
	Vector3 camPos = usedCamera->GetTransform()->GetPosition();
	TextManager::ClearTexts();

	Graphics::OrderDrawables();

	Engine::renderer->NewFrame();
	Engine::renderer->Clear();

	float scale = 10.01f;
	MeshManager::DrawMesh(Vector3(0, -5, 0) + camPos, Vector3(0, 180, 0), Vector3(scale), skybox->down, skyPlane, false);
	MeshManager::DrawMesh(Vector3(0, 5, 0) + camPos, Vector3(180, 180, 0), Vector3(scale), skybox->up, skyPlane, false);
	MeshManager::DrawMesh(Vector3(0, 0, 5) + camPos, Vector3(90, 0, 180), Vector3(scale), skybox->front, skyPlane, false);
	MeshManager::DrawMesh(Vector3(0, 0, -5) + camPos, Vector3(90, 0, 0), Vector3(scale), skybox->back, skyPlane, false);
	MeshManager::DrawMesh(Vector3(5, 0, 0) + camPos, Vector3(90, -90, 0), Vector3(scale), skybox->left, skyPlane, false);
	MeshManager::DrawMesh(Vector3(-5, 0, 0) + camPos, Vector3(90, 0, -90), Vector3(scale), skybox->right, skyPlane, false);

	for (int i = 0; i < iDrawablesCount; i++)
	{
		orderedIDrawable[i]->Draw();
	}
	Engine::renderer->EndFrame();
}

bool spriteComparator(const IDrawable *t1, const IDrawable *t2)
{
	const int priority1 = t1->GetDrawPriority();
	const int priority2 = t2->GetDrawPriority();

	if (priority1 <= priority2)
	{
		if (priority1 == priority2)
		{
			return t1->GetTransform()->GetPosition().z > t2->GetTransform()->GetPosition().z;
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
		IDrawable *drawableToCheck = orderedIDrawable[iDrawIndex];
		if (drawableToCheck->GetTransform()->movedLastFrame)
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

void Graphics::AddDrawable(IDrawable *drawableToPlace)
{
	orderedIDrawable.push_back(drawableToPlace);
	iDrawablesCount++;
}