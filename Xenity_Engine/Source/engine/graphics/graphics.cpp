#include "graphics.h"

#include "iDrawable.h"

#include "renderer/renderer.h"
#include "renderer/renderer_opengl.h"

#include "2d_graphics/sprite_manager.h"
#include "ui/text_manager.h"

#include "3d_graphics/mesh_manager.h"
#include "3d_graphics/mesh_data.h"
#include <engine/graphics/3d_graphics/lod.h>
#include <engine/graphics/render_command.h>

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
std::shared_ptr<Camera> Graphics::usedCamera;
bool Graphics::needUpdateCamera = true;
int Graphics::iDrawablesCount = 0;
int Graphics::lodsCount = 0;

std::vector<std::weak_ptr<IDrawable>> Graphics::orderedIDrawable;

std::vector<std::weak_ptr<IDrawable>> Graphics::noTransparentDrawable;
std::vector<std::weak_ptr<IDrawable>> Graphics::transparentDrawable;
std::vector<std::weak_ptr<IDrawable>> Graphics::spriteDrawable;
std::vector<std::weak_ptr<IDrawable>> Graphics::uiDrawable;
std::vector<std::weak_ptr<Lod>> Graphics::lods;

bool Graphics::drawOrderListDirty = true;

std::shared_ptr<ProfilerBenchmark> orderBenchmark = nullptr;
std::shared_ptr<ProfilerBenchmark> skyboxBenchmark = nullptr;
std::shared_ptr<ProfilerBenchmark> drawAllBenchmark = nullptr;
std::shared_ptr<ProfilerBenchmark> drawMeshBenchmark = nullptr;
std::shared_ptr<ProfilerBenchmark> drawEndFrameBenchmark = nullptr;

std::shared_ptr <MeshData> skyPlane = nullptr;

std::shared_ptr <Shader> Graphics::currentShader = nullptr;
std::shared_ptr <Material> Graphics::currentMaterial = nullptr;
IDrawableTypes Graphics::currentMode = IDrawableTypes::Draw_3D;

bool Graphics::UseOpenGLFixedFunctions = false;
bool Graphics::isRenderingBatchDirty = true;
RenderBatch renderBatch;

GraphicsSettings Graphics::settings;

void Graphics::SetSkybox(const std::shared_ptr<SkyBox>& skybox_)
{
	settings.skybox = skybox_;
}

ReflectiveData GraphicsSettings::GetReflectiveData()
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
	Engine::GetRenderer().SetFog(settings.isFogEnabled);
	Engine::GetRenderer().SetFogValues(settings.fogStart, settings.fogEnd, settings.fogColor);
}

void Graphics::Init()
{
#if defined(__PSP__) || defined(_EE)
	UseOpenGLFixedFunctions = true;
#endif

	skyPlane = MeshManager::LoadMesh("public_engine_assets\\models\\PlaneTriangulate.obj");

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
	settings.isFogEnabled = false;
	settings.fogStart = 10;
	settings.fogEnd = 50;
	settings.fogColor = Color::CreateFromRGB(152, 152, 152);
	settings.skyColor = Color::CreateFromRGB(25, 25, 25);
	settings.skybox.reset();
}

void Graphics::Draw()
{
	/*auto camera = usedCamera.lock();
	if (!camera)
	{
		Debug::PrintWarning("[Graphics::DrawAllDrawable] There is no camera for rendering");
	}*/

	Engine::GetRenderer().NewFrame();
	usedCamera.reset();
	currentMaterial = nullptr;

	const size_t cameraCount = cameras.size();
	const int matCount = AssetManager::GetMaterialCount();

	OrderDrawables();
	for (size_t cameraIndex = 0; cameraIndex < cameraCount; cameraIndex++)
	{
		usedCamera = cameras[cameraIndex].lock();
		if (usedCamera->GetIsEnabled() && usedCamera->GetGameObject()->GetLocalActive())
		{
			SortTransparentDrawables();
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
			usedCamera->UpdateProjection();
			usedCamera->BindFrameBuffer();
			const Vector3& camPos = usedCamera->GetTransform()->GetPosition();

			Engine::GetRenderer().SetClearColor(settings.skyColor);
			Engine::GetRenderer().Clear();

			if (UseOpenGLFixedFunctions)
			{
				Engine::GetRenderer().SetCameraPosition(usedCamera);
				Engine::GetRenderer().Setlights(usedCamera);
			}

			skyboxBenchmark->Start();
			DrawSkybox(camPos);
			skyboxBenchmark->Stop();

			Engine::GetRenderer().SetFog(settings.isFogEnabled);

			drawAllBenchmark->Start();

			for (const auto& renderQueue : renderBatch.renderQueues)
			{
				const int commandCount = renderQueue.second.commandIndex;
				for (int commandIndex = 0; commandIndex < commandCount; commandIndex++)
				{
					const RenderCommand& com = renderQueue.second.commands[commandIndex];
					if (com.isEnabled)
						com.drawable->DrawCommand(com);
				}
			}

			const int transparentCommandCount = renderBatch.transparentMeshCommandIndex;
			for (int commandIndex = 0; commandIndex < transparentCommandCount; commandIndex++)
			{
				const RenderCommand& com = renderBatch.transparentMeshCommands[commandIndex];
				if (com.isEnabled)
					com.drawable->DrawCommand(com);
			}
			currentMode = IDrawableTypes::Draw_2D;
			const int spriteCommandCount = renderBatch.spriteCommandIndex;
			for (int commandIndex = 0; commandIndex < spriteCommandCount; commandIndex++)
			{
				const RenderCommand& com = renderBatch.spriteCommands[commandIndex];
				if (com.isEnabled)
					com.drawable->DrawCommand(com);
			}

			if (!usedCamera->GetIsEditor())
				currentMode = IDrawableTypes::Draw_UI;
			if (UseOpenGLFixedFunctions && !usedCamera->GetIsEditor())
			{
				Engine::GetRenderer().SetCameraPosition(Vector3(0,0,-1), Vector3(0,0,0));
				Engine::GetRenderer().SetProjection2D(5, 0.03f, 100);
			}
			const int uiCommandCount = renderBatch.uiCommandIndex;
			for (int commandIndex = 0; commandIndex < uiCommandCount; commandIndex++)
			{
				const RenderCommand& com = renderBatch.uiCommands[commandIndex];
				if (com.isEnabled)
					com.drawable->DrawCommand(com);
			}

			drawAllBenchmark->Stop();

#if defined(EDITOR)
			if (usedCamera->GetIsEditor())
			{
				Engine::GetRenderer().SetFog(false);

				//Draw editor scene grid
				if (currentMode != IDrawableTypes::Draw_3D)
				{
					currentMode = IDrawableTypes::Draw_3D;
					if (UseOpenGLFixedFunctions)
					{
						usedCamera->UpdateProjection();
					}
				}

				Engine::GetRenderer().ResetTransform();
				Engine::GetRenderer().SetCameraPosition(usedCamera);

				// Currently lines do not support shaders
				if (!UseOpenGLFixedFunctions)
				{
					Engine::GetRenderer().UseShaderProgram(0);
					currentShader = nullptr;
					currentMaterial = nullptr;
				}

				if (usedCamera->GetProjectionType() == ProjectionTypes::Perspective)
				{
					Engine::GetRenderer().SetProjection3D(usedCamera->GetFov(), usedCamera->GetNearClippingPlane(), usedCamera->GetFarClippingPlane(), usedCamera->GetAspectRatio());
				}
				else
				{
					Engine::GetRenderer().SetProjection2D(usedCamera->GetProjectionSize(), usedCamera->GetNearClippingPlane(), usedCamera->GetFarClippingPlane());
				}

				// Get the grid axis
				std::vector<std::shared_ptr<SceneMenu>> sceneMenus;
				sceneMenus = Editor::GetMenus<SceneMenu>();
				const int sceneMenuCount = sceneMenus.size();
				int gridAxis = 0;
				for (int i = 0; i < sceneMenuCount; i++)
				{
					if (sceneMenus[i]->weakCamera.lock() == usedCamera)
					{
						gridAxis = sceneMenus[i]->gridAxis;
						break;
					}
				}

				DrawEditorGrid(camPos, gridAxis);
				DrawSelectedItemBoundingBox(camPos);

				// Draw all gizmos
				for (int i = 0; i < GameplayManager::componentsCount; i++)
				{
					if (std::shared_ptr<Component> component = GameplayManager::orderedComponents[i].lock())
					{
						if (component->GetGameObject()->GetLocalActive() && component->GetIsEnabled())
						{
							component->OnDrawGizmos();

							if (component->GetGameObject()->isSelected)
								component->OnDrawGizmosSelected();
						}
					}
				}

				DrawEditorTool(camPos);
			}
#endif
			usedCamera->CopyMultiSampledFrameBuffer();
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

//bool spriteComparator(const std::weak_ptr<IDrawable>& t1, const std::weak_ptr<IDrawable>& t2)
//{
//	if (t1.lock()->type == IDrawableTypes::Draw_3D && t2.lock()->type == IDrawableTypes::Draw_3D)
//	{
//		return false;
//	}
//	if (t2.lock()->type == IDrawableTypes::Draw_2D && t1.lock()->type == IDrawableTypes::Draw_3D)
//	{
//		return true;
//	}
//	if (t1.lock()->type == IDrawableTypes::Draw_2D && t2.lock()->type == IDrawableTypes::Draw_3D)
//	{
//		return false;
//	}
//	if (t2.lock()->type == IDrawableTypes::Draw_UI && t1.lock()->type != IDrawableTypes::Draw_UI)
//	{
//		return true;
//	}
//
//	const int priority1 = t1.lock()->GetDrawPriority();
//	const int priority2 = t2.lock()->GetDrawPriority();
//
//	if (priority1 <= priority2)
//	{
//		if (priority1 == priority2)
//		{
//			return t1.lock()->GetTransform()->GetPosition().z > t2.lock()->GetTransform()->GetPosition().z;
//		}
//
//		return true;
//	}
//
//	return false;
//}
//
//bool meshComparator(const std::weak_ptr<IDrawable>& t1, const std::weak_ptr<IDrawable>& t2)
//{
//	const Vector3& pos1 = t1.lock()->GetTransform()->GetPosition();
//	const Vector3& pos2 = t2.lock()->GetTransform()->GetPosition();
//	const Vector3& camPos = Graphics::usedCamera.lock()->GetTransform()->GetPosition();
//	const float dis1 = Vector3::Distance(pos1, camPos);
//	const float dis2 = Vector3::Distance(pos2, camPos);
//
//	return dis1 > dis2;
//}

bool meshComparator2(const RenderCommand& c1, const RenderCommand& c2)
{
	/*if (!c1.transform)
		return false;
	if (!c2.transform)
		return false;*/
	const Vector3& pos1 = c1.transform->GetPosition();
	const Vector3& pos2 = c2.transform->GetPosition();
	const Vector3& camPos = Graphics::usedCamera->GetTransform()->GetPosition();
	const float dis1 = Vector3::Distance(pos1, camPos);
	const float dis2 = Vector3::Distance(pos2, camPos);

	return dis1 > dis2;
}

void Graphics::SortTransparentDrawables()
{
	std::sort(renderBatch.transparentMeshCommands.begin(), renderBatch.transparentMeshCommands.begin() + renderBatch.transparentMeshCommandIndex, meshComparator2);
}

void Graphics::OrderDrawables()
{
	orderBenchmark->Start();
	if (isRenderingBatchDirty)
	{
		isRenderingBatchDirty = false;
		renderBatch.Reset();
		for (int iDrawIndex = 0; iDrawIndex < iDrawablesCount; iDrawIndex++)
		{
			const std::shared_ptr<IDrawable> drawableToCheck = orderedIDrawable[iDrawIndex].lock();
			drawableToCheck->CreateRenderCommands(renderBatch);
		}
	}

	orderBenchmark->Stop();
}

void Graphics::DeleteAllDrawables()
{
	orderedIDrawable.clear();
	iDrawablesCount = 0;
	isRenderingBatchDirty = true;
}

void Graphics::AddDrawable(const std::weak_ptr<IDrawable>& drawableToAdd)
{
	DXASSERT(drawableToAdd.lock() != nullptr, "[Graphics::AddDrawable] drawableToAdd is nullptr")

	orderedIDrawable.push_back(drawableToAdd);
	iDrawablesCount++;
	isRenderingBatchDirty = true;
	SetDrawOrderListAsDirty();
}

void Graphics::RemoveDrawable(const std::weak_ptr<IDrawable>& drawableToRemove)
{
	DXASSERT(drawableToRemove.lock() != nullptr, "[Graphics::RemoveDrawable] drawableToRemove is nullptr")

	if (!Engine::IsRunning(true))
		return;

	for (int i = 0; i < iDrawablesCount; i++)
	{
		if (orderedIDrawable[i].lock() == drawableToRemove.lock())
		{
			orderedIDrawable.erase(orderedIDrawable.begin() + i);
			iDrawablesCount--;
			isRenderingBatchDirty = true;
			break;
		}
	}
}

void Graphics::AddLod(const std::weak_ptr<Lod>& lodToAdd)
{
	DXASSERT(lodToAdd.lock() != nullptr, "[Graphics::AddLod] lodToAdd is nullptr")

	lods.push_back(lodToAdd);
	lodsCount++;
}

void Graphics::RemoveLod(const std::weak_ptr<Lod>& lodToRemove)
{
	DXASSERT(lodToRemove.lock() != nullptr, "[Graphics::RemoveLod] lodToRemove is nullptr")

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
	DXASSERT(cameraToRemove.lock() != nullptr, "[Graphics::RemoveCamera] cameraToRemove is nullptr")

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

void Graphics::DrawSubMesh(const MeshData::SubMesh& subMesh, const std::shared_ptr<Material>& material, RenderingSettings& renderSettings, const glm::mat4& matrix, bool forUI)
{
	DrawSubMesh(subMesh, material, material->texture, renderSettings, matrix, forUI);
}

void Graphics::DrawSubMesh(const MeshData::SubMesh& subMesh, const std::shared_ptr<Material>& material, std::shared_ptr<Texture> texture, RenderingSettings& renderSettings, const glm::mat4& matrix, bool forUI)
{
	DXASSERT(material != nullptr, "[Graphics::DrawSubMesh] material is nullptr")
	DXASSERT(usedCamera != nullptr, "[Graphics::DrawSubMesh] usedCamera is nullptr")

	if (texture == nullptr)
		texture = AssetManager::defaultTexture;

	drawMeshBenchmark->Start();

	if (!UseOpenGLFixedFunctions)
	{
		material->Use();

		if (!currentShader)
			return;

		currentShader->SetShaderModel(matrix);
	}
	else
	{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64) // The PSP does not need to set the camera position every draw call
		if (!forUI || usedCamera->GetIsEditor())
			Engine::GetRenderer().SetCameraPosition(usedCamera);
#endif
		Engine::GetRenderer().SetTransform(matrix);
	}

	Engine::GetRenderer().DrawSubMesh(subMesh, material, texture, renderSettings);
	drawMeshBenchmark->Stop();
}

void Graphics::SetDrawOrderListAsDirty()
{
	drawOrderListDirty = true;
}

void Graphics::DrawSkybox(const Vector3& cameraPosition)
{
	if (settings.skybox)
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

		AssetManager::unlitMaterial->texture = settings.skybox->down;
		MeshManager::DrawMesh(Vector3(0, -5, 0) + cameraPosition, Vector3(0, 180, 0), scale, *skyPlane->subMeshes[0], AssetManager::unlitMaterial, renderSettings);
		AssetManager::unlitMaterial->texture = settings.skybox->up;
		MeshManager::DrawMesh(Vector3(0, 5, 0) + cameraPosition, Vector3(180, 180, 0), scale, *skyPlane->subMeshes[0], AssetManager::unlitMaterial, renderSettings);
		AssetManager::unlitMaterial->texture = settings.skybox->front;
		MeshManager::DrawMesh(Vector3(0, 0, 5) + cameraPosition, Vector3(90, 0, 180), scale, *skyPlane->subMeshes[0], AssetManager::unlitMaterial, renderSettings);
		AssetManager::unlitMaterial->texture = settings.skybox->back;
		MeshManager::DrawMesh(Vector3(0, 0, -5) + cameraPosition, Vector3(90, 0, 0), scale, *skyPlane->subMeshes[0], AssetManager::unlitMaterial, renderSettings);
		AssetManager::unlitMaterial->texture = settings.skybox->left;
		MeshManager::DrawMesh(Vector3(5, 0, 0) + cameraPosition, Vector3(90, -90, 0), scale, *skyPlane->subMeshes[0], AssetManager::unlitMaterial, renderSettings);
		AssetManager::unlitMaterial->texture = settings.skybox->right;
		MeshManager::DrawMesh(Vector3(-5, 0, 0) + cameraPosition, Vector3(90, 0, -90), scale, *skyPlane->subMeshes[0], AssetManager::unlitMaterial, renderSettings);
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
	const std::vector<std::weak_ptr<GameObject>>& selectedGameObjects = Editor::GetSelectedGameObjects();
	for (const std::weak_ptr<GameObject>& selectedGOWeak : selectedGameObjects)
	{
		std::shared_ptr<GameObject> selectedGO = selectedGOWeak.lock();
		if (!selectedGO)
			continue;

		const std::shared_ptr<MeshRenderer> meshRenderer = selectedGO->GetComponent<MeshRenderer>();
		if (meshRenderer && meshRenderer->GetMeshData() && selectedGO->GetLocalActive() && meshRenderer->GetIsEnabled())
		{
			const Color color = Color::CreateFromRGBAFloat(0.0f, 1.0f, 1.0f, 1.0f);

			RenderingSettings renderSettings = RenderingSettings();
			renderSettings.useBlend = true;
			renderSettings.useDepth = true;
			renderSettings.useLighting = false;
			renderSettings.useTexture = false;

			const Vector3 min = meshRenderer->GetMeshData()->minBoundingBox;
			const Vector3 max = meshRenderer->GetMeshData()->maxBoundingBox;

			const glm::mat4x4& matrix = selectedGO->GetTransform()->GetTransformationMatrix();
			const Vector3 bottom0 = matrix * glm::vec4(min.x, min.y, min.z, 1);
			const Vector3 bottom1 = matrix * glm::vec4(min.x, min.y, max.z, 1);
			const Vector3 bottom2 = matrix * glm::vec4(max.x, min.y, min.z, 1);
			const Vector3 bottom3 = matrix * glm::vec4(max.x, min.y, max.z, 1);

			const Vector3 top0 = matrix * glm::vec4(min.x, max.y, min.z, 1);
			const Vector3 top1 = matrix * glm::vec4(min.x, max.y, max.z, 1);
			const Vector3 top2 = matrix * glm::vec4(max.x, max.y, min.z, 1);
			const Vector3 top3 = matrix * glm::vec4(max.x, max.y, max.z, 1);

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
	float distance;
	if (gridAxis == 0)
		distance = abs(cameraPosition.y);
	else if (gridAxis == 1)
		distance = abs(cameraPosition.x);
	else //if (gridAxis == 2)
		distance = abs(cameraPosition.z);

	if (distance < 0.7f)
		distance = 0.7f;

	// Get the coef for grid lineCount by using the camera distance
	int coef = 1;
	while (coef < distance / 10)
	{
		coef *= 10;
	}

	const float lineLenght = 20 * distance;
	const float lineCount = lineLenght / coef;
	const Color color = Color::CreateFromRGBAFloat(0.7f, 0.7f, 0.7f, 0.2f);

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
	if (Editor::GetSelectedGameObjects().size() == 1 && sceneMenu)
	{
		std::shared_ptr<GameObject>selectedGo = Editor::GetSelectedGameObjects()[0].lock();
		if (!selectedGo)
			return;

		const Vector3& selectedGoPos = selectedGo->GetTransform()->GetPosition();

		Vector3 selectedGoRot = selectedGo->GetTransform()->GetRotation();
		if (Editor::isToolLocalMode)
			selectedGoRot = Vector3(0);

		float dist = 1;
		if (usedCamera->GetProjectionType() == ProjectionTypes::Perspective)
			dist = Vector3::Distance(selectedGoPos, cameraPosition);
		else
			dist = usedCamera->GetProjectionSize() * 1.5f;

		dist /= 40;
		const Vector3 scale = Vector3(dist);

		RenderingSettings renderSettings = RenderingSettings();
		renderSettings.invertFaces = false;
		renderSettings.useBlend = false;
		renderSettings.useDepth = false;
		renderSettings.useTexture = true;
		renderSettings.useLighting = false;

		AssetManager::unlitMaterial->texture = Editor::toolArrowsTexture;
		if (sceneMenu->toolMode == ToolMode::Tool_Move || sceneMenu->toolMode == ToolMode::Tool_Scale)
		{
			MeshManager::DrawMesh(selectedGoPos, selectedGoRot, scale, *Editor::rightArrow->subMeshes[0], AssetManager::unlitMaterial, renderSettings);
			MeshManager::DrawMesh(selectedGoPos, selectedGoRot, scale, *Editor::upArrow->subMeshes[0], AssetManager::unlitMaterial, renderSettings);
			MeshManager::DrawMesh(selectedGoPos, selectedGoRot, scale, *Editor::forwardArrow->subMeshes[0], AssetManager::unlitMaterial, renderSettings);
		}
		else if (sceneMenu->toolMode == ToolMode::Tool_Rotate)
		{
			MeshManager::DrawMesh(selectedGoPos, selectedGoRot, scale, *Editor::rotationCircleX->subMeshes[0], AssetManager::unlitMaterial, renderSettings);
			MeshManager::DrawMesh(selectedGoPos, selectedGoRot, scale, *Editor::rotationCircleY->subMeshes[0], AssetManager::unlitMaterial, renderSettings);
			MeshManager::DrawMesh(selectedGoPos, selectedGoRot, scale, *Editor::rotationCircleZ->subMeshes[0], AssetManager::unlitMaterial, renderSettings);
		}
		AssetManager::unlitMaterial->texture = nullptr;
	}
}
#endif