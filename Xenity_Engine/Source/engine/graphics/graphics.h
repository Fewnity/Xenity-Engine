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

class IDrawable;
class Material;
class Camera;
class Texture;
class SkyBox;

class API Graphics
{
public:

	/**
	* Init graphics (Load skybox, load some meshes)
	*/
	static void Init();

	/**
	* Set skybox
	*/
	static void SetSkybox(std::shared_ptr <SkyBox> skybox_);

	static std::unordered_map<std::string, ReflectionEntry> GetLightingSettingsReflection();
	static void OnLightingSettingsReflectionUpdate();

	/**
	* Draw all Drawable elements
	*/
	static void DrawAllDrawable();

	/**
	* Order all drawables
	*/
	static void OrderDrawables();

	/**
	* Add a drawable
	* @param drawableToAdd Drawable to add
	*/
	static void AddDrawable(std::weak_ptr<IDrawable> drawableToAdd);

	/**
	* Remove a drawable
	* @param drawableToRemove Drawable to remove
	*/
	static void RemoveDrawable(std::weak_ptr<IDrawable> drawableToRemove);

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

	// static int usedShaderProgram;
	// static Material *usedMaterial;
private:
	static int iDrawablesCount;

};
