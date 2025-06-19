// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */
#include <memory>
#include <vector>

#include <engine/api.h>
#include "iDrawableTypes.h"
#include "renderer/renderer.h" // For RenderingSettings

class IDrawable;
class Material;
class Camera;
class Texture;
class SkyBox;
class Vector3;
class Shader;
class Lod;
class Quaternion;

class GraphicsSettings : public Reflective
{
public:
	ReflectiveData GetReflectiveData() override;

	std::shared_ptr <SkyBox> skybox;

	bool isFogEnabled = false;
	float fogStart = 0;
	float fogEnd = 10;
	Color fogColor;
	Color skyColor;
};

class Graphics
{
public:

	/**
	* @brief [Internal] Init graphics (Load skybox, load some meshes)
	*/
	static void Init();

	static void Stop();

	/**
	* @brief [Internal] Set default values (fog, skybox, ...), called on init and on unloading a project
	*/
	static void SetDefaultValues();

	/**
	* @brief Set skybox TODO move this function?
	*/
	API static void SetSkybox(const std::shared_ptr<SkyBox>&  skybox_);

	static void OnLightingSettingsReflectionUpdate();

	/**
	* @brief Draw all Drawable elements
	*/
	static void Draw();

	/**
	* @brief Order all drawables
	*/
	static void OrderDrawables();

	/**
	* @brief Sort transparent drawables
	*/
	static void SortTransparentDrawables();

	/**
	* @brief Delete all drawables
	*/
	static void DeleteAllDrawables();

	/**
	* @brief Add a drawable
	* @param drawableToAdd Drawable to add
	*/
	static void AddDrawable(IDrawable* drawableToAdds);

	/**
	* @brief Remove a drawable
	* @param drawableToRemove Drawable to remove
	*/
	static void RemoveDrawable(const IDrawable* drawableToRemove);

	/**
	* @brief Add a lod
	* @param lodToAddLlod to add
	*/
	static void AddLod(const std::weak_ptr<Lod>& lodToAdd);

	/**
	* @brief Remove a lod
	* @param lodToRemove Lod to remove
	*/
	static void RemoveLod(const std::weak_ptr<Lod>& lodToRemove);

	/**
	* @brief Remove a camera
	* @param cameraToRemove Camera to remove
	*/
	static void RemoveCamera(const std::weak_ptr<Camera>& cameraToRemove);

	/**
	* @brief Draw a submesh
	* @param subMesh The submesh to draw
	* @param material The material to use
	* @param renderSettings The rendering settings
	* @param matrix The matrix to apply
	* @param forUI If the mesh is for UI
	*/
	static void DrawSubMesh(const MeshData::SubMesh& subMesh, Material& material, RenderingSettings& renderSettings, const glm::mat4& matrix, const glm::mat3& normalMatrix, const glm::mat4& mvpMatrix, bool forUI);
	
	/**
	* @brief Draw a submesh
	* @param subMesh The submesh to draw
	* @param material The material to use
	* @param texture The texture to use
	* @param renderSettings The rendering settings
	* @param matrix The matrix to apply
	* @param forUI If the mesh is for UI
	*/
	static void DrawSubMesh(const MeshData::SubMesh& subMesh, Material& material, Texture* texture, RenderingSettings& renderSettings, const glm::mat4& matrix, const glm::mat3& normalMatrix, const glm::mat4& mvpMatrix, bool forUI);


	static void DrawSubMesh(const Vector3& position, const Quaternion& rotation, const Vector3& scale, const MeshData::SubMesh& subMesh, Material& material, RenderingSettings& renderSettings);

	static std::vector<std::weak_ptr<Camera>> cameras;
	API static std::shared_ptr<Camera> usedCamera;
	static bool needUpdateCamera;

	static std::vector <IDrawable*> s_orderedIDrawable;
	static std::vector<std::weak_ptr<Lod>> s_lods;

	
	static Shader* s_currentShader;
	static Material* s_currentMaterial;
	static IDrawableTypes s_currentMode;
	static bool s_isRenderingBatchDirty;
	static GraphicsSettings s_settings;
	static size_t s_currentFrame;

	static std::vector <Light*> s_directionalLights;
	static void CreateLightLists();
	static bool s_isLightUpdateNeeded;
	static void SetIsGridRenderingEnabled(bool enabled);
	[[nodiscard]] static bool IsGridRenderingEnabled();

private:
	static bool s_isGridRenderingEnabled;
	static float s_gridAlphaMultiplier;

	static void OnProjectLoaded();

	/**
	* @brief Draw skybox
	* @param cameraPosition The camera position
	*/
	static void DrawSkybox(const Vector3& cameraPosition);

	/**
	* @brief Check lods
	*/
	static void CheckLods();

	static void UpdateShadersCameraMatrices();

#if defined(EDITOR)
	/**
	* @brief Draw selected item bounding box
	* @param cameraPosition The camera position
	*/
	static void DrawSelectedItemBoundingBox();

	/**
	* @brief Draw editor grid
	* @param cameraPosition The camera position
	* @param gridAxis The grid axis
	*/
	static void DrawEditorGrid(const Vector3& cameraPosition, int gridAxis);

	/**
	* @brief Draw editor tool
	* @param cameraPosition The camera position
	*/
	static void DrawEditorTool(const Vector3& cameraPosition);
#endif

	static int s_iDrawablesCount;
	static int s_lodsCount;
	static bool s_drawOrderListDirty;
};
