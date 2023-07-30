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
	static std::weak_ptr<Camera> usedCamera;
	static bool needUpdateCamera; // TODO change this when usedCamera is changed

	// static int usedShaderProgram;
	static void SetSkybox(SkyBox *skybox_);
	static void Init();
	static void DrawAllDrawable();
	// static Material *usedMaterial;
	static void OrderDrawables();
	static void AddDrawable(std::weak_ptr<IDrawable> drawableToPlace);
	static void RemoveDrawable(std::weak_ptr<IDrawable> drawableToPlace);
	static void ChangeFrameBufferSize(Vector2Int resolution);
	static void UpdaterameBuffer();

	static std::vector<std::weak_ptr<IDrawable>> orderedIDrawable;
	static int iDrawablesCount;

	static SkyBox *skybox;
	static unsigned int framebuffer;
	static unsigned int depthframebuffer;
	static unsigned int framebufferTexture;
	static Vector2Int framebufferSize;
	static bool needFremeBufferUpdate;
private:

};
