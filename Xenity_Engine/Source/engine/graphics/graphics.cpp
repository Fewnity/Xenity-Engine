#include "graphics.h"
#include "../../xenity.h"
#include "../graphics/renderer/renderer.h"
#include "../graphics/renderer/renderer_opengl.h"
#include <algorithm>
#include "../file_system/mesh_loader/wavefront_loader.h"
#include "../graphics/3d_graphics/mesh_data.h"
#include "skybox.h"

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
std::shared_ptr <SkyBox> Graphics::skybox = nullptr;

ProfilerBenchmark* orderBenchmark = nullptr;
// ProfilerBenchmark *gameobjectScanBenchmark = new ProfilerBenchmark("Scan GameObjects");

std::shared_ptr <MeshData> skyPlane = nullptr;
std::shared_ptr <MeshData> rightArrow = nullptr;
std::shared_ptr <MeshData> upArrow = nullptr;
std::shared_ptr <MeshData> forwardArrow = nullptr;
std::shared_ptr <Texture> toolArrowsTexture = nullptr;

bool Graphics::isFogEnabled;
float Graphics::fogStart = 0;
float Graphics::fogEnd = 10;
Color Graphics::fogColor;
Color Graphics::skyColor;

void Graphics::SetSkybox(std::shared_ptr <SkyBox> skybox_)
{
	skybox = skybox_;
}

std::unordered_map<std::string, ReflectionEntry> Graphics::GetLightingSettingsReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	Reflection::AddReflectionVariable(reflectedVariables, Graphics::skybox, "skybox", true);
	Reflection::AddReflectionVariable(reflectedVariables, Graphics::skyColor, "skyColor", true);
	Reflection::AddReflectionVariable(reflectedVariables, Graphics::isFogEnabled, "isFogEnabled", true);
	Reflection::AddReflectionVariable(reflectedVariables, Graphics::fogStart, "fogStart", true);
	Reflection::AddReflectionVariable(reflectedVariables, Graphics::fogEnd, "fogEnd", true);
	Reflection::AddReflectionVariable(reflectedVariables, Graphics::fogColor, "fogColor", true);
	return reflectedVariables;
}

void Graphics::OnLightingSettingsReflectionUpdate()
{
	Engine::renderer->SetFog(isFogEnabled);
	Engine::renderer->SetFogValues(fogStart, fogEnd, fogColor);
}

void Graphics::Init()
{
	skyPlane = MeshManager::LoadMesh("engine_assets\\models\\Plane2Triangulate.obj");

#if defined(EDITOR)
	rightArrow = MeshManager::LoadMesh("engine_assets\\right_arrow.obj");
	upArrow = MeshManager::LoadMesh("engine_assets\\up_arrow.obj");
	forwardArrow = MeshManager::LoadMesh("engine_assets\\forward_arrow.obj");

	toolArrowsTexture = Texture::MakeTexture("engine_assets\\tool_arrows_colors.png", true);
	toolArrowsTexture->file = FileSystem::MakeFile("engine_assets\\tool_arrows_colors.png");
	toolArrowsTexture->SetFilter(Texture::Point);
	toolArrowsTexture->LoadFileReference();
#endif

	orderBenchmark = new ProfilerBenchmark("Draw", "Order Drawables");

	SetDefaultValues();

	Debug::Print("-------- Graphics initiated --------");
}

void Graphics::SetDefaultValues()
{
	isFogEnabled = false;
	fogStart = 10;
	fogEnd = 50;
	fogColor = Color::CreateFromRGB(152,152,152);
	skyColor = Color::CreateFromRGB(25, 25, 25);
	skybox.reset();
}

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
			Engine::renderer->SetClearColor(skyColor);
			Engine::renderer->Clear();

			Vector3 camPos = camera->GetTransform()->GetPosition();

			camera->UpdateProjection();
			DrawSkybox(camPos);

			Engine::renderer->SetFog(isFogEnabled);
			IDrawableTypes currentMode = Draw_3D;
			for (int drawableIndex = 0; drawableIndex < iDrawablesCount; drawableIndex++)
			{
				std::shared_ptr<IDrawable> drawable = orderedIDrawable[drawableIndex].lock();
				if (drawable->type != currentMode) 
				{
					currentMode = drawable->type;
					if (currentMode == Draw_UI) 
					{
						Engine::renderer->SetProjection2D(5, 0.03f, 100);
					}
				}
				drawable->Draw();
			}

#if defined(EDITOR)
			if (camera->isEditor)
			{
				Engine::renderer->SetFog(false);
				//Draw editor scene grid
				if (currentMode != Draw_3D)
				{
					currentMode = Draw_3D;
					camera->UpdateProjection();
				}
				Engine::renderer->ResetTransform();
				Engine::renderer->SetCameraPosition(camera);
				
				DrawEditorGrid(camPos);
				DrawEditorTool(camPos);
			}
#endif
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

	if (t1.lock()->type == Draw_3D && t2.lock()->type == Draw_3D) 
	{
		return false;
	}
	if (t2.lock()->type == Draw_UI && t1.lock()->type != Draw_UI)
	{
		return true;
	}

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
	orderBenchmark->Start();
	for (int iDrawIndex = 0; iDrawIndex < iDrawablesCount; iDrawIndex++)
	{
		std::shared_ptr<IDrawable> drawableToCheck = orderedIDrawable[iDrawIndex].lock();
		if (drawableToCheck && drawableToCheck->GetTransform()->movedLastFrame)
		{
			Engine::drawOrderListDirty = true;
			break;
		}
	}

	if (Engine::drawOrderListDirty)
	{
		std::sort(orderedIDrawable.begin(), orderedIDrawable.end(), spriteComparator);
		Engine::drawOrderListDirty = false;
	}
	orderBenchmark->Stop();
}

void Graphics::AddDrawable(std::weak_ptr<IDrawable> drawableToAdd)
{
	orderedIDrawable.push_back(drawableToAdd);
	iDrawablesCount++;
	Engine::drawOrderListDirty = true;
}

void Graphics::RemoveDrawable(std::weak_ptr<IDrawable> drawableToRemove)
{
	iDrawablesCount = (int)orderedIDrawable.size(); // TODO remove?
	for (int i = 0; i < iDrawablesCount; i++)
	{
		if (orderedIDrawable[i].lock() == drawableToRemove.lock())
		{
			orderedIDrawable.erase(orderedIDrawable.begin() + i);
			iDrawablesCount--;
			break;
		}
	}
}

void Graphics::DrawSkybox(Vector3 & cameraPosition)
{
	if (skybox)
	{
		Engine::renderer->SetFog(false);
		float scaleF = 10.01f;
		Vector3 scale = Vector3(scaleF);
		MeshManager::DrawMesh(Vector3(0, -5, 0) + cameraPosition, Vector3(0, 180, 0), scale, skybox->down, skyPlane, false, false, false);
		MeshManager::DrawMesh(Vector3(0, 5, 0) + cameraPosition, Vector3(180, 180, 0), scale, skybox->up, skyPlane, false, false, false);
		MeshManager::DrawMesh(Vector3(0, 0, 5) + cameraPosition, Vector3(90, 0, 180), scale, skybox->front, skyPlane, false, false, false);
		MeshManager::DrawMesh(Vector3(0, 0, -5) + cameraPosition, Vector3(90, 0, 0), scale, skybox->back, skyPlane, false, false, false);
		MeshManager::DrawMesh(Vector3(5, 0, 0) + cameraPosition, Vector3(90, -90, 0), scale, skybox->left, skyPlane, false, false, false);
		MeshManager::DrawMesh(Vector3(-5, 0, 0) + cameraPosition, Vector3(90, 0, -90), scale, skybox->right, skyPlane, false, false, false);
	}
}

void Graphics::DrawEditorGrid(Vector3& cameraPosition)
{
	Color color = Color::CreateFromRGBAFloat(0.7f, 0.7f, 0.7f, 0.2f);

	int gridAxis = 0;
	float distance;
	if (gridAxis == 0)
		distance = abs(cameraPosition.y);
	else if (gridAxis == 1)
		distance = abs(cameraPosition.x);
	else //if (gridAxis == 2)
		distance = abs(cameraPosition.z);

	// Get the coef for grid lineCount by using the camera distance
	int coef = 1;
	while (coef < distance / 10)
	{
		coef *= 10;
	}

	if (distance < 0.7f)
		distance = 0.7f;

	float lineLenght = 20 * distance;
	float lineCount = lineLenght / coef;

	if (gridAxis == 0)
	{
		// For XZ
		for (int z = -lineCount + cameraPosition.z / coef; z < lineCount + cameraPosition.z / coef; z++)
		{
			float zPos = z * coef;
			Engine::renderer->DrawLine(Vector3(-lineLenght - cameraPosition.x, 0, zPos), Vector3(lineLenght - cameraPosition.x, 0, zPos), color);
		}
		for (int x = -lineCount + cameraPosition.x / coef; x < lineCount + cameraPosition.x / coef; x++)
		{
			float xPos = -x * coef;
			Engine::renderer->DrawLine(Vector3(xPos, 0, -lineLenght + cameraPosition.z), Vector3(xPos, 0, lineLenght + cameraPosition.z), color);
		}
	}
	else if (gridAxis == 1)
	{
		//For YZ
		for (int z = -lineCount + cameraPosition.z / coef; z < lineCount + cameraPosition.z / coef; z++)
		{
			float zPos = z * coef;
			Engine::renderer->DrawLine(Vector3(0, -lineLenght - cameraPosition.y, zPos), Vector3(0, lineLenght - cameraPosition.y, zPos), color);
		}
		for (int y = -lineCount + cameraPosition.y / coef; y < lineCount + cameraPosition.y / coef; y++)
		{
			float yPos = -y * coef;
			Engine::renderer->DrawLine(Vector3(0, yPos, -lineLenght + cameraPosition.z), Vector3(0, yPos, lineLenght + cameraPosition.z), color);
		}
	}
	else if (gridAxis == 2)
	{
		// For XY
		for (int x = -lineCount + cameraPosition.x / coef; x < lineCount + cameraPosition.x / coef; x++)
		{
			float xPos = x * coef;
			Engine::renderer->DrawLine(Vector3(xPos, -lineLenght - cameraPosition.y, 0), Vector3(xPos, lineLenght - cameraPosition.y, 0), color);
		}
		for (int y = -lineCount + cameraPosition.y / coef; y < lineCount + cameraPosition.y / coef; y++)
		{
			float yPos = -y * coef;
			Engine::renderer->DrawLine(Vector3(-lineLenght + cameraPosition.x, yPos, 0), Vector3(lineLenght + cameraPosition.x, yPos, 0), color);
		}
	}
}

void Graphics::DrawEditorTool(Vector3& cameraPosition)
{
	// Draw tool
	if (Engine::selectedGameObject.lock())
	{
		Vector3 selectedGOPos = Engine::selectedGameObject.lock()->GetTransform()->GetPosition();
		Vector3 selectedGoRot = Engine::selectedGameObject.lock()->GetTransform()->GetRotation();
		float dist = Vector3::Distance(selectedGOPos, cameraPosition);
		dist /= 40;
		Vector3 scale = Vector3(dist);
		MeshManager::DrawMesh(selectedGOPos, selectedGoRot, scale, toolArrowsTexture, rightArrow, false, false, false);
		MeshManager::DrawMesh(selectedGOPos, selectedGoRot, scale, toolArrowsTexture, upArrow, false, false, false);
		MeshManager::DrawMesh(selectedGOPos, selectedGoRot, scale, toolArrowsTexture, forwardArrow, false, false, false);
	}
}
