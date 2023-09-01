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
	SkyBox(std::shared_ptr<Texture> front, std::shared_ptr<Texture> back, std::shared_ptr<Texture> up, std::shared_ptr<Texture> down, std::shared_ptr<Texture> left, std::shared_ptr<Texture> right);
	std::shared_ptr<Texture> front = nullptr;
	std::shared_ptr<Texture> back = nullptr;
	std::shared_ptr<Texture> up = nullptr;
	std::shared_ptr<Texture> down = nullptr;
	std::shared_ptr<Texture> left = nullptr;
	std::shared_ptr<Texture> right = nullptr;
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
