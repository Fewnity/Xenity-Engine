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

class Graphics
{
public:

	/**
	* [Internal] Init graphics (Load skybox, load some meshes)
	*/
	static void Init();

	/**
	* [Internal] Set default values (fog, skybox, ...), called on init and on unloading a project
	*/
	static void SetDefaultValues();

	/**
	* Set skybox TODO move this function?
	*/
	API static void SetSkybox(const std::shared_ptr<SkyBox>&  skybox_);

	static ReflectiveData GetLightingSettingsReflection();
	static void OnLightingSettingsReflectionUpdate();

	/**
	* Draw all Drawable elements
	*/
	static void Draw();

	/**
	* Order all drawables
	*/
	static void OrderDrawables();

	/**
	* Sort transparent drawables
	*/
	static void SortTransparentDrawables();

	/**
	* Delete all drawables
	*/
	static void DeleteAllDrawables();

	/**
	* Add a drawable
	* @param drawableToAdd Drawable to add
	*/
	static void AddDrawable(const std::weak_ptr<IDrawable>& drawableToAdd);

	/**
	* Remove a drawable
	* @param drawableToRemove Drawable to remove
	*/
	static void RemoveDrawable(const std::weak_ptr<IDrawable>& drawableToRemove);

	/**
	* Add a lod
	* @param lodToAddLlod to add
	*/
	static void AddLod(const std::weak_ptr<Lod>& lodToAdd);

	/**
	* Remove a lod
	* @param lodToRemove Lod to remove
	*/
	static void RemoveLod(const std::weak_ptr<Lod>& lodToRemove);

	/**
	* Remove a camera
	* @param cameraToRemove Camera to remove
	*/
	static void RemoveCamera(const std::weak_ptr<Camera>& cameraToRemove);

	/**
	* Draw a submesh
	* @param subMesh The submesh to draw
	* @param material The material to use
	* @param renderSettings The rendering settings
	* @param matrix The matrix to apply
	* @param forUI If the mesh is for UI
	*/
	static void DrawSubMesh(const MeshData::SubMesh& subMesh, const std::shared_ptr<Material>& material, RenderingSettings& renderSettings, const glm::mat4& matrix, bool forUI);
	
	/**
	* Draw a submesh
	* @param subMesh The submesh to draw
	* @param material The material to use
	* @param texture The texture to use
	* @param renderSettings The rendering settings
	* @param matrix The matrix to apply
	* @param forUI If the mesh is for UI
	*/
	static void DrawSubMesh(const MeshData::SubMesh& subMesh, const std::shared_ptr<Material>& material, std::shared_ptr<Texture> texture, RenderingSettings& renderSettings, const glm::mat4& matrix, bool forUI);

	/**
	* Set draw order list as dirty
	*/
	static void SetDrawOrderListAsDirty();

	static bool UseOpenGLFixedFunctions;

	static std::vector<std::weak_ptr<Camera>> cameras;
	API static std::shared_ptr<Camera> usedCamera;
	static bool needUpdateCamera;

	static std::vector<std::weak_ptr<IDrawable>> noTransparentDrawable;
	static std::vector<std::weak_ptr<IDrawable>> transparentDrawable;
	static std::vector<std::weak_ptr<IDrawable>> spriteDrawable;
	static std::vector<std::weak_ptr<IDrawable>> uiDrawable;

	static std::vector<std::weak_ptr<IDrawable>> orderedIDrawable;
	static std::vector<std::weak_ptr<Lod>> lods;

	static std::shared_ptr <SkyBox> skybox;

	static bool isFogEnabled;
	static float fogStart;
	static float fogEnd;
	static Color fogColor;
	static Color skyColor;
	static std::shared_ptr <Shader> currentShader;
	static std::shared_ptr <Material> currentMaterial;
	static IDrawableTypes currentMode;
	static bool isRenderingBatchDirty;

private:
	/**
	* Draw skybox
	* @param cameraPosition The camera position
	*/
	static void DrawSkybox(const Vector3& cameraPosition);

	/**
	* Check lods
	*/
	static void CheckLods();

#if defined(EDITOR)
	/**
	* Draw selected item bounding box
	* @param cameraPosition The camera position
	*/
	static void DrawSelectedItemBoundingBox(const Vector3& cameraPosition);

	/**
	* Draw editor grid
	* @param cameraPosition The camera position
	* @param gridAxis The grid axis
	*/
	static void DrawEditorGrid(const Vector3& cameraPosition, int gridAxis);

	/**
	* Draw editor tool
	* @param cameraPosition The camera position
	*/
	static void DrawEditorTool(const Vector3& cameraPosition);
#endif

	static int iDrawablesCount;
	static int lodsCount;
	static bool drawOrderListDirty;
};
