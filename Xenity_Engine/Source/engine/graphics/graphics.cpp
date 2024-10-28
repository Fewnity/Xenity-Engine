// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "graphics.h"

#include <algorithm>
#if defined(_WIN32) || defined(_WIN64) || defined(__LINUX__)
#include <glad/glad.h>
#endif

#if defined(EDITOR)
#include <editor/editor.h>
#include <editor/ui/menus/scene_menu.h>
#include <editor/tool_mode.h>
#include <engine/graphics/3d_graphics/mesh_renderer.h>
#endif

#include <engine/game_elements/transform.h>
#include <engine/game_elements/gameobject.h>
#include <engine/engine.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/network/network.h>
#include <engine/tools/profiler_benchmark.h>
#include <engine/graphics/3d_graphics/lod.h>
#include <engine/graphics/render_command.h>
#include <engine/debug/debug.h>
#include <engine/tools/scope_benchmark.h>
#include <engine/debug/performance.h>
#include "iDrawable.h"
#include "renderer/renderer.h"
#include "2d_graphics/sprite_manager.h"
#include "ui/text_manager.h"
#include "3d_graphics/mesh_manager.h"
#include "3d_graphics/mesh_data.h"
#include "material.h"
#include "skybox.h"
#include "camera.h"
#include <engine/tools/math.h>
#include <engine/world_partitionner/world_partitionner.h>
#include <engine/debug/stack_debug_object.h>
#include <engine/time/time.h>

std::vector<std::weak_ptr<Camera>> Graphics::cameras;
std::shared_ptr<Camera> Graphics::usedCamera;
bool Graphics::needUpdateCamera = true;
int Graphics::s_iDrawablesCount = 0;
int Graphics::s_lodsCount = 0;

std::vector<IDrawable*> Graphics::s_orderedIDrawable;

std::vector<std::weak_ptr<Lod>> Graphics::s_lods;

bool Graphics::s_drawOrderListDirty = true;

std::shared_ptr<ProfilerBenchmark> orderBenchmark = nullptr;
std::shared_ptr<ProfilerBenchmark> skyboxBenchmark = nullptr;
std::shared_ptr<ProfilerBenchmark> drawAllBenchmark = nullptr;
std::shared_ptr<ProfilerBenchmark> drawMeshBenchmark = nullptr;
std::shared_ptr<ProfilerBenchmark> drawEndFrameBenchmark = nullptr;

std::shared_ptr <MeshData> skyPlane = nullptr;

std::shared_ptr <Shader> Graphics::s_currentShader = nullptr;
std::shared_ptr <Material> Graphics::s_currentMaterial = nullptr;
IDrawableTypes Graphics::s_currentMode = IDrawableTypes::Draw_3D;

bool Graphics::s_isRenderingBatchDirty = true;
RenderBatch renderBatch;

GraphicsSettings Graphics::s_settings;

std::vector <Light*> Graphics::s_directionalLights;
bool Graphics::s_isLightUpdateNeeded = true;
bool Graphics::s_isGridRenderingEnabled = true;
float  Graphics::s_gridAlphaMultiplier = 1;

void Graphics::SetSkybox(const std::shared_ptr<SkyBox>& skybox_)
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	s_settings.skybox = skybox_;
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
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	Engine::GetRenderer().SetFog(s_settings.isFogEnabled);
	Engine::GetRenderer().SetFogValues(s_settings.fogStart, s_settings.fogEnd, s_settings.fogColor);
}

void Graphics::Init()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	ProjectManager::GetProjectLoadedEvent().Bind(&Graphics::OnProjectLoaded);

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

void Graphics::Stop()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	ProjectManager::GetProjectLoadedEvent().Unbind(&Graphics::OnProjectLoaded);

	orderBenchmark.reset();
	skyboxBenchmark.reset();
	drawAllBenchmark.reset();
	drawMeshBenchmark.reset();
	drawEndFrameBenchmark.reset();

	cameras.clear();
	usedCamera.reset();
	s_iDrawablesCount = 0;
	s_lods.clear();
	s_lodsCount = 0;
	s_orderedIDrawable.clear();
	s_isRenderingBatchDirty = true;
	renderBatch.Reset();
	s_settings.skybox.reset();
	skyPlane.reset();
	s_currentShader.reset();
	s_currentMaterial.reset();
}

void Graphics::SetDefaultValues()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	s_settings.isFogEnabled = false;
	s_settings.fogStart = 10;
	s_settings.fogEnd = 50;
	s_settings.fogColor = Color::CreateFromRGB(152, 152, 152);
	s_settings.skyColor = Color::CreateFromRGB(25, 25, 25);
	s_settings.skybox.reset();
}

void Graphics::Draw()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	SCOPED_PROFILER("Graphics::Draw", scopeBenchmark);

	Engine::GetRenderer().NewFrame();

	usedCamera.reset();
	s_currentMaterial = nullptr;

	OrderDrawables();
	
	const int matCount = AssetManager::GetMaterialCount();
	for (const std::weak_ptr<Camera>& weakCam : cameras)
	{
		usedCamera = weakCam.lock();

		if (usedCamera->IsEnabled() && usedCamera->GetGameObjectRaw()->IsLocalActive())
		{
			SortTransparentDrawables();
			CheckLods();

			// Set material as dirty
			for (int materialIndex = 0; materialIndex < matCount; materialIndex++)
			{
				Material* mat = AssetManager::GetMaterial(materialIndex);
				mat->m_updated = false;
			}

			s_currentMode = IDrawableTypes::Draw_3D;

			needUpdateCamera = true;
			s_isLightUpdateNeeded = true;

			// Update camera and bind frame buffer
			usedCamera->UpdateProjection();
			usedCamera->UpdateFrustum();
			usedCamera->BindFrameBuffer();
			const Vector3& camPos = usedCamera->GetTransformRaw()->GetPosition();

			Engine::GetRenderer().SetClearColor(s_settings.skyColor);
			Engine::GetRenderer().Clear();

			if constexpr (s_UseOpenGLFixedFunctions)
			{
				Engine::GetRenderer().SetCameraPosition(*usedCamera);
			}

			skyboxBenchmark->Start();
			DrawSkybox(camPos);
			skyboxBenchmark->Stop();

			Engine::GetRenderer().SetFog(s_settings.isFogEnabled);

			drawAllBenchmark->Start();

			{
				SCOPED_PROFILER("Graphics::CallOnNewRender", scopeBenchmarkNewRender);
				for (IDrawable* drawable : s_orderedIDrawable)
				{
					drawable->OnNewRender();
				}
			}

			{
				SCOPED_PROFILER("Graphics::RenderOpaque", scopeBenchmarkRenderOpaque);
				for (const auto& renderQueue : renderBatch.renderQueues)
				{
					for (const RenderCommand& com : renderQueue.second.commands)
					{
						if (com.isEnabled)
							com.drawable->DrawCommand(com);
					}
				}
			}

			{
				SCOPED_PROFILER("Graphics::RenderTransparent", scopeBenchmarkRenderTransparent);
				for (const RenderCommand& com : renderBatch.transparentMeshCommands)
				{
					if (com.isEnabled)
						com.drawable->DrawCommand(com);
				}
			}

			s_currentMode = IDrawableTypes::Draw_2D;
			for (const RenderCommand& com : renderBatch.spriteCommands)
			{
				if (com.isEnabled)
					com.drawable->DrawCommand(com);
			}

			if (!usedCamera->IsEditor())
				s_currentMode = IDrawableTypes::Draw_UI;
			if constexpr (s_UseOpenGLFixedFunctions)
			{
				if (!usedCamera->IsEditor())
				{
					Engine::GetRenderer().SetCameraPosition(Vector3(0, 0, -1), Vector3(0, 0, 0));
					Engine::GetRenderer().SetProjection2D(5, 0.03f, 100);
				}
			}
			const size_t uiCommandCount = renderBatch.uiCommandIndex;
			for (size_t commandIndex = 0; commandIndex < uiCommandCount; commandIndex++)
			{
				const RenderCommand& com = renderBatch.uiCommands[commandIndex];
				if (com.isEnabled)
					com.drawable->DrawCommand(com);
			}

			drawAllBenchmark->Stop();

#if defined(EDITOR)
			if (usedCamera->IsEditor())
			{
				Engine::GetRenderer().SetFog(false);

				//Draw editor scene grid
				if (s_currentMode != IDrawableTypes::Draw_3D)
				{
					s_currentMode = IDrawableTypes::Draw_3D;
					if constexpr (s_UseOpenGLFixedFunctions)
					{
						usedCamera->UpdateProjection();
					}
				}

				Engine::GetRenderer().ResetTransform();
				Engine::GetRenderer().SetCameraPosition(*usedCamera);

				// Currently lines do not support shaders
				if constexpr (!s_UseOpenGLFixedFunctions)
				{
					Engine::GetRenderer().UseShaderProgram(0);
					s_currentShader = nullptr;
					s_currentMaterial = nullptr;
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
				const std::vector<std::shared_ptr<SceneMenu>> sceneMenus = Editor::GetMenus<SceneMenu>();
				int gridAxis = 0;
				for (const std::shared_ptr<SceneMenu>& sceneMenu : sceneMenus)
				{
					if (sceneMenu->weakCamera.lock() == usedCamera)
					{
						gridAxis = sceneMenu->gridAxis;
						break;
					}
				}

				DrawEditorGrid(camPos, gridAxis);
				DrawSelectedItemBoundingBox();

				// Draw all gizmos
				{
					SCOPED_PROFILER("Graphics::DrawGizmo", scopeBenchmarkDrawGizmo);
					for (const std::weak_ptr<Component>& weakComponent : GameplayManager::orderedComponents)
					{
						if (const std::shared_ptr<Component> component = weakComponent.lock())
						{
							if (component->GetGameObjectRaw()->IsLocalActive() && component->IsEnabled())
							{
								component->OnDrawGizmos();

								if (component->GetGameObjectRaw()->m_isSelected)
									component->OnDrawGizmosSelected();
							}
						}
					}
				}

				WorldPartitionner::OnDrawGizmos();

				DrawEditorTool(camPos);
			}
#endif
			usedCamera->CopyMultiSampledFrameBuffer();
		}
	}

#if defined(DEBUG)
	if (!usedCamera)
	{
		Debug::PrintWarning("There is no camera for rendering");
	}
#endif

	if (NetworkManager::s_needDrawMenu)
	{
		NetworkManager::DrawNetworkSetupMenu();
	}

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

Vector3 meshComparatorCamPos;
bool meshComparator2(const RenderCommand& c1, const RenderCommand& c2)
{
	return Vector3::Distance(c1.transform->GetPosition(), meshComparatorCamPos) > Vector3::Distance(c2.transform->GetPosition(), meshComparatorCamPos);
}

void Graphics::SortTransparentDrawables()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	SCOPED_PROFILER("Graphics::SortTransparentDrawables", scopeBenchmark);
	meshComparatorCamPos = usedCamera->GetTransformRaw()->GetPosition();
	std::sort(renderBatch.transparentMeshCommands.begin(), renderBatch.transparentMeshCommands.begin() + renderBatch.transparentMeshCommandIndex, meshComparator2);
}

void Graphics::OrderDrawables()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	orderBenchmark->Start();
	if (s_isRenderingBatchDirty)
	{
		SCOPED_PROFILER("Graphics::OrderDrawables", scopeBenchmark);
		s_isRenderingBatchDirty = false;
		renderBatch.Reset();
		for (IDrawable* drawable : s_orderedIDrawable)
		{
			drawable->CreateRenderCommands(renderBatch);
		}
	}

	orderBenchmark->Stop();
}

void Graphics::DeleteAllDrawables()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	s_orderedIDrawable.clear();
	s_iDrawablesCount = 0;
	s_isRenderingBatchDirty = true;
}

void Graphics::AddDrawable(IDrawable* drawableToAdd)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	XASSERT(drawableToAdd != nullptr, "[Graphics::AddDrawable] drawableToAdd is nullptr");

	s_orderedIDrawable.push_back(drawableToAdd);
	s_iDrawablesCount++;
	s_isRenderingBatchDirty = true;
	SetDrawOrderListAsDirty();
}

void Graphics::RemoveDrawable(const IDrawable* drawableToRemove)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	XASSERT(drawableToRemove != nullptr, "[Graphics::RemoveDrawable] drawableToRemove is nullptr");

	if (!Engine::IsRunning(true))
		return;

	for (int i = 0; i < s_iDrawablesCount; i++)
	{
		if (s_orderedIDrawable[i] == drawableToRemove)
		{
			s_orderedIDrawable.erase(s_orderedIDrawable.begin() + i);
			s_iDrawablesCount--;
			s_isRenderingBatchDirty = true;
			break;
		}
	}
}

void Graphics::AddLod(const std::weak_ptr<Lod>& lodToAdd)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	XASSERT(lodToAdd.lock() != nullptr, "[Graphics::AddLod] lodToAdd is nullptr");

	s_lods.push_back(lodToAdd);
	s_lodsCount++;
}

void Graphics::RemoveLod(const std::weak_ptr<Lod>& lodToRemove)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	XASSERT(lodToRemove.lock() != nullptr, "[Graphics::RemoveLod] lodToRemove is nullptr");

	if (!Engine::IsRunning(true))
		return;

	for (int i = 0; i < s_lodsCount; i++)
	{
		if (s_lods[i].lock() == lodToRemove.lock())
		{
			s_lods.erase(s_lods.begin() + i);
			s_lodsCount--;
			break;
		}
	}
}

void Graphics::RemoveCamera(const std::weak_ptr<Camera>& cameraToRemove)
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	XASSERT(cameraToRemove.lock() != nullptr, "[Graphics::RemoveCamera] cameraToRemove is nullptr");

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

void Graphics::DrawSubMesh(const MeshData::SubMesh& subMesh, Material& material, RenderingSettings& renderSettings, const glm::mat4& matrix, bool forUI)
{
	DrawSubMesh(subMesh, material, material.m_texture.get(), renderSettings, matrix, forUI);
}

void Graphics::DrawSubMesh(const MeshData::SubMesh& subMesh, Material& material, Texture* texture, RenderingSettings& renderSettings, const glm::mat4& matrix, bool forUI)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	SCOPED_PROFILER("Graphics::DrawSubMesh", scopeBenchmark);

	XASSERT(usedCamera != nullptr, "[Graphics::DrawSubMesh] usedCamera is nullptr");

	if (texture == nullptr)
		texture = AssetManager::defaultTexture.get();

	drawMeshBenchmark->Start();

	if constexpr (!s_UseOpenGLFixedFunctions)
	{
		material.Use();

		if (!s_currentShader)
			return;

		s_currentShader->SetShaderModel(matrix);
	}
	else
	{
#if defined(__vita__) || defined(_WIN32) || defined(_WIN64) || defined(__LINUX__) // The PSP does not need to set the camera position every draw call
		if (!forUI || usedCamera->IsEditor())
			Engine::GetRenderer().SetCameraPosition(*usedCamera);
#endif
		Engine::GetRenderer().SetTransform(matrix);
	}

	Engine::GetRenderer().DrawSubMesh(subMesh, material, *texture, renderSettings);
	drawMeshBenchmark->Stop();
}

void Graphics::SetDrawOrderListAsDirty()
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	s_drawOrderListDirty = true;
}

void Graphics::CreateLightLists()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	s_directionalLights.clear();
	const std::vector<Light*>& lights = AssetManager::GetLights();
	for (Light* light : lights)
	{
		if (light->GetType() == LightType::Directional && light->IsEnabled() && light->GetGameObjectRaw()->IsLocalActive())
		{
			s_directionalLights.push_back(light);
		}
	}
}

void Graphics::SetIsGridRenderingEnabled(bool enabled)
{
	s_isGridRenderingEnabled = enabled;
}

bool Graphics::IsGridRenderingEnabled()
{
	return s_isGridRenderingEnabled;
}

void Graphics::OnProjectLoaded()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	skyPlane = AssetManager::LoadEngineAsset<MeshData>("public_engine_assets/models/PlaneTriangulate.obj");
	XASSERT(skyPlane != nullptr, "[Graphics::OnProjectLoaded] skyPlane is null");
	skyPlane->LoadFileReference();
}

void Graphics::DrawSkybox(const Vector3& cameraPosition)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	SCOPED_PROFILER("Graphics::DrawSkybox", scopeBenchmark);
	if (s_settings.skybox)
	{
		Engine::GetRenderer().SetFog(false);
		const float scaleF = 10.01f;
		const Vector3 scale = Vector3(scaleF);

		RenderingSettings renderSettings = RenderingSettings();
		renderSettings.invertFaces = false;
		renderSettings.renderingMode = MaterialRenderingModes::Opaque;
		renderSettings.useDepth = false;
		renderSettings.useTexture = true;
		renderSettings.useLighting = false;

		const std::shared_ptr<Texture>& texture = AssetManager::unlitMaterial->m_texture;

		AssetManager::unlitMaterial->m_texture = s_settings.skybox->down;
		static const Quaternion q0 = Quaternion::Euler(0, 180, 0);
		Graphics::DrawSubMesh(Vector3(0, -5, 0) + cameraPosition, q0, scale, *skyPlane->m_subMeshes[0], *AssetManager::unlitMaterial, renderSettings);

		AssetManager::unlitMaterial->m_texture = s_settings.skybox->up;
		static const Quaternion q1 = Quaternion::Euler(180, 180, 0);
		Graphics::DrawSubMesh(Vector3(0, 5, 0) + cameraPosition, q1, scale, *skyPlane->m_subMeshes[0], *AssetManager::unlitMaterial, renderSettings);

		AssetManager::unlitMaterial->m_texture = s_settings.skybox->front;
		static const Quaternion q2 = Quaternion::Euler(90, 0, 180);
		Graphics::DrawSubMesh(Vector3(0, 0, 5) + cameraPosition, q2, scale, *skyPlane->m_subMeshes[0], *AssetManager::unlitMaterial, renderSettings);

		AssetManager::unlitMaterial->m_texture = s_settings.skybox->back;
		static const Quaternion q3 = Quaternion::Euler(90, 0, 0);
		Graphics::DrawSubMesh(Vector3(0, 0, -5) + cameraPosition, q3, scale, *skyPlane->m_subMeshes[0], *AssetManager::unlitMaterial, renderSettings);

		AssetManager::unlitMaterial->m_texture = s_settings.skybox->left;
		static const Quaternion q4 = Quaternion::Euler(90, -90, 0);
		Graphics::DrawSubMesh(Vector3(5, 0, 0) + cameraPosition, q4, scale, *skyPlane->m_subMeshes[0], *AssetManager::unlitMaterial, renderSettings);

		AssetManager::unlitMaterial->m_texture = s_settings.skybox->right;
		static const Quaternion q5 = Quaternion::Euler(90, 0, -90);
		Graphics::DrawSubMesh(Vector3(-5, 0, 0) + cameraPosition, q5, scale, *skyPlane->m_subMeshes[0], *AssetManager::unlitMaterial, renderSettings);

		AssetManager::unlitMaterial->m_texture = texture;
	}
}

void Graphics::CheckLods()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	SCOPED_PROFILER("Graphics::CheckLods", scopeBenchmark);
	for (int i = 0; i < s_lodsCount; i++)
	{
		const std::shared_ptr<Lod> lod = s_lods[i].lock();
		if (lod)
		{
			lod->CheckLod();
		}
	}
}

void Graphics::DrawSubMesh(const Vector3& position, const Quaternion& rotation, const Vector3& scale, const MeshData::SubMesh& subMesh, Material& material, RenderingSettings& renderSettings)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	const glm::mat4 matrix = Math::CreateModelMatrix(position, rotation, scale);
	Graphics::DrawSubMesh(subMesh, material, renderSettings, matrix, false);
}

#if defined(EDITOR)

void Graphics::DrawSelectedItemBoundingBox()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	SCOPED_PROFILER("Graphics::DrawSelectedItemBoundingBox", scopeBenchmark);
	const std::vector<std::weak_ptr<GameObject>>& selectedGameObjects = Editor::GetSelectedGameObjects();
	for (const std::weak_ptr<GameObject>& selectedGOWeak : selectedGameObjects)
	{
		const std::shared_ptr<GameObject> selectedGO = selectedGOWeak.lock();
		if (!selectedGO)
			continue;

		const std::shared_ptr<MeshRenderer> meshRenderer = selectedGO->GetComponent<MeshRenderer>();
		if (meshRenderer && meshRenderer->GetMeshData() && selectedGO->IsLocalActive() && meshRenderer->IsEnabled())
		{
			const Color color = Color::CreateFromRGBAFloat(0.0f, 1.0f, 1.0f, 1.0f);

			RenderingSettings renderSettings = RenderingSettings();
			renderSettings.renderingMode = MaterialRenderingModes::Transparent;
			renderSettings.useDepth = true;
			renderSettings.useLighting = false;
			renderSettings.useTexture = false;

			const Vector3& min = meshRenderer->GetMeshData()->GetMinBoundingBox();
			const Vector3& max = meshRenderer->GetMeshData()->GetMaxBoundingBox();

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
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	SCOPED_PROFILER("Graphics::DrawEditorGrid", scopeBenchmark);

	if (s_isGridRenderingEnabled && s_gridAlphaMultiplier < 1)
	{
		s_gridAlphaMultiplier += Time::GetDeltaTime() * 7.0f;
		s_gridAlphaMultiplier = std::clamp(s_gridAlphaMultiplier, 0.0f, 1.0f);
	}
	else if (!s_isGridRenderingEnabled && s_gridAlphaMultiplier > 0)
	{
		s_gridAlphaMultiplier -= Time::GetDeltaTime() * 7.0f;
		s_gridAlphaMultiplier = std::clamp(s_gridAlphaMultiplier, 0.0f, 1.0f);
	}

	if (s_gridAlphaMultiplier == 0)
	{
		return;
	}

	float distance = 0;
	if (gridAxis == 0)
	{
		distance = fabs(cameraPosition.y);
	}
	else if (gridAxis == 1)
	{
		distance = fabs(cameraPosition.x);
	}
	else //if (gridAxis == 2)
	{
		distance = fabs(cameraPosition.z);
	}

	if (distance < 0.7f)
	{
		distance = 0.7f;
	}

	// Get the coef for grid lineCount by using the camera distance
	int coef = 1;
	while (coef < distance / 10.0f)
	{
		coef *= 10;
	}

	const float lineLenght = 20 * distance;
	const float lineCount = lineLenght / coef;
	const Color color = Color::CreateFromRGBAFloat(0.7f, 0.7f, 0.7f, 0.2f * s_gridAlphaMultiplier);

	RenderingSettings renderSettings = RenderingSettings();
	renderSettings.renderingMode = MaterialRenderingModes::Transparent;
	renderSettings.useDepth = true;
	renderSettings.useLighting = false;
	renderSettings.useTexture = false;

	if (gridAxis == 0)
	{
		// For XZ
		for (int z = static_cast<int>(-lineCount + cameraPosition.z / coef); z < lineCount + cameraPosition.z / coef; z++)
		{
			const float zPos = static_cast<float>(z * coef);
			Engine::GetRenderer().DrawLine(Vector3(-lineLenght - cameraPosition.x, 0, zPos), Vector3(lineLenght - cameraPosition.x, 0, zPos), color, renderSettings);
		}
		for (int x = static_cast<int>(-lineCount + cameraPosition.x / coef); x < lineCount + cameraPosition.x / coef; x++)
		{
			const float xPos = static_cast<float>(-x * coef);
			Engine::GetRenderer().DrawLine(Vector3(xPos, 0, -lineLenght + cameraPosition.z), Vector3(xPos, 0, lineLenght + cameraPosition.z), color, renderSettings);
		}
	}
	else if (gridAxis == 1)
	{
		//For YZ
		for (int z = static_cast<int>(-lineCount + cameraPosition.z / coef); z < lineCount + cameraPosition.z / coef; z++)
		{
			const float zPos = static_cast<float>(z * coef);
			Engine::GetRenderer().DrawLine(Vector3(0, -lineLenght - cameraPosition.y, zPos), Vector3(0, lineLenght - cameraPosition.y, zPos), color, renderSettings);
		}
		for (int y = static_cast<int>(-lineCount + cameraPosition.y / coef); y < lineCount + cameraPosition.y / coef; y++)
		{
			const float yPos = static_cast<float>(-y * coef);
			Engine::GetRenderer().DrawLine(Vector3(0, yPos, -lineLenght + cameraPosition.z), Vector3(0, yPos, lineLenght + cameraPosition.z), color, renderSettings);
		}
	}
	else if (gridAxis == 2)
	{
		// For XY
		for (int x = static_cast<int>(-lineCount + cameraPosition.x / coef); x < lineCount + cameraPosition.x / coef; x++)
		{
			const float xPos = static_cast<float>(x * coef);
			Engine::GetRenderer().DrawLine(Vector3(xPos, -lineLenght - cameraPosition.y, 0), Vector3(xPos, lineLenght - cameraPosition.y, 0), color, renderSettings);
		}
		for (int y = static_cast<int>(-lineCount + cameraPosition.y / coef); y < lineCount + cameraPosition.y / coef; y++)
		{
			const float yPos = static_cast<float>(-y * coef);
			Engine::GetRenderer().DrawLine(Vector3(-lineLenght + cameraPosition.x, yPos, 0), Vector3(lineLenght + cameraPosition.x, yPos, 0), color, renderSettings);
		}
	}
}

void Graphics::DrawEditorTool(const Vector3& cameraPosition)
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);

	SCOPED_PROFILER("Graphics::DrawEditorTool", scopeBenchmark);

	const std::shared_ptr< SceneMenu> sceneMenu = Editor::GetMenu<SceneMenu>();
	// Draw tool
	if (Editor::GetSelectedGameObjects().size() == 1 && sceneMenu)
	{
		const std::shared_ptr<GameObject> selectedGo = Editor::GetSelectedGameObjects()[0].lock();
		if (!selectedGo)
			return;

		const Vector3& selectedGoPos = selectedGo->GetTransform()->GetPosition();

		Quaternion selectedGoRot = selectedGo->GetTransform()->GetRotation();
		if (Editor::isToolLocalMode)
			selectedGoRot = Quaternion::Identity();

		float dist = 1;
		if (usedCamera->GetProjectionType() == ProjectionTypes::Perspective)
			dist = Vector3::Distance(selectedGoPos, cameraPosition);
		else
			dist = usedCamera->GetProjectionSize() * 1.5f;

		dist /= 40;
		const Vector3 scale = Vector3(dist);

		RenderingSettings renderSettings = RenderingSettings();
		renderSettings.invertFaces = false;
		renderSettings.renderingMode = MaterialRenderingModes::Opaque;
		renderSettings.useDepth = false;
		renderSettings.useTexture = true;
		renderSettings.useLighting = false;

		AssetManager::unlitMaterial->m_texture = Editor::toolArrowsTexture;
		if (sceneMenu->toolMode == ToolMode::Tool_Move || sceneMenu->toolMode == ToolMode::Tool_Scale)
		{
			Graphics::DrawSubMesh(selectedGoPos, selectedGoRot, scale, *Editor::rightArrow->m_subMeshes[0], *AssetManager::unlitMaterial, renderSettings);
			Graphics::DrawSubMesh(selectedGoPos, selectedGoRot, scale, *Editor::upArrow->m_subMeshes[0], *AssetManager::unlitMaterial, renderSettings);
			Graphics::DrawSubMesh(selectedGoPos, selectedGoRot, scale, *Editor::forwardArrow->m_subMeshes[0], *AssetManager::unlitMaterial, renderSettings);
		}
		else if (sceneMenu->toolMode == ToolMode::Tool_Rotate)
		{
			Graphics::DrawSubMesh(selectedGoPos, selectedGoRot, scale, *Editor::rotationCircleX->m_subMeshes[0], *AssetManager::unlitMaterial, renderSettings);
			Graphics::DrawSubMesh(selectedGoPos, selectedGoRot, scale, *Editor::rotationCircleY->m_subMeshes[0], *AssetManager::unlitMaterial, renderSettings);
			Graphics::DrawSubMesh(selectedGoPos, selectedGoRot, scale, *Editor::rotationCircleZ->m_subMeshes[0], *AssetManager::unlitMaterial, renderSettings);
		}
		AssetManager::unlitMaterial->m_texture = nullptr;
	}
}
#endif // #if defined(EDITOR)