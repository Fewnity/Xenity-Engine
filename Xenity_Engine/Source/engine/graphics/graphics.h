#pragma once

#include <vector>

class IDrawable;
class Material;
class Camera;
class Texture;

class SkyBox
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

class Graphics
{
public:
	static Camera *usedCamera;
	static int usedShaderProgram;
	static void SetSkybox(SkyBox *skybox_);
	static void Init();
	static void DrawAllDrawable();
	static Material *usedMaterial;
	static void OrderDrawables();
	static void OrderOneDrawable(IDrawable *drawable);
	static void AddDrawable(IDrawable *drawableToPlace);

	static std::vector<IDrawable *> orderedIDrawable;
	static int iDrawablesCount;

	static SkyBox *skybox;
};
