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

class API SkyBox
{
public:
	/**
	* Create a skybox
	* @param front Front face
	* @param back Back face
	* @param up Up face
	* @param down Down face
	* @param left Left face
	* @param right Right face
	*/
	SkyBox(Texture *front, Texture *back, Texture *up, Texture *down, Texture *left, Texture *right);
	Texture *front = nullptr;
	Texture *back = nullptr;
	Texture *up = nullptr;
	Texture *down = nullptr;
	Texture *left = nullptr;
	Texture *right = nullptr;
};

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
	static void SetSkybox(SkyBox *skybox_);

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

	static SkyBox *skybox;

	// static int usedShaderProgram;
	// static Material *usedMaterial;
private:
	static int iDrawablesCount;

};
