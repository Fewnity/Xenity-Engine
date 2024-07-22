// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

/**
 * [Internal]
 */

#include <engine/api.h>

#include <memory>
#include <vector>
#include <engine/vectors/vector2_int.h>
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
	static void AddDrawable(const std::shared_ptr<IDrawable>& drawableToAdd);

	/**
	* @brief Remove a drawable
	* @param drawableToRemove Drawable to remove
	*/
	static void RemoveDrawable(const std::shared_ptr<IDrawable>& drawableToRemove);

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
	static void DrawSubMesh(const MeshData::SubMesh& subMesh, Material& material, RenderingSettings& renderSettings, const glm::mat4& matrix, bool forUI);
	
	/**
	* @brief Draw a submesh
	* @param subMesh The submesh to draw
	* @param material The material to use
	* @param texture The texture to use
	* @param renderSettings The rendering settings
	* @param matrix The matrix to apply
	* @param forUI If the mesh is for UI
	*/
	static void DrawSubMesh(const MeshData::SubMesh& subMesh, Material& material, std::shared_ptr<Texture> texture, RenderingSettings& renderSettings, const glm::mat4& matrix, bool forUI);

	/**
	* @brief Set draw order list as dirty
	*/
	static void SetDrawOrderListAsDirty();

#if defined(__PSP__) || defined(_EE)
	static constexpr bool UseOpenGLFixedFunctions = true;
#else
	static constexpr bool UseOpenGLFixedFunctions = false;
#endif

	static std::vector<std::weak_ptr<Camera>> cameras;
	API static std::shared_ptr<Camera> usedCamera;
	static bool needUpdateCamera;

	static std::vector <IDrawable*> orderedIDrawable;
	static std::vector<std::weak_ptr<Lod>> lods;

	
	static std::shared_ptr <Shader> currentShader;
	static std::shared_ptr <Material> currentMaterial;
	static IDrawableTypes currentMode;
	static bool isRenderingBatchDirty;

	static GraphicsSettings settings;

private:

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

#if defined(EDITOR)
	/**
	* @brief Draw selected item bounding box
	* @param cameraPosition The camera position
	*/
	static void DrawSelectedItemBoundingBox(const Vector3& cameraPosition);

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

	static int iDrawablesCount;
	static int lodsCount;
	static bool drawOrderListDirty;
};
