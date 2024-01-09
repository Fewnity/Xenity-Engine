#include "graphics.h"

#include "iDrawable.h"

#include "renderer/renderer.h"
#include "renderer/renderer_opengl.h"

#include "2d_graphics/sprite_manager.h"
#include "ui/text_manager.h"

#include "3d_graphics/mesh_manager.h"
#include "3d_graphics/mesh_data.h"

#include "material.h"

#include "skybox.h"
#include "camera.h"

#include <engine/game_elements/transform.h>
#include <engine/game_elements/gameobject.h>
#include <engine/engine.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/network/network.h>

#include <engine/tools/profiler_benchmark.h>

#include <engine/file_system/mesh_loader/wavefront_loader.h>

#if defined(EDITOR)
#include <editor/editor.h>
#include <editor/ui/menus/scene_menu.h>
#include <editor/tool_mode.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
#include <glad/glad.h>
#endif

#include <algorithm>
#include <engine/debug/debug.h>


std::vector<std::weak_ptr<Camera>> Graphics::cameras;
std::weak_ptr<Camera> Graphics::usedCamera;
bool Graphics::needUpdateCamera = true;
int Graphics::iDrawablesCount = 0;

std::vector<std::weak_ptr<IDrawable>> Graphics::orderedIDrawable;

std::vector<std::weak_ptr<IDrawable>> Graphics::noTransparentDrawable;
std::vector<std::weak_ptr<IDrawable>> Graphics::transparentDrawable;
std::vector<std::weak_ptr<IDrawable>> Graphics::spriteDrawable;
std::vector<std::weak_ptr<IDrawable>> Graphics::uiDrawable;

std::shared_ptr <SkyBox> Graphics::skybox = nullptr;
bool Graphics::drawOrderListDirty = true;

std::shared_ptr<ProfilerBenchmark> orderBenchmark = nullptr;
std::shared_ptr<ProfilerBenchmark> skyboxBenchmark = nullptr;
std::shared_ptr<ProfilerBenchmark> drawAllBenchmark = nullptr;
std::shared_ptr<ProfilerBenchmark> drawMeshBenchmark = nullptr;
std::shared_ptr<ProfilerBenchmark> drawEndFrameBenchmark = nullptr;

std::shared_ptr <MeshData> skyPlane = nullptr;

bool Graphics::isFogEnabled;
float Graphics::fogStart = 0;
float Graphics::fogEnd = 10;
Color Graphics::fogColor;
Color Graphics::skyColor;
std::shared_ptr <Shader> Graphics::currentShader = nullptr;
std::shared_ptr <Material> Graphics::currentMaterial = nullptr;
IDrawableTypes Graphics::currentMode = Draw_3D;

bool Graphics::UseOpenGLFixedFunctions = false;

void Graphics::SetSkybox(const std::shared_ptr<SkyBox>& skybox_)
{
	skybox = skybox_;
}

ReflectiveData Graphics::GetLightingSettingsReflection()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, Graphics::skybox, "skybox", true);
	Reflective::AddVariable(reflectedVariables, Graphics::skyColor, "skyColor", true);
	Reflective::AddVariable(reflectedVariables, Graphics::isFogEnabled, "isFogEnabled", true);
	Reflective::AddVariable(reflectedVariables, Graphics::fogStart, "fogStart", true);
	Reflective::AddVariable(reflectedVariables, Graphics::fogEnd, "fogEnd", true);
	Reflective::AddVariable(reflectedVariables, Graphics::fogColor, "fogColor", true);
	return reflectedVariables;
}

void Graphics::OnLightingSettingsReflectionUpdate()
{
	Engine::GetRenderer().SetFog(isFogEnabled);
	Engine::GetRenderer().SetFogValues(fogStart, fogEnd, fogColor);
}

void Graphics::Init()
{
#if defined(__PSP__) || defined(_EE)
	UseOpenGLFixedFunctions = true;
#endif

	skyPlane = MeshManager::LoadMesh("engine_assets\\models\\PlaneTriangulate.obj");

	orderBenchmark = std::make_shared<ProfilerBenchmark>("Draw", "Order Drawables");
	skyboxBenchmark = std::make_shared <ProfilerBenchmark>("Draw", "Skybox");
	drawAllBenchmark = std::make_shared <ProfilerBenchmark>("Draw", "Drawables");
	drawMeshBenchmark = std::make_shared <ProfilerBenchmark>("Draw", "Mesh");
	drawEndFrameBenchmark = std::make_shared <ProfilerBenchmark>("Draw", "End frame");

	SetDefaultValues();

	Debug::Print("-------- Graphics initiated --------");


	Shader::Init();
	SpriteManager::Init();
	MeshManager::Init();
	TextManager::Init();
}

void Graphics::SetDefaultValues()
{
	isFogEnabled = false;
	fogStart = 10;
	fogEnd = 50;
	fogColor = Color::CreateFromRGB(152, 152, 152);
	skyColor = Color::CreateFromRGB(25, 25, 25);
	skybox.reset();
}

void Graphics::Draw()
{
	/*auto camera = usedCamera.lock();
	if (!camera)
	{
		Debug::PrintWarning("[Graphics::DrawAllDrawable] There is no camera for rendering");
	}*/

	Graphics::OrderDrawables();

	Engine::GetRenderer().NewFrame();

	size_t cameraCount = cameras.size();
	int matCount = AssetManager::GetMaterialCount();

	for (size_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++)
	{
		usedCamera = cameras[cameraIndex].lock();
		//auto camera = usedCamera.lock();
		if (usedCamera.lock()->GetIsEnabled() && usedCamera.lock()->GetGameObject()->GetLocalActive())
		{
			// Set material as dirty
			for (int materialIndex = 0; materialIndex < matCount; materialIndex++)
			{
				Material* mat = AssetManager::GetMaterial(materialIndex);
				mat->updated = false;
			}

			currentMode = Draw_3D;

			needUpdateCamera = true;

			// Update camera and bind frame buffer
			usedCamera.lock()->UpdateProjection();
			usedCamera.lock()->BindFrameBuffer();
			const Vector3 camPos = usedCamera.lock()->GetTransform()->GetPosition();

			Engine::GetRenderer().SetClearColor(skyColor);
			Engine::GetRenderer().Clear();

			if (UseOpenGLFixedFunctions)
			{
				Engine::GetRenderer().SetCameraPosition(usedCamera.lock());
				Engine::GetRenderer().Setlights(usedCamera.lock());
			}

			skyboxBenchmark->Start();
			DrawSkybox(camPos);
			skyboxBenchmark->Stop();

			Engine::GetRenderer().SetFog(isFogEnabled);

			drawAllBenchmark->Start();

			size_t noTransCount = noTransparentDrawable.size();
			size_t transCount = transparentDrawable.size();
			size_t spriteCount = spriteDrawable.size();
			size_t uiCount = uiDrawable.size();

			for (size_t drawableIndex = 0; drawableIndex < noTransCount; drawableIndex++)
			{
				std::shared_ptr<IDrawable> drawable = noTransparentDrawable[drawableIndex].lock();
				drawable->Draw();
			}
			currentMode = Draw_2D;
			for (size_t drawableIndex = 0; drawableIndex < spriteCount; drawableIndex++)
			{
				std::shared_ptr<IDrawable> drawable = spriteDrawable[drawableIndex].lock();
				drawable->Draw();
			}
			if(!usedCamera.lock()->isEditor)
				currentMode = Draw_UI;
			if (UseOpenGLFixedFunctions)
			{
				Engine::GetRenderer().ResetView();
				Engine::GetRenderer().SetProjection2D(5, 0.03f, 100);
			}
			for (size_t drawableIndex = 0; drawableIndex < uiCount; drawableIndex++)
			{
				std::shared_ptr<IDrawable> drawable = uiDrawable[drawableIndex].lock();
				drawable->Draw();
			}

			drawAllBenchmark->Stop();

#if defined(EDITOR)
			if (usedCamera.lock()->isEditor)
			{
				Engine::GetRenderer().SetFog(false);

				//Draw editor scene grid
				if (currentMode != Draw_3D)
				{
					currentMode = Draw_3D;
					if (UseOpenGLFixedFunctions)
					{
						usedCamera.lock()->UpdateProjection();
					}
				}

				Engine::GetRenderer().ResetTransform();
				Engine::GetRenderer().SetCameraPosition(usedCamera.lock());

				// Currently lines do not support shaders
				if (!UseOpenGLFixedFunctions)
				{
					Engine::GetRenderer().UseShaderProgram(0);
					Graphics::currentShader = nullptr;
					Graphics::currentMaterial = nullptr;
				}

				if (usedCamera.lock()->GetProjectionType() == ProjectionTypes::Perspective)
				{
					Engine::GetRenderer().SetProjection3D(usedCamera.lock()->GetFov(), usedCamera.lock()->GetNearClippingPlane(), usedCamera.lock()->GetFarClippingPlane(), usedCamera.lock()->GetAspectRatio());
				}
				else
				{
					Engine::GetRenderer().SetProjection2D(usedCamera.lock()->GetProjectionSize(), usedCamera.lock()->GetNearClippingPlane(), usedCamera.lock()->GetFarClippingPlane());
				}

				DrawEditorGrid(camPos);

				for (int i = 0; i < GameplayManager::componentsCount; i++)
				{
					if (auto component = GameplayManager::orderedComponents[i].lock())
					{
						if (component->GetGameObject()->GetLocalActive() && component->GetIsEnabled())
						{
							component->OnDrawGizmos();

							if (Editor::GetSelectedGameObject() == component->GetGameObject())
								component->OnDrawGizmosSelected();
						}
					}
				}

				DrawEditorTool(camPos);
			}
#endif
			usedCamera.lock()->CopyMultiSampledFrameBuffer();
		}
	}

	if (NetworkManager::needDrawMenu)
		NetworkManager::DrawNetworkSetupMenu();

#if defined(EDITOR)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	Engine::GetRenderer().SetClearColor(Color::CreateFromRGB(15, 15, 15));
	Engine::GetRenderer().Clear();
#endif
	drawEndFrameBenchmark->Start();
	Engine::GetRenderer().EndFrame();
	drawEndFrameBenchmark->Stop();

	//usedCamera.reset();
}

bool spriteComparator(const std::weak_ptr<IDrawable>& t1, const std::weak_ptr<IDrawable>& t2)
{
	if (t1.lock()->type == Draw_3D && t2.lock()->type == Draw_3D)
	{
		return false;
	}
	if (t2.lock()->type == Draw_2D && t1.lock()->type == Draw_3D)
	{
		return true;
	}
	if (t1.lock()->type == Draw_2D && t2.lock()->type == Draw_3D)
	{
		return false;
	}
	if (t2.lock()->type == Draw_UI && t1.lock()->type != Draw_UI)
	{
		return true;
	}

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
	orderBenchmark->Start();

	//if (drawOrderListDirty)
	//{
	noTransparentDrawable.clear();
	transparentDrawable.clear();
	spriteDrawable.clear();
	uiDrawable.clear();

	//noTransparentDrawable.reserve(iDrawablesCount);
	//spriteDrawable.reserve(iDrawablesCount);
	//uiDrawable.reserve(iDrawablesCount);

	for (int iDrawIndex = 0; iDrawIndex < iDrawablesCount; iDrawIndex++)
	{
		std::shared_ptr<IDrawable> drawableToCheck = orderedIDrawable[iDrawIndex].lock();
		if (drawableToCheck->type == Draw_3D)
		{
			noTransparentDrawable.push_back(drawableToCheck);
		}
		else if (drawableToCheck->type == Draw_2D)
		{
			spriteDrawable.push_back(drawableToCheck);
		}
		else
		{
			uiDrawable.push_back(drawableToCheck);
		}
	}
	//}
	/*if (drawOrderListDirty)
	{
		std::sort(orderedIDrawable.begin(), orderedIDrawable.end(), spriteComparator);
		drawOrderListDirty = false;
	}*/
	orderBenchmark->Stop();
}

void Graphics::DeleteAllDrawables()
{
	Graphics::orderedIDrawable.clear();
	iDrawablesCount = 0;
}

void Graphics::AddDrawable(const std::weak_ptr<IDrawable>& drawableToAdd)
{
	orderedIDrawable.push_back(drawableToAdd);
	iDrawablesCount++;
	Graphics::SetDrawOrderListAsDirty();
}

void Graphics::RemoveDrawable(const std::weak_ptr<IDrawable>& drawableToRemove)
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

void Graphics::RemoveCamera(const std::weak_ptr<Camera>& cameraToRemove)
{
	size_t cameraCount = cameras.size();
	for (size_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++)
	{
		auto cam = cameras[cameraIndex].lock();
		if (cam && cam == cameraToRemove.lock())
		{
			cameras.erase(cameras.begin() + cameraIndex);
			break;
		}
	}
}

void Graphics::DrawMesh(const std::shared_ptr<MeshData>& meshData, const std::vector<std::shared_ptr<Texture>>& textures, RenderingSettings& renderSettings, const glm::mat4& matrix, const std::shared_ptr<Material>& material, bool forUI)
{
	if (!meshData || !usedCamera.lock())
		return;

	drawMeshBenchmark->Start();

	if (!UseOpenGLFixedFunctions)
	{
		if (!material)
			return;

		material->Use();

		if (!Graphics::currentShader)
			return;

		Graphics::currentShader->SetShaderModel(matrix);
	}
	else
	{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64) // The PSP does not need to set the camera position every draw call
		if (!forUI)
			Engine::GetRenderer().SetCameraPosition(usedCamera.lock());
#endif
		Engine::GetRenderer().SetTransform(matrix);
	}

	Engine::GetRenderer().DrawMeshData(meshData, textures, renderSettings);
	drawMeshBenchmark->Stop();
}

void Graphics::SetDrawOrderListAsDirty()
{
	drawOrderListDirty = true;
}

void Graphics::DrawSkybox(const Vector3& cameraPosition)
{
	if (skybox)
	{
		Engine::GetRenderer().SetFog(false);
		float scaleF = 10.01f;
		Vector3 scale = Vector3(scaleF);

		RenderingSettings renderSettings = RenderingSettings();
		renderSettings.invertFaces = false;
		renderSettings.useBlend = false;
		renderSettings.useDepth = false;
		renderSettings.useTexture = true;
		renderSettings.useLighting = false;

		MeshManager::DrawMesh(Vector3(0, -5, 0) + cameraPosition, Vector3(0, 180, 0), scale, skybox->down, skyPlane, renderSettings, AssetManager::unlitMaterial);
		MeshManager::DrawMesh(Vector3(0, 5, 0) + cameraPosition, Vector3(180, 180, 0), scale, skybox->up, skyPlane, renderSettings, AssetManager::unlitMaterial);
		MeshManager::DrawMesh(Vector3(0, 0, 5) + cameraPosition, Vector3(90, 0, 180), scale, skybox->front, skyPlane, renderSettings, AssetManager::unlitMaterial);
		MeshManager::DrawMesh(Vector3(0, 0, -5) + cameraPosition, Vector3(90, 0, 0), scale, skybox->back, skyPlane, renderSettings, AssetManager::unlitMaterial);
		MeshManager::DrawMesh(Vector3(5, 0, 0) + cameraPosition, Vector3(90, -90, 0), scale, skybox->left, skyPlane, renderSettings, AssetManager::unlitMaterial);
		MeshManager::DrawMesh(Vector3(-5, 0, 0) + cameraPosition, Vector3(90, 0, -90), scale, skybox->right, skyPlane, renderSettings, AssetManager::unlitMaterial);
	}
}
#if defined(EDITOR)
void Graphics::DrawEditorGrid(const Vector3& cameraPosition)
{
	Color color = Color::CreateFromRGBAFloat(0.7f, 0.7f, 0.7f, 0.2f);

	/*glEnable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		glEnableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);*/

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
	RenderingSettings renderSettings = RenderingSettings();
	renderSettings.useBlend = true;
	renderSettings.useDepth = true;
	renderSettings.useLighting = false;
	renderSettings.useTexture = false;

	if (gridAxis == 0)
	{
		// For XZ
		for (int z = -lineCount + cameraPosition.z / coef; z < lineCount + cameraPosition.z / coef; z++)
		{
			int zPos = z * coef;
			Engine::GetRenderer().DrawLine(Vector3(-lineLenght - cameraPosition.x, 0, zPos), Vector3(lineLenght - cameraPosition.x, 0, zPos), color, renderSettings);
		}
		for (int x = -lineCount + cameraPosition.x / coef; x < lineCount + cameraPosition.x / coef; x++)
		{
			int xPos = -x * coef;
			Engine::GetRenderer().DrawLine(Vector3(xPos, 0, -lineLenght + cameraPosition.z), Vector3(xPos, 0, lineLenght + cameraPosition.z), color, renderSettings);
		}
	}
	else if (gridAxis == 1)
	{
		//For YZ
		for (int z = -lineCount + cameraPosition.z / coef; z < lineCount + cameraPosition.z / coef; z++)
		{
			float zPos = z * coef;
			Engine::GetRenderer().DrawLine(Vector3(0, -lineLenght - cameraPosition.y, zPos), Vector3(0, lineLenght - cameraPosition.y, zPos), color, renderSettings);
		}
		for (int y = -lineCount + cameraPosition.y / coef; y < lineCount + cameraPosition.y / coef; y++)
		{
			float yPos = -y * coef;
			Engine::GetRenderer().DrawLine(Vector3(0, yPos, -lineLenght + cameraPosition.z), Vector3(0, yPos, lineLenght + cameraPosition.z), color, renderSettings);
		}
	}
	else if (gridAxis == 2)
	{
		// For XY
		for (int x = -lineCount + cameraPosition.x / coef; x < lineCount + cameraPosition.x / coef; x++)
		{
			int xPos = x * coef;
			Engine::GetRenderer().DrawLine(Vector3(xPos, -lineLenght - cameraPosition.y, 0), Vector3(xPos, lineLenght - cameraPosition.y, 0), color, renderSettings);
		}
		for (int y = -lineCount + cameraPosition.y / coef; y < lineCount + cameraPosition.y / coef; y++)
		{
			int yPos = -y * coef;
			Engine::GetRenderer().DrawLine(Vector3(-lineLenght + cameraPosition.x, yPos, 0), Vector3(lineLenght + cameraPosition.x, yPos, 0), color, renderSettings);
		}
	}
}

void Graphics::DrawEditorTool(const Vector3& cameraPosition)
{
	// Draw tool
	if (Editor::GetSelectedGameObject())
	{
		Vector3 selectedGoPos = Editor::GetSelectedGameObject()->GetTransform()->GetPosition();
		//if (usedCamera.lock()->isEditor) 
		//{
		//	float xOff = (-Graphics::usedCamera.lock()->GetAspectRatio() * 5) + (selectedGoPos.x * (Graphics::usedCamera.lock()->GetAspectRatio() * 10));
		//	float yOff = (-1 * 5) + (selectedGoPos.y * (1 * 10));
		//	selectedGoPos = Vector3(xOff, -yOff, 1); // Z 1 to avoid issue with near clipping plane
		//}
		Vector3 selectedGoRot = Editor::GetSelectedGameObject()->GetTransform()->GetRotation();
		if (Editor::isToolLocalMode)
			selectedGoRot = Vector3(0);

		float dist = 1;
		if (usedCamera.lock()->GetProjectionType() == ProjectionTypes::Perspective)
			dist = Vector3::Distance(selectedGoPos, cameraPosition);
		else
			dist = usedCamera.lock()->GetProjectionSize() * 1.5f;

		dist /= 40;
		Vector3 scale = Vector3(dist);

		RenderingSettings renderSettings = RenderingSettings();
		renderSettings.invertFaces = false;
		renderSettings.useBlend = false;
		renderSettings.useDepth = false;
		renderSettings.useTexture = true;
		renderSettings.useLighting = false;
		if (Editor::GetMenu<SceneMenu>()->toolMode == Tool_Move || Editor::GetMenu<SceneMenu>()->toolMode == Tool_Scale)
		{
			MeshManager::DrawMesh(selectedGoPos, selectedGoRot, scale, Editor::toolArrowsTexture, Editor::rightArrow, renderSettings, AssetManager::unlitMaterial);
			MeshManager::DrawMesh(selectedGoPos, selectedGoRot, scale, Editor::toolArrowsTexture, Editor::upArrow, renderSettings, AssetManager::unlitMaterial);
			MeshManager::DrawMesh(selectedGoPos, selectedGoRot, scale, Editor::toolArrowsTexture, Editor::forwardArrow, renderSettings, AssetManager::unlitMaterial);
		}
		else if (Editor::GetMenu<SceneMenu>()->toolMode == Tool_Rotate) 
		{
			MeshManager::DrawMesh(selectedGoPos, selectedGoRot, scale, Editor::toolArrowsTexture, Editor::rotationCircleX, renderSettings, AssetManager::unlitMaterial);
			MeshManager::DrawMesh(selectedGoPos, selectedGoRot, scale, Editor::toolArrowsTexture, Editor::rotationCircleY, renderSettings, AssetManager::unlitMaterial);
			MeshManager::DrawMesh(selectedGoPos, selectedGoRot, scale, Editor::toolArrowsTexture, Editor::rotationCircleZ, renderSettings, AssetManager::unlitMaterial);
		}
	}
}
#endif