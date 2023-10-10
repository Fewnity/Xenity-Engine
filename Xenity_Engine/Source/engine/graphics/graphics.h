#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <memory>
#include <vector>
#include "../vectors/vector2_int.h"
#include "iDrawableTypes.h"

class IDrawable;
class Material;
class Camera;
class Texture;
class SkyBox;
class Vector3;
class Shader;

class API Graphics
{
public:

	/**
	* Init graphics (Load skybox, load some meshes)
	*/
	static void Init();
	static void SetDefaultValues();

	/**
	* Set skybox
	*/
	static void SetSkybox(std::shared_ptr <SkyBox> skybox_);

	static std::unordered_map<std::string, ReflectionEntry> GetLightingSettingsReflection();
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

	static std::vector<std::weak_ptr<Camera>> cameras;
	static std::weak_ptr<Camera> usedCamera;
	static bool needUpdateCamera;

	static std::vector<std::weak_ptr<IDrawable>> orderedIDrawable;

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
	static void DrawSkybox(const Vector3& cameraPosition);
	static void DrawEditorGrid(const Vector3& cameraPosition);
	static void DrawEditorTool(const Vector3& cameraPosition);
};
