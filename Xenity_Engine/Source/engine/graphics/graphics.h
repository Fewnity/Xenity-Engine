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

	static void DrawMesh(const std::shared_ptr<MeshData>& meshData, const std::vector<std::shared_ptr<Texture>>& textures, RenderingSettings& renderSettings, const glm::mat4& matrix, const std::shared_ptr<Material>& material, bool forUI);
	static void SetDrawOrderListAsDirty();

	static bool UseOpenGLFixedFunctions;

	static std::vector<std::weak_ptr<Camera>> cameras;
	API static std::weak_ptr<Camera> usedCamera;
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

private:
	static int iDrawablesCount;
	static int lodsCount;
	static bool drawOrderListDirty;
	static void DrawSkybox(const Vector3& cameraPosition);
	static void CheckLods();
#if defined(EDITOR)
	static void DrawSelectedItemBoundingBox(const Vector3& cameraPosition);
	static void DrawEditorGrid(const Vector3& cameraPosition, int gridAxis);
	static void DrawEditorTool(const Vector3& cameraPosition);
#endif
};
