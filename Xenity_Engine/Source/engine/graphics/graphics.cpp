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

Texture *back = nullptr;
Texture *down = nullptr;
Texture *front = nullptr;
Texture *left = nullptr;
Texture *right = nullptr;
Texture *up = nullptr;

void Graphics::Init()
{
	cameraGO2 = GameObject::FindGameObjectByName("Camera");

	back = new Texture("sky6_BK_low.jpg", "sky6_BK_low");
	back->SetWrapMode(Texture::ClampToEdge);
	down = new Texture("sky6_DN_low.jpg", "sky6_DN_low");
	down->SetWrapMode(Texture::ClampToEdge);
	front = new Texture("sky6_FR_low.jpg", "sky6_FR_low");
	front->SetWrapMode(Texture::ClampToEdge);
	left = new Texture("sky6_LF_low.jpg", "sky6_LF_low");
	left->SetWrapMode(Texture::ClampToEdge);
	right = new Texture("sky6_RT_low.jpg", "sky6_RT_low");
	right->SetWrapMode(Texture::ClampToEdge);
	up = new Texture("sky6_UP_low.jpg", "sky6_UP_low");
	up->SetWrapMode(Texture::ClampToEdge);

	skyPlane = WavefrontLoader::LoadFromRawData("Plane2Triangulate.obj");
}

/// <summary>
/// Draw all Drawable elements
/// </summary>
void Graphics::DrawAllDrawable()
{
	Vector3 camPos = cameraGO2->GetTransform()->GetPosition();
	TextManager::ClearTexts();

	Graphics::OrderDrawables();

	Engine::renderer->NewFrame();
	Engine::renderer->Clear();

	MeshManager::DrawMesh(Vector3(0, -5, 0) + camPos, Vector3(0, 0, 0), Vector3(10, 10, 10), down, skyPlane, false);
	MeshManager::DrawMesh(Vector3(0, 5, 0) + camPos, Vector3(180, 0, 0), Vector3(10, 10, 10), up, skyPlane, false);
	MeshManager::DrawMesh(Vector3(0, 0, 5) + camPos, Vector3(90, 0, 180), Vector3(10, 10, 10), front, skyPlane, false);
	MeshManager::DrawMesh(Vector3(0, 0, -5) + camPos, Vector3(90, 0, 0), Vector3(10, 10, 10), back, skyPlane, false);
	MeshManager::DrawMesh(Vector3(5, 0, 0) + camPos, Vector3(90, -90, 0), Vector3(10, 10, 10), left, skyPlane, false);
	MeshManager::DrawMesh(Vector3(-5, 0, 0) + camPos, Vector3(90, 0, -90), Vector3(10, 10, 10), right, skyPlane, false);

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
	// return (t1->GetDrawPriority() < t2->GetDrawPriority()) || (t1->GetDrawPriority() == t2->GetDrawPriority() && t1->GetGameObject()->transform.GetPosition().z > t2->GetGameObject()->transform.GetPosition().z);
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