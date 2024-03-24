#include "graphics.h"

#include "iDrawable.h"

#include "renderer/renderer.h"
#include "renderer/renderer_opengl.h"

#include "2d_graphics/sprite_manager.h"
#include "ui/text_manager.h"

#include "3d_graphics/mesh_manager.h"
#include "3d_graphics/mesh_data.h"
#include <engine/graphics/3d_graphics/lod.h>

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
#include <engine/graphics/3d_graphics/mesh_renderer.h>
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
int Graphics::lodsCount = 0;

std::vector<std::weak_ptr<IDrawable>> Graphics::orderedIDrawable;

std::vector<std::weak_ptr<IDrawable>> Graphics::noTransparentDrawable;
std::vector<std::weak_ptr<IDrawable>> Graphics::transparentDrawable;
std::vector<std::weak_ptr<IDrawable>> Graphics::spriteDrawable;
std::vector<std::weak_ptr<IDrawable>> Graphics::uiDrawable;
std::vector<std::weak_ptr<Lod>> Graphics::lods;

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
IDrawableTypes Graphics::currentMode = IDrawableTypes::Draw_3D;

bool Graphics::UseOpenGLFixedFunctions = false;

void Graphics::SetSkybox(const std::shared_ptr<SkyBox>& skybox_)
{
	skybox = skybox_;
}

ReflectiveData Graphics::GetLightingSettingsReflection()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, skybox, "skybox", true);
	Reflective::AddVariable(reflectedVariables, skyColor, "skyColor", true);
	Reflective::AddVariable(reflectedVariables, isFogEnabled, "isFogEnabled", true);
	Reflective::AddVariable(reflectedVariables, fogStart, "fogStart", true);
	Reflective::AddVariable(reflectedVariables, fogEnd, "fogEnd", true);
	Reflective::AddVariable(reflectedVariables, fogColor, "fogColor", true);
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

	Debug::Print("-------- Graphics initiated --------", true);


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

	Engine::GetRenderer().NewFrame();
	currentMaterial = nullptr;

	const size_t cameraCount = cameras.size();
	const int matCount = AssetManager::GetMaterialCount();

	for (size_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++)
	{
		usedCamera = cameras[cameraIndex].lock();
		if (usedCamera.lock()->GetIsEnabled() && usedCamera.lock()->GetGameObject()->GetLocalActive())
		{
			OrderDrawables();
			CheckLods();

			// Set material as dirty
			for (int materialIndex = 0; materialIndex < matCount; materialIndex++)
			{
				Material* mat = AssetManager::GetMaterial(materialIndex);
				mat->updated = false;
			}

			currentMode = IDrawableTypes::Draw_3D;

			needUpdateCamera = true;

			// Update camera and bind frame buffer
			usedCamera.lock()->UpdateProjection();
			usedCamera.lock()->BindFrameBuffer();
			const Vector3& camPos = usedCamera.lock()->GetTransform()->GetPosition();

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

			const size_t noTransCount = noTransparentDrawable.size();
			const size_t transCount = transparentDrawable.size();
			const size_t spriteCount = spriteDrawable.size();
			const size_t uiCount = uiDrawable.size();

			for (size_t drawableIndex = 0; drawableIndex < noTransCount; drawableIndex++)
			{
				std::shared_ptr<IDrawable> drawable = noTransparentDrawable[drawableIndex].lock();
				drawable->Draw();
			}
			for (size_t drawableIndex = 0; drawableIndex < transCount; drawableIndex++)
			{
				std::shared_ptr<IDrawable> drawable = transparentDrawable[drawableIndex].lock();
				drawable->Draw();
			}
			currentMode = IDrawableTypes::Draw_2D;
			for (size_t drawableIndex = 0; drawableIndex < spriteCount; drawableIndex++)
			{
				std::shared_ptr<IDrawable> drawable = spriteDrawable[drawableIndex].lock();
				drawable->Draw();
			}
			if (!usedCamera.lock()->isEditor)
				currentMode = IDrawableTypes::Draw_UI;
			if (UseOpenGLFixedFunctions && !usedCamera.lock()->isEditor)
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
				if (currentMode != IDrawableTypes::Draw_3D)
				{
					currentMode = IDrawableTypes::Draw_3D;
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
					currentShader = nullptr;
					currentMaterial = nullptr;
				}

				if (usedCamera.lock()->GetProjectionType() == ProjectionTypes::Perspective)
				{
					Engine::GetRenderer().SetProjection3D(usedCamera.lock()->GetFov(), usedCamera.lock()->GetNearClippingPlane(), usedCamera.lock()->GetFarClippingPlane(), usedCamera.lock()->GetAspectRatio());
				}
				else
				{
					Engine::GetRenderer().SetProjection2D(usedCamera.lock()->GetProjectionSize(), usedCamera.lock()->GetNearClippingPlane(), usedCamera.lock()->GetFarClippingPlane());
				}

				// Get the grid axis
				std::vector<std::shared_ptr<SceneMenu>> sceneMenus;
				sceneMenus = Editor::GetMenus<SceneMenu>();
				const int sceneMenuCount = sceneMenus.size();
				int gridAxis = 0;
				for (int i = 0; i < sceneMenuCount; i++)
				{
					if (sceneMenus[i]->weakCamera.lock() == usedCamera.lock())
					{
						gridAxis = sceneMenus[i]->gridAxis;
						break;
					}
				}

				DrawEditorGrid(camPos, gridAxis);
				DrawSelectedItemBoundingBox(camPos);

				for (int i = 0; i < GameplayManager::componentsCount; i++)
				{
					if (std::shared_ptr<Component> component = GameplayManager::orderedComponents[i].lock())
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
	if (t1.lock()->type == IDrawableTypes::Draw_3D && t2.lock()->type == IDrawableTypes::Draw_3D)
	{
		return false;
	}
	if (t2.lock()->type == IDrawableTypes::Draw_2D && t1.lock()->type == IDrawableTypes::Draw_3D)
	{
		return true;
	}
	if (t1.lock()->type == IDrawableTypes::Draw_2D && t2.lock()->type == IDrawableTypes::Draw_3D)
	{
		return false;
	}
	if (t2.lock()->type == IDrawableTypes::Draw_UI && t1.lock()->type != IDrawableTypes::Draw_UI)
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

bool meshComparator(const std::weak_ptr<IDrawable>& t1, const std::weak_ptr<IDrawable>& t2)
{
	const Vector3& pos1 = t1.lock()->GetTransform()->GetPosition();
	const Vector3& pos2 = t2.lock()->GetTransform()->GetPosition();
	const Vector3& camPos = Graphics::usedCamera.lock()->GetTransform()->GetPosition();
	const float dis1 = Vector3::Distance(pos1, camPos);
	const float dis2 = Vector3::Distance(pos2, camPos);

	return dis1 > dis2;
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
		const std::shared_ptr<IDrawable> drawableToCheck = orderedIDrawable[iDrawIndex].lock();
		if (drawableToCheck->type == IDrawableTypes::Draw_3D)
		{
			if(drawableToCheck->isTransparent)
				transparentDrawable.push_back(drawableToCheck);
			else
				noTransparentDrawable.push_back(drawableToCheck);
		}
		else if (drawableToCheck->type == IDrawableTypes::Draw_2D)
		{
			spriteDrawable.push_back(drawableToCheck);
		}
		else
		{
			uiDrawable.push_back(drawableToCheck);
		}
	}

	std::sort(transparentDrawable.begin(), transparentDrawable.end(), meshComparator);

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
	orderedIDrawable.clear();
	iDrawablesCount = 0;
}

void Graphics::AddDrawable(const std::weak_ptr<IDrawable>& drawableToAdd)
{
	orderedIDrawable.push_back(drawableToAdd);
	iDrawablesCount++;
	SetDrawOrderListAsDirty();
}

void Graphics::RemoveDrawable(const std::weak_ptr<IDrawable>& drawableToRemove)
{
	if (!Engine::IsRunning(true))
		return;

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

void Graphics::AddLod(const std::weak_ptr<Lod>& lodToAdd)
{
	lods.push_back(lodToAdd);
	lodsCount++;
}

void Graphics::RemoveLod(const std::weak_ptr<Lod>& lodToRemove)
{
	if (!Engine::IsRunning(true))
		return;

	for (int i = 0; i < lodsCount; i++)
	{
		if (lods[i].lock() == lodToRemove.lock())
		{
			lods.erase(lods.begin() + i);
			lodsCount--;
			break;
		}
	}
}

void Graphics::RemoveCamera(const std::weak_ptr<Camera>& cameraToRemove)
{
	const size_t cameraCount = cameras.size();
	for (size_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++)
	{
		const std::shared_ptr<Camera> cam = cameras[cameraIndex].lock();
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

		if (!currentShader)
			return;

		currentShader->SetShaderModel(matrix);
	}
	else
	{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64) // The PSP does not need to set the camera position every draw call
		if (!forUI || usedCamera.lock()->isEditor)
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
		const float scaleF = 10.01f;
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

void Graphics::CheckLods()
{
	for (int i = 0; i < lodsCount; i++)
	{
		std::shared_ptr<Lod> lod = lods[i].lock();
		if (lod) 
		{
			lod->CheckLod();
		}
	}
}

#if defined(EDITOR)

void Graphics::DrawSelectedItemBoundingBox(const Vector3& cameraPosition)
{
	if (const std::shared_ptr<GameObject> selectedGO = Editor::GetSelectedGameObject())
	{
		const std::shared_ptr<MeshRenderer> meshRenderer = selectedGO->GetComponent<MeshRenderer>();
		if (meshRenderer && meshRenderer->meshData && selectedGO->GetLocalActive() && meshRenderer->GetIsEnabled())
		{
			const Color color = Color::CreateFromRGBAFloat(0.0f, 1.0f, 1.0f, 1.0f);

			RenderingSettings renderSettings = RenderingSettings();
			renderSettings.useBlend = true;
			renderSettings.useDepth = true;
			renderSettings.useLighting = false;
			renderSettings.useTexture = false;

			const Vector3 min = meshRenderer->meshData->minBoundingBox;
			const Vector3 max = meshRenderer->meshData->maxBoundingBox;

			const Vector3 bottom0 = selectedGO->GetTransform()->transformationMatrix * glm::vec4(min.x, min.y, min.z, 1);
			const Vector3 bottom1 = selectedGO->GetTransform()->transformationMatrix * glm::vec4(min.x, min.y, max.z, 1);
			const Vector3 bottom2 = selectedGO->GetTransform()->transformationMatrix * glm::vec4(max.x, min.y, min.z, 1);
			const Vector3 bottom3 = selectedGO->GetTransform()->transformationMatrix * glm::vec4(max.x, min.y, max.z, 1);

			const Vector3 top0 = selectedGO->GetTransform()->transformationMatrix * glm::vec4(min.x, max.y, min.z, 1);
			const Vector3 top1 = selectedGO->GetTransform()->transformationMatrix * glm::vec4(min.x, max.y, max.z, 1);
			const Vector3 top2 = selectedGO->GetTransform()->transformationMatrix * glm::vec4(max.x, max.y, min.z, 1);
			const Vector3 top3 = selectedGO->GetTransform()->transformationMatrix * glm::vec4(max.x, max.y, max.z, 1);

			Engine::GetRenderer().DrawLine(bottom0, bottom1, color, renderSettings);
			Engine::GetRenderer().DrawLine(bottom1, bottom3, color, renderSettings);
			Engine::GetRenderer().DrawLine(bottom2, bottom0, color, renderSettings);
			Engine::GetRenderer().DrawLine(bottom2, bottom3, color, renderSettings);

			Engine::GetRenderer().DrawLine(top0, top1, color, renderSettings);
			Engine::GetRenderer().DrawLine(top1, top3, color, renderSettings);
			Engine::GetRenderer().DrawLine(top2, top0, color, renderSettings);
			Engine::GetRenderer().DrawLine(top2, top3, color, renderSettings);

			Engine::GetRenderer().DrawLine(bottom0, top0, color, renderSettings);
			Engine::GetRenderer().DrawLine(bottom1, top1, color, renderSettings);
			Engine::GetRenderer().DrawLine(bottom2, top2, color, renderSettings);
			Engine::GetRenderer().DrawLine(bottom3, top3, color, renderSettings);
		}
	}
}

void Graphics::DrawEditorGrid(const Vector3& cameraPosition, int gridAxis)
{
	const Color color = Color::CreateFromRGBAFloat(0.7f, 0.7f, 0.7f, 0.2f);

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

	const float lineLenght = 20 * distance;
	const float lineCount = lineLenght / coef;
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
			const float zPos = z * coef;
			Engine::GetRenderer().DrawLine(Vector3(0, -lineLenght - cameraPosition.y, zPos), Vector3(0, lineLenght - cameraPosition.y, zPos), color, renderSettings);
		}
		for (int y = -lineCount + cameraPosition.y / coef; y < lineCount + cameraPosition.y / coef; y++)
		{
			const float yPos = -y * coef;
			Engine::GetRenderer().DrawLine(Vector3(0, yPos, -lineLenght + cameraPosition.z), Vector3(0, yPos, lineLenght + cameraPosition.z), color, renderSettings);
		}
	}
	else if (gridAxis == 2)
	{
		// For XY
		for (int x = -lineCount + cameraPosition.x / coef; x < lineCount + cameraPosition.x / coef; x++)
		{
			const int xPos = x * coef;
			Engine::GetRenderer().DrawLine(Vector3(xPos, -lineLenght - cameraPosition.y, 0), Vector3(xPos, lineLenght - cameraPosition.y, 0), color, renderSettings);
		}
		for (int y = -lineCount + cameraPosition.y / coef; y < lineCount + cameraPosition.y / coef; y++)
		{
			const int yPos = -y * coef;
			Engine::GetRenderer().DrawLine(Vector3(-lineLenght + cameraPosition.x, yPos, 0), Vector3(lineLenght + cameraPosition.x, yPos, 0), color, renderSettings);
		}
	}
}

void Graphics::DrawEditorTool(const Vector3& cameraPosition)
{
	std::shared_ptr< SceneMenu> sceneMenu = Editor::GetMenu<SceneMenu>();
	// Draw tool
	if (Editor::GetSelectedGameObject() && sceneMenu)
	{
		const Vector3& selectedGoPos = Editor::GetSelectedGameObject()->GetTransform()->GetPosition();
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
		const Vector3 scale = Vector3(dist);

		RenderingSettings renderSettings = RenderingSettings();
		renderSettings.invertFaces = false;
		renderSettings.useBlend = false;
		renderSettings.useDepth = false;
		renderSettings.useTexture = true;
		renderSettings.useLighting = false;

		if (sceneMenu->toolMode == ToolMode::Tool_Move || sceneMenu->toolMode == ToolMode::Tool_Scale)
		{
			MeshManager::DrawMesh(selectedGoPos, selectedGoRot, scale, Editor::toolArrowsTexture, Editor::rightArrow, renderSettings, AssetManager::unlitMaterial);
			MeshManager::DrawMesh(selectedGoPos, selectedGoRot, scale, Editor::toolArrowsTexture, Editor::upArrow, renderSettings, AssetManager::unlitMaterial);
			MeshManager::DrawMesh(selectedGoPos, selectedGoRot, scale, Editor::toolArrowsTexture, Editor::forwardArrow, renderSettings, AssetManager::unlitMaterial);
		}
		else if (sceneMenu->toolMode == ToolMode::Tool_Rotate)
		{
			MeshManager::DrawMesh(selectedGoPos, selectedGoRot, scale, Editor::toolArrowsTexture, Editor::rotationCircleX, renderSettings, AssetManager::unlitMaterial);
			MeshManager::DrawMesh(selectedGoPos, selectedGoRot, scale, Editor::toolArrowsTexture, Editor::rotationCircleY, renderSettings, AssetManager::unlitMaterial);
			MeshManager::DrawMesh(selectedGoPos, selectedGoRot, scale, Editor::toolArrowsTexture, Editor::rotationCircleZ, renderSettings, AssetManager::unlitMaterial);
		}
	}
}
#endif